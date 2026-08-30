#include "terminal_ui.h"

#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 1024
#define MAX_ARGUMENTS 4

static void print_separator(void) {
    puts("------------------------------------------------------------");
}

static void print_header(void) {
    print_separator();
    puts("                    GERENCIADOR DE ARQUIVOS");
    puts("                     Navegador de terminal");
    print_separator();
}

static void print_help(void) {
    puts("Comandos:");
    puts("  ls [caminho]       Lista o diretorio atual ou um caminho");
    puts("  cd <caminho>       Entra em um diretorio");
    puts("  pwd                Mostra o diretorio atual");
    puts("  touch <caminho>    Cria um arquivo vazio");
    puts("  mkdir <caminho>    Cria um diretorio");
    puts("  help               Mostra esta ajuda");
    puts("  exit               Encerra a aplicacao");
}

static void print_result(FileManagerResult result) {
    if (result.success) {
        printf("\n[OK] %s\n", result.message);
    } else {
        fprintf(stderr, "\n[ERROR] %s\n", result.message);
    }

    if (result.path[0] != '\0') {
        printf("Caminho: %s\n", result.path);
    }
}

static void print_list(const FileManagerList *list) {
    size_t index = 0;

    printf("\n%-12s %s\n", "TIPO", "NOME");
    print_separator();

    if (list->count == 0) {
        puts("(diretorio vazio)");
        return;
    }

    for (index = 0; index < list->count; index++) {
        printf("%-12s %s\n", fm_entry_type_label(list->entries[index].type), list->entries[index].name);
    }
}

static void handle_list(const FileManagerSession *session, const char *path) {
    FileManagerList list;
    FileManagerResult result = fm_list_directory(session, path, &list);

    if (!result.success) {
        print_result(result);
        return;
    }

    print_list(&list);
    fm_list_free(&list);
}

static int split_input(char *input, char *tokens[MAX_ARGUMENTS]) {
    char *token = strtok(input, " \t");
    int count = 0;

    while (token != NULL && count < MAX_ARGUMENTS) {
        tokens[count++] = token;
        token = strtok(NULL, " \t");
    }

    return count;
}

int fm_terminal_ui_run(FileManagerSession *session) {
    char input[INPUT_SIZE];

    if (session == NULL) {
        return 1;
    }

    print_header();
    print_help();
    handle_list(session, NULL);

    while (1) {
        char *tokens[MAX_ARGUMENTS] = {0};
        int token_count = 0;

        printf("\nfm [%s]\n> ", session->current_directory);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            break;
        }

        input[strcspn(input, "\n")] = '\0';
        token_count = split_input(input, tokens);
        if (token_count == 0) {
            continue;
        }

        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }
        if (strcmp(tokens[0], "help") == 0) {
            print_help();
            continue;
        }
        if (strcmp(tokens[0], "pwd") == 0) {
            printf("%s\n", session->current_directory);
            continue;
        }
        if (strcmp(tokens[0], "ls") == 0) {
            handle_list(session, token_count >= 2 ? tokens[1] : NULL);
            continue;
        }
        if (strcmp(tokens[0], "cd") == 0) {
            if (token_count < 2) {
                fputs("[ERROR] caminho ausente\n", stderr);
                continue;
            }
            print_result(fm_change_directory(session, tokens[1]));
            continue;
        }
        if (strcmp(tokens[0], "touch") == 0) {
            if (token_count < 2) {
                fputs("[ERROR] caminho ausente\n", stderr);
                continue;
            }
            print_result(fm_create_file(session, tokens[1]));
            continue;
        }
        if (strcmp(tokens[0], "mkdir") == 0) {
            if (token_count < 2) {
                fputs("[ERROR] caminho ausente\n", stderr);
                continue;
            }
            print_result(fm_create_directory(session, tokens[1]));
            continue;
        }

        fprintf(stderr, "[ERROR] comando desconhecido: %s\n", tokens[0]);
    }

    puts("Sessao encerrada.");
    return 0;
}
