#ifndef FILE_MANAGER_APPLICATION_H
#define FILE_MANAGER_APPLICATION_H

#include "filesystem_port.h"

typedef enum FileManagerAppResultKind {
    FM_APP_USAGE = 0,
    FM_APP_UNKNOWN,
    FM_APP_HELP,
    FM_APP_EXIT,
    FM_APP_PWD,
    FM_APP_LIST,
    FM_APP_INFO,
    FM_APP_OPERATION,
    FM_APP_CONFIRM
} FileManagerAppResultKind;

typedef struct FileManagerAppResult {
    FileManagerAppResultKind kind;
    FileManagerResult operation;
    FileManagerList list;
    FileManagerEntry info;
    char value[FM_PATH_SIZE];
    char message[FM_MESSAGE_SIZE];
} FileManagerAppResult;

/*
 * Executes one parsed command without terminal I/O.
 * Set confirmed=1 only after the presentation layer obtained user approval.
 * The caller owns list memory for FM_APP_LIST and must call fm_list_free().
 */
FileManagerAppResult fm_application_execute(
    FileManagerSession *session,
    const FileSystemPort *filesystem,
    int argc,
    char *const argv[],
    int confirmed
);

#endif
