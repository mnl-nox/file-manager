#include "filesystem_port.h"

/* POSIX implementation is kept behind this adapter boundary. */
static const FileSystemPort POSIX_PORT = {
    .change_directory = fm_change_directory,
    .list_directory = fm_list_directory,
    .create_file = fm_create_file,
    .create_directory = fm_create_directory,
    .copy_file = fm_copy_file,
    .move_entry = fm_move_entry,
    .remove_file = fm_remove_file,
    .remove_directory = fm_remove_directory,
    .get_info = fm_get_info
};

const FileSystemPort *fm_posix_filesystem_port(void) {
    return &POSIX_PORT;
}
