#include "command_parser.h"

#include <ctype.h>

int fm_parse_command(char *line, char **argv, size_t capacity) {
    char *readp, *writep;
    size_t argc = 0;
    if (!line || !argv || capacity == 0) return -3;
    readp = line;
    writep = line;

    while (*readp) {
        char quote = '\0';
        while (isspace((unsigned char)*readp)) ++readp;
        if (!*readp) break;
        if (argc == capacity) return -2;
        argv[argc++] = writep;

        while (*readp) {
            char c = *readp++;
            if (c == '\\' && quote != '\'') {
                if (!*readp) return -1;
                *writep++ = *readp++;
            } else if (quote) {
                if (c == quote) quote = '\0';
                else *writep++ = c;
            } else if (c == '\'' || c == '"') {
                quote = c;
            } else if (isspace((unsigned char)c)) {
                break;
            } else {
                *writep++ = c;
            }
        }
        if (quote) return -1;
        *writep++ = '\0';
    }
    return (int)argc;
}
