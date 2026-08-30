#define _POSIX_C_SOURCE 200809L

#include "file_manager.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef int (*test_fn)(void);

static char g_temp_dir[FM_PATH_SIZE];

static int assert_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "assertion failed: %s\n", message);
        return 0;
    }
    return 1;
}

static int path_exists(const char *path) {
    struct stat info;
    return stat(path, &info) == 0;
}

static int build_path(char output[FM_PATH_SIZE], const char *base, const char *name) {
    int written = snprintf(output, FM_PATH_SIZE, "%s/%s", base, name);
    if (written < 0 || written >= FM_PATH_SIZE) {
        fprintf(stderr, "path is too long: %s/%s\n", base, name);
        return 0;
    }
    return 1;
}

static int reset_temp_dir(void) {
    char command[FM_PATH_SIZE + 32];
    int written = snprintf(command, sizeof(command), "rm -rf '%s' && mkdir -p '%s'", g_temp_dir, g_temp_dir);
    if (written < 0 || (size_t)written >= sizeof(command)) {
        fprintf(stderr, "failed to build cleanup command\n");
        return 0;
    }
    return system(command) == 0;
}

static int test_session_starts_in_current_directory(void) {
    FileManagerSession session;
    char cwd[FM_PATH_SIZE];
    FileManagerResult result = fm_session_init(&session);

    if (!assert_true(result.success, "session should initialize")) {
        return 0;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 0;
    }

    return assert_true(strcmp(session.current_directory, cwd) == 0, "session path should match cwd");
}

static int test_create_file_and_directory(void) {
    FileManagerSession session;
    FileManagerResult file_result;
    FileManagerResult dir_result;
    char file_path[FM_PATH_SIZE];
    char dir_path[FM_PATH_SIZE];

    if (!reset_temp_dir()) {
        return 0;
    }

    snprintf(session.current_directory, sizeof(session.current_directory), "%s", g_temp_dir);

    file_result = fm_create_file(&session, "notes.txt");
    dir_result = fm_create_directory(&session, "docs");

    if (!build_path(file_path, g_temp_dir, "notes.txt")) {
        return 0;
    }

    if (!build_path(dir_path, g_temp_dir, "docs")) {
        return 0;
    }

    if (!assert_true(file_result.success, "file should be created")) {
        return 0;
    }

    if (!assert_true(dir_result.success, "directory should be created")) {
        return 0;
    }

    if (!assert_true(path_exists(file_path), "created file should exist")) {
        return 0;
    }

    return assert_true(path_exists(dir_path), "created directory should exist");
}

static int test_list_directory_reports_types(void) {
    FileManagerSession session;
    FileManagerList list;
    FileManagerResult list_result;
    size_t index = 0;
    int saw_file = 0;
    int saw_directory = 0;

    if (!reset_temp_dir()) {
        return 0;
    }

    snprintf(session.current_directory, sizeof(session.current_directory), "%s", g_temp_dir);

    if (!fm_create_file(&session, "alpha.txt").success) {
        fprintf(stderr, "failed to create file fixture\n");
        return 0;
    }

    if (!fm_create_directory(&session, "bravo").success) {
        fprintf(stderr, "failed to create directory fixture\n");
        return 0;
    }

    list_result = fm_list_directory(&session, NULL, &list);
    if (!assert_true(list_result.success, "list should succeed")) {
        return 0;
    }

    for (index = 0; index < list.count; index++) {
        if (strcmp(list.entries[index].name, "alpha.txt") == 0 &&
            list.entries[index].type == FM_ENTRY_FILE) {
            saw_file = 1;
        }
        if (strcmp(list.entries[index].name, "bravo") == 0 &&
            list.entries[index].type == FM_ENTRY_DIRECTORY) {
            saw_directory = 1;
        }
    }

    fm_list_free(&list);

    if (!assert_true(saw_file, "list should include created file")) {
        return 0;
    }

    return assert_true(saw_directory, "list should include created directory");
}

static int test_change_directory_validates_destination(void) {
    FileManagerSession session;
    FileManagerResult result;

    if (!reset_temp_dir()) {
        return 0;
    }

    snprintf(session.current_directory, sizeof(session.current_directory), "%s", g_temp_dir);

    result = fm_change_directory(&session, "missing");
    if (!assert_true(!result.success, "changing to missing directory should fail")) {
        return 0;
    }

    if (!fm_create_directory(&session, "nested").success) {
        fprintf(stderr, "failed to create nested directory fixture\n");
        return 0;
    }

    result = fm_change_directory(&session, "nested");
    if (!assert_true(result.success, "changing to existing directory should succeed")) {
        return 0;
    }

    return assert_true(strstr(session.current_directory, "/nested") != NULL, "session should move into nested directory");
}

int main(void) {
    test_fn tests[] = {
        test_session_starts_in_current_directory,
        test_create_file_and_directory,
        test_list_directory_reports_types,
        test_change_directory_validates_destination
    };
    const char *names[] = {
        "session_starts_in_current_directory",
        "create_file_and_directory",
        "list_directory_reports_types",
        "change_directory_validates_destination"
    };
    const size_t test_count = sizeof(tests) / sizeof(tests[0]);
    size_t index = 0;

    snprintf(g_temp_dir, sizeof(g_temp_dir), "/tmp/file-manager-tests-%ld", (long)getpid());

    for (index = 0; index < test_count; index++) {
        if (!tests[index]()) {
            fprintf(stderr, "test failed: %s\n", names[index]);
            return 1;
        }
    }

    puts("all tests passed");
    return 0;
}
