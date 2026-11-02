#include "file_manager_application.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static FileManagerAppResult app_result(FileManagerAppResultKind kind) {
    FileManagerAppResult r;
    memset(&r, 0, sizeof(r));
    r.kind = kind;
    return r;
}

static FileManagerAppResult usage(const char *text) {
    FileManagerAppResult r = app_result(FM_APP_USAGE);
    (void)snprintf(r.message, sizeof(r.message), "%s", text);
    return r;
}

static FileManagerAppResult operation(FileManagerResult result) {
    FileManagerAppResult r = app_result(FM_APP_OPERATION);
    r.operation = result;
    return r;
}

FileManagerAppResult fm_application_execute(
    FileManagerSession *session,
    int argc,
    char *const argv[],
    int confirmed
) {
    FileManagerAppResult r;
    if (!session || argc < 1 || !argv || !argv[0])
        return usage("comando ou sessao invalida");

    if (!strcmp(argv[0], "help")) {
        return argc == 1 ? app_result(FM_APP_HELP) : usage("Uso: help");
    }
    if (!strcmp(argv[0], "exit")) {
        return argc == 1 ? app_result(FM_APP_EXIT) : usage("Uso: exit");
    }
    if (!strcmp(argv[0], "pwd")) {
        if (argc != 1) return usage("Uso: pwd");
        r = app_result(FM_APP_PWD);
        (void)snprintf(r.value, sizeof(r.value), "%s", session->current_directory);
        return r;
    }
    if (!strcmp(argv[0], "ls")) {
        if (argc > 2) return usage("Uso: ls [caminho]");
        r = app_result(FM_APP_LIST);
        r.operation = fm_list_directory(session, argc == 2 ? argv[1] : NULL, &r.list);
        if (!r.operation.success) r.kind = FM_APP_OPERATION;
        return r;
    }
    if (!strcmp(argv[0], "cd")) {
        if (argc != 2) return usage("Uso: cd <diretorio>");
        return operation(fm_change_directory(session, argv[1]));
    }
    if (!strcmp(argv[0], "touch")) {
        if (argc != 2) return usage("Uso: touch <arquivo>");
        return operation(fm_create_file(session, argv[1]));
    }
    if (!strcmp(argv[0], "mkdir")) {
        if (argc != 2) return usage("Uso: mkdir <diretorio>");
        return operation(fm_create_directory(session, argv[1]));
    }
    if (!strcmp(argv[0], "cp")) {
        if (argc != 3) return usage("Uso: cp <origem> <destino>");
        return operation(fm_copy_file(session, argv[1], argv[2]));
    }
    if (!strcmp(argv[0], "mv")) {
        if (argc != 3) return usage("Uso: mv <origem> <destino>");
        return operation(fm_move_entry(session, argv[1], argv[2]));
    }
    if (!strcmp(argv[0], "rm")) {
        if (argc != 2) return usage("Uso: rm <arquivo|link>");
        if (!confirmed) {
            r = app_result(FM_APP_CONFIRM);
            (void)snprintf(r.value, sizeof(r.value), "%s", argv[1]);
            (void)snprintf(r.message, sizeof(r.message), "remover arquivo ou link");
            return r;
        }
        return operation(fm_remove_file(session, argv[1]));
    }
    if (!strcmp(argv[0], "rmdir")) {
        if (argc != 2) return usage("Uso: rmdir <diretorio vazio>");
        if (!confirmed) {
            r = app_result(FM_APP_CONFIRM);
            (void)snprintf(r.value, sizeof(r.value), "%s", argv[1]);
            (void)snprintf(r.message, sizeof(r.message), "remover diretorio vazio");
            return r;
        }
        return operation(fm_remove_directory(session, argv[1]));
    }
    if (!strcmp(argv[0], "stat")) {
        if (argc != 2) return usage("Uso: stat <caminho>");
        r = app_result(FM_APP_INFO);
        r.operation = fm_get_info(session, argv[1], &r.info);
        if (!r.operation.success) r.kind = FM_APP_OPERATION;
        return r;
    }

    r = app_result(FM_APP_UNKNOWN);
    (void)snprintf(r.message, sizeof(r.message), "comando desconhecido: %s", argv[0]);
    return r;
}
