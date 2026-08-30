#ifndef TERMINAL_UI_H
#define TERMINAL_UI_H

#include "file_manager.h"

/* Starts the interactive terminal interface for an initialized session. */
int fm_terminal_ui_run(FileManagerSession *session);

#endif
