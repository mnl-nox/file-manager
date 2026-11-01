#ifndef FILESYSTEM_PORT_H
#define FILESYSTEM_PORT_H

#include "file_manager.h"

/*
 * Application-facing filesystem boundary.
 * Implementations own the semantics documented by file_manager.h; callers
 * must release FileManagerList with fm_list_free().
 */
typedef struct FileSystemPort {
    FileManagerResult (*change_directory)(FileManagerSession *, const char *);
    FileManagerResult (*list_directory)(const FileManagerSession *, const char *, FileManagerList *);
    FileManagerResult (*create_file)(const FileManagerSession *, const char *);
    FileManagerResult (*create_directory)(const FileManagerSession *, const char *);
    FileManagerResult (*copy_file)(const FileManagerSession *, const char *, const char *);
    FileManagerResult (*move_entry)(const FileManagerSession *, const char *, const char *);
    FileManagerResult (*remove_file)(const FileManagerSession *, const char *);
    FileManagerResult (*remove_directory)(const FileManagerSession *, const char *);
    FileManagerResult (*get_info)(const FileManagerSession *, const char *, FileManagerEntry *);
} FileSystemPort;

const FileSystemPort *fm_posix_filesystem_port(void);

#endif
