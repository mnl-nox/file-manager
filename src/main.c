#include "file_manager.h"
#include "terminal_ui.h"

#include <stdio.h>

int main(void) {
    FileManagerSession session;
    FileManagerResult init_result = fm_session_init(&session);

    if (!init_result.success) {
        fprintf(stderr, "[ERROR] %s\n", init_result.message);
        return 1;
    }

    return fm_terminal_ui_run(&session);
}
