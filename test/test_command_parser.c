#include "command_parser.h"

#include <stdio.h>
#include <string.h>

static int expect(int condition, const char *message) {
    if (!condition) fprintf(stderr, "parser: %s\n", message);
    return condition;
}

int main(void) {
    char line1[] = "cp \"folder a/file.txt\" 'dest file.txt'";
    char line2[] = "touch path\\ with\\ spaces";
    char line3[] = "echo \"\"";
    char line4[] = "touch \"unfinished";
    char line5[] = "a b c d";
    char *argv[3] = {0};
    int n;

    n = fm_parse_command(line1, argv, 3);
    if (!expect(n == 3 && !strcmp(argv[1], "folder a/file.txt") &&
                !strcmp(argv[2], "dest file.txt"), "quoted arguments")) return 1;

    n = fm_parse_command(line2, argv, 3);
    if (!expect(n == 2 && !strcmp(argv[1], "path with spaces"), "escaped whitespace")) return 1;

    n = fm_parse_command(line3, argv, 3);
    if (!expect(n == 2 && argv[1][0] == '\0', "empty quoted argument")) return 1;

    n = fm_parse_command(line4, argv, 3);
    if (!expect(n == -1, "unclosed quote rejected")) return 1;

    n = fm_parse_command(line5, argv, 3);
    if (!expect(n == -2, "argument overflow rejected")) return 1;

    puts("Parser tests passed.");
    return 0;
}
