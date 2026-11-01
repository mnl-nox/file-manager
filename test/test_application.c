#define _POSIX_C_SOURCE 200809L
#include "file_manager_application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int expect(int ok, const char *message) {
    if (!ok) fprintf(stderr, "application: %s\n", message);
    return ok;
}

int main(void) {
    char temp[] = "/tmp/file-manager-app-XXXXXX";
    FileManagerSession session;
    FileManagerAppResult r;
    char *touch_argv[] = {"touch", "hello world.txt"};
    char *ls_argv[] = {"ls"};
    char *stat_argv[] = {"stat", "hello world.txt"};
    char *rm_argv[] = {"rm", "hello world.txt"};
    char *unknown_argv[] = {"no-such-command"};
    char path[FM_PATH_SIZE];
    struct stat st;

    if (!mkdtemp(temp)) { perror("mkdtemp"); return 1; }
    (void)snprintf(session.current_directory, sizeof(session.current_directory), "%s", temp);
    (void)snprintf(path, sizeof(path), "%s/hello world.txt", temp);

    r = fm_application_execute(&session, 2, touch_argv, 0);
    if (!expect(r.kind == FM_APP_OPERATION && r.operation.success && lstat(path, &st) == 0,
                "touch deve criar arquivo")) goto fail;

    r = fm_application_execute(&session, 1, ls_argv, 0);
    if (!expect(r.kind == FM_APP_LIST && r.operation.success && r.list.count == 1,
                "ls deve devolver lista")) goto fail;
    fm_list_free(&r.list);

    r = fm_application_execute(&session, 2, stat_argv, 0);
    if (!expect(r.kind == FM_APP_INFO && r.operation.success &&
                r.info.type == FM_ENTRY_FILE, "stat deve devolver metadados")) goto fail;

    r = fm_application_execute(&session, 2, rm_argv, 0);
    if (!expect(r.kind == FM_APP_CONFIRM, "rm deve pedir confirmacao")) goto fail;
    r = fm_application_execute(&session, 2, rm_argv, 1);
    if (!expect(r.kind == FM_APP_OPERATION && r.operation.success && lstat(path, &st) != 0,
                "rm confirmado deve remover")) goto fail;

    r = fm_application_execute(&session, 1, unknown_argv, 0);
    if (!expect(r.kind == FM_APP_UNKNOWN, "comando desconhecido deve ser tipado")) goto fail;

    if (rmdir(temp) != 0) { perror("rmdir"); return 1; }
    puts("Application tests passed.");
    return 0;

fail:
    (void)unlink(path);
    (void)rmdir(temp);
    return 1;
}
