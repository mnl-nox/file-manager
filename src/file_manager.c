#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include "file_manager.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static FileManagerResult fm_make_result(
    int success,
    int error_code,
    const char *path,
    const char *format,
    ...
) {
    FileManagerResult result;
    va_list args;

    result.success = success;
    result.error_code = error_code;
    result.path[0] = '\0';
    result.message[0] = '\0';

    if (path != NULL) {
        snprintf(result.path, sizeof(result.path), "%s", path);
    }

    va_start(args, format);
    vsnprintf(result.message, sizeof(result.message), format, args);
    va_end(args);

    return result;
}

static FileManagerResult fm_resolve_path(
    const FileManagerSession *session,
    const char *path,
    char output[FM_PATH_SIZE]
) {
    int written = 0;

    if (path == NULL || path[0] == '\0') {
        return fm_make_result(0, EINVAL, "", "path cannot be empty");
    }

    if (path[0] == '/') {
        written = snprintf(output, FM_PATH_SIZE, "%s", path);
    } else {
        written = snprintf(output, FM_PATH_SIZE, "%s/%s", session->current_directory, path);
    }

    if (written < 0 || written >= FM_PATH_SIZE) {
        output[0] = '\0';
        return fm_make_result(0, ENAMETOOLONG, path, "path is too long");
    }

    return fm_make_result(1, 0, output, "ok");
}

static int fm_compare_entries(const void *left, const void *right) {
    const FileManagerEntry *entry_left = (const FileManagerEntry *)left;
    const FileManagerEntry *entry_right = (const FileManagerEntry *)right;
    return strcmp(entry_left->name, entry_right->name);
}

FileManagerResult fm_session_init(FileManagerSession *session) {
    if (session == NULL) {
        return fm_make_result(0, EINVAL, "", "session cannot be null");
    }

    if (getcwd(session->current_directory, sizeof(session->current_directory)) == NULL) {
        return fm_make_result(0, errno, "", "failed to read current directory: %s", strerror(errno));
    }

    return fm_make_result(1, 0, session->current_directory, "session initialized");
}

FileManagerResult fm_change_directory(FileManagerSession *session, const char *path) {
    char resolved[FM_PATH_SIZE];
    struct stat info;
    FileManagerResult path_result;

    if (session == NULL) {
        return fm_make_result(0, EINVAL, "", "session cannot be null");
    }

    path_result = fm_resolve_path(session, path, resolved);
    if (!path_result.success) {
        return path_result;
    }

    if (stat(resolved, &info) != 0) {
        return fm_make_result(0, errno, resolved, "failed to access directory: %s", strerror(errno));
    }

    if (!S_ISDIR(info.st_mode)) {
        return fm_make_result(0, ENOTDIR, resolved, "path is not a directory");
    }

    if (realpath(resolved, session->current_directory) == NULL) {
        return fm_make_result(0, errno, resolved, "failed to resolve directory: %s", strerror(errno));
    }

    return fm_make_result(1, 0, session->current_directory, "directory changed");
}

FileManagerResult fm_list_directory(
    const FileManagerSession *session,
    const char *path,
    FileManagerList *list
) {
    char resolved[FM_PATH_SIZE];
    const char *target = NULL;
    DIR *directory = NULL;
    struct dirent *entry = NULL;
    size_t capacity = 0;

    if (session == NULL || list == NULL) {
        return fm_make_result(0, EINVAL, "", "session and list are required");
    }

    list->entries = NULL;
    list->count = 0;

    if (path == NULL || path[0] == '\0') {
        target = session->current_directory;
    } else {
        FileManagerResult path_result = fm_resolve_path(session, path, resolved);
        if (!path_result.success) {
            return path_result;
        }
        target = resolved;
    }

    directory = opendir(target);
    if (directory == NULL) {
        return fm_make_result(0, errno, target, "failed to open directory: %s", strerror(errno));
    }

    while ((entry = readdir(directory)) != NULL) {
        struct stat info;
        FileManagerEntry *expanded_entries = NULL;
        char child_path[FM_PATH_SIZE];
        int written = 0;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (list->count == capacity) {
            capacity = capacity == 0 ? 8 : capacity * 2;
            expanded_entries = realloc(list->entries, capacity * sizeof(*list->entries));
            if (expanded_entries == NULL) {
                closedir(directory);
                fm_list_free(list);
                return fm_make_result(0, ENOMEM, target, "failed to allocate memory");
            }
            list->entries = expanded_entries;
        }

        written = snprintf(child_path, sizeof(child_path), "%s/%s", target, entry->d_name);
        if (written < 0 || written >= FM_PATH_SIZE) {
            closedir(directory);
            fm_list_free(list);
            return fm_make_result(0, ENAMETOOLONG, target, "child path is too long");
        }

        if (stat(child_path, &info) != 0) {
            closedir(directory);
            fm_list_free(list);
            return fm_make_result(0, errno, child_path, "failed to read entry info: %s", strerror(errno));
        }

        snprintf(list->entries[list->count].name, FM_NAME_SIZE, "%s", entry->d_name);
        if (S_ISDIR(info.st_mode)) {
            list->entries[list->count].type = FM_ENTRY_DIRECTORY;
        } else if (S_ISREG(info.st_mode)) {
            list->entries[list->count].type = FM_ENTRY_FILE;
        } else {
            list->entries[list->count].type = FM_ENTRY_OTHER;
        }

        list->count++;
    }

    closedir(directory);
    qsort(list->entries, list->count, sizeof(*list->entries), fm_compare_entries);

    return fm_make_result(1, 0, target, "directory listed");
}

FileManagerResult fm_create_file(const FileManagerSession *session, const char *path) {
    char resolved[FM_PATH_SIZE];
    int descriptor = -1;
    FileManagerResult path_result;

    if (session == NULL) {
        return fm_make_result(0, EINVAL, "", "session cannot be null");
    }

    path_result = fm_resolve_path(session, path, resolved);
    if (!path_result.success) {
        return path_result;
    }

    descriptor = open(resolved, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (descriptor < 0) {
        return fm_make_result(0, errno, resolved, "failed to create file: %s", strerror(errno));
    }

    if (close(descriptor) != 0) {
        return fm_make_result(0, errno, resolved, "failed to close file: %s", strerror(errno));
    }

    return fm_make_result(1, 0, resolved, "file created");
}

FileManagerResult fm_create_directory(const FileManagerSession *session, const char *path) {
    char resolved[FM_PATH_SIZE];
    FileManagerResult path_result;

    if (session == NULL) {
        return fm_make_result(0, EINVAL, "", "session cannot be null");
    }

    path_result = fm_resolve_path(session, path, resolved);
    if (!path_result.success) {
        return path_result;
    }

    if (mkdir(resolved, 0755) != 0) {
        return fm_make_result(0, errno, resolved, "failed to create directory: %s", strerror(errno));
    }

    return fm_make_result(1, 0, resolved, "directory created");
}

void fm_list_free(FileManagerList *list) {
    if (list == NULL) {
        return;
    }

    free(list->entries);
    list->entries = NULL;
    list->count = 0;
}

const char *fm_entry_type_label(FileManagerEntryType type) {
    switch (type) {
        case FM_ENTRY_FILE:
            return "file";
        case FM_ENTRY_DIRECTORY:
            return "directory";
        default:
            return "other";
    }
}
