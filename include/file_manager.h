#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stddef.h>
#include <sys/types.h>
#include <time.h>

#define FM_PATH_SIZE 4096
#define FM_NAME_SIZE 256
#define FM_MESSAGE_SIZE 256

typedef enum FileManagerEntryType {
    FM_ENTRY_FILE = 0,
    FM_ENTRY_DIRECTORY,
    FM_ENTRY_SYMLINK,
    FM_ENTRY_OTHER
} FileManagerEntryType;

typedef struct FileManagerEntry {
    char name[FM_NAME_SIZE];
    FileManagerEntryType type;
    off_t size;
    mode_t mode;
    time_t modified_at;
} FileManagerEntry;

typedef struct FileManagerList {
    FileManagerEntry *entries;
    size_t count;
} FileManagerList;

typedef struct FileManagerResult {
    int success;
    int error_code;
    char path[FM_PATH_SIZE];
    char message[FM_MESSAGE_SIZE];
} FileManagerResult;

typedef struct FileManagerSession {
    char current_directory[FM_PATH_SIZE];
} FileManagerSession;

FileManagerResult fm_session_init(FileManagerSession *session);
FileManagerResult fm_change_directory(FileManagerSession *session, const char *path);
FileManagerResult fm_list_directory(const FileManagerSession *session, const char *path, FileManagerList *list);
FileManagerResult fm_create_file(const FileManagerSession *session, const char *path);
FileManagerResult fm_create_directory(const FileManagerSession *session, const char *path);
FileManagerResult fm_copy_file(const FileManagerSession *session, const char *source, const char *destination);
FileManagerResult fm_move_entry(const FileManagerSession *session, const char *source, const char *destination);
FileManagerResult fm_remove_file(const FileManagerSession *session, const char *path);
FileManagerResult fm_remove_directory(const FileManagerSession *session, const char *path);
FileManagerResult fm_get_info(const FileManagerSession *session, const char *path, FileManagerEntry *info);
void fm_list_free(FileManagerList *list);
const char *fm_entry_type_label(FileManagerEntryType type);

#endif
