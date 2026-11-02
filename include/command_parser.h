#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stddef.h>

/*
 * Tokenizes a command line in place.
 * Returns argc, -1 for an incomplete quote/escape, -2 for too many arguments,
 * or -3 for invalid pointers/capacity.
 */
int fm_parse_command(char *line, char **argv, size_t capacity);

#endif
