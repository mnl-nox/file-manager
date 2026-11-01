#define _POSIX_C_SOURCE 200809L
#include "terminal_ui.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define INPUT_SIZE 4096
#define MAX_ARGUMENTS 4

static void separator(void) { puts("--------------------------------------------------------------------------"); }

static void help(void) {
    puts("Comandos:");
    puts("  ls [caminho]             Lista entradas e metadados");
    puts("  cd <diretorio>           Altera o diretorio atual");
    puts("  pwd                      Mostra o diretorio atual");
    puts("  touch <arquivo>          Cria arquivo vazio (nao sobrescreve)");
    puts("  mkdir <diretorio>        Cria diretorio");
    puts("  cp <origem> <destino>    Copia arquivo regular (destino novo)");
    puts("  mv <origem> <destino>    Move ou renomeia sem sobrescrever");
    puts("  rm <arquivo|link>        Remove arquivo ou link simbolico");
    puts("  rmdir <diretorio>        Remove somente diretorio vazio");
    puts("  stat <caminho>           Exibe metadados");
    puts("  help                     Exibe esta ajuda");
    puts("  exit                     Encerra");
    puts("");
    puts("Aspas simples/duplas e escape com barra invertida sao aceitos.");
    puts("Links simbolicos sao listados como links; cp nao os segue.");
}

static void print_result(FileManagerResult r) {
    if (r.success) printf("[OK] %s\n", r.message);
    else fprintf(stderr, "[ERRO] %s (errno=%d)\n", r.message, r.error_code);
    if (r.path[0]) printf("  Caminho: %s\n", r.path);
}

static void print_entry(const FileManagerEntry *e) {
    char date[32] = "-";
    struct tm tm_value;
    if (localtime_r(&e->modified_at, &tm_value))
        (void)strftime(date, sizeof(date), "%Y-%m-%d %H:%M", &tm_value);
    printf("%-10s %10lld %04o %-16s %s\n",
           fm_entry_type_label(e->type), (long long)e->size,
           (unsigned)(e->mode & 07777), date, e->name);
}

static void list_directory(const FileManagerSession *s, const char *path) {
    FileManagerList list;
    FileManagerResult r = fm_list_directory(s, path, &list);
    size_t i;
    if (!r.success) { print_result(r); return; }
    printf("%-10s %10s %4s %-16s %s\n", "TIPO", "BYTES", "MODO", "MODIFICADO", "NOME");
    separator();
    if (!list.count) puts("(diretorio vazio)");
    for (i = 0; i < list.count; ++i) print_entry(&list.entries[i]);
    printf("%zu entrada(s)\n", list.count);
    fm_list_free(&list);
}

static int lex(char *line, char *argv[MAX_ARGUMENTS]) {
    char *readp = line, *writep = line;
    int argc = 0;
    while (*readp) {
        char quote = '\0';
        while (isspace((unsigned char)*readp)) ++readp;
        if (!*readp) break;
        if (argc == MAX_ARGUMENTS) return -2;
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
    return argc;
}

static int confirm_action(const char *verb, const char *path) {
    char answer[16];
    printf("%s '%s'? [y/N] ", verb, path);
    if (!fgets(answer, sizeof(answer), stdin)) return 0;
    return answer[0] == 'y' || answer[0] == 'Y';
}

int fm_terminal_ui_run(FileManagerSession *session) {
    char line[INPUT_SIZE];
    if (!session) return 1;
    separator();
    puts("FILE MANAGER | POSIX / C11");
    separator();
    help();
    list_directory(session, NULL);

    for (;;) {
        char *argv[MAX_ARGUMENTS] = {0};
        int argc, ch;
        printf("\nfm:%s$ ", session->current_directory);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) { putchar('\n'); break; }
        if (!strchr(line, '\n') && !feof(stdin)) {
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            fputs("[ERRO] comando excede o limite de entrada\n", stderr);
            continue;
        }
        argc = lex(line, argv);
        if (argc == 0) continue;
        if (argc < 0) {
            fputs(argc == -1 ? "[ERRO] aspas ou escape incompleto\n" : "[ERRO] argumentos demais\n", stderr);
            continue;
        }
        if (!strcmp(argv[0], "exit")) {
            if (argc != 1) { puts("Uso: exit"); continue; }
            break;
        } else if (!strcmp(argv[0], "help")) {
            help();
        } else if (!strcmp(argv[0], "pwd")) {
            if (argc != 1) puts("Uso: pwd");
            else puts(session->current_directory);
        } else if (!strcmp(argv[0], "ls")) {
            if (argc > 2) puts("Uso: ls [caminho]");
            else list_directory(session, argc == 2 ? argv[1] : NULL);
        } else if (!strcmp(argv[0], "cd")) {
            if (argc != 2) puts("Uso: cd <diretorio>");
            else print_result(fm_change_directory(session, argv[1]));
        } else if (!strcmp(argv[0], "touch")) {
            if (argc != 2) puts("Uso: touch <arquivo>");
            else print_result(fm_create_file(session, argv[1]));
        } else if (!strcmp(argv[0], "mkdir")) {
            if (argc != 2) puts("Uso: mkdir <diretorio>");
            else print_result(fm_create_directory(session, argv[1]));
        } else if (!strcmp(argv[0], "cp")) {
            if (argc != 3) puts("Uso: cp <origem> <destino>");
            else print_result(fm_copy_file(session, argv[1], argv[2]));
        } else if (!strcmp(argv[0], "mv")) {
            if (argc != 3) puts("Uso: mv <origem> <destino>");
            else print_result(fm_move_entry(session, argv[1], argv[2]));
        } else if (!strcmp(argv[0], "rm")) {
            if (argc != 2) puts("Uso: rm <arquivo|link>");
            else if (confirm_action("Remover", argv[1])) print_result(fm_remove_file(session, argv[1]));
            else puts("Operacao cancelada.");
        } else if (!strcmp(argv[0], "rmdir")) {
            if (argc != 2) puts("Uso: rmdir <diretorio vazio>");
            else if (confirm_action("Remover diretorio vazio", argv[1])) print_result(fm_remove_directory(session, argv[1]));
            else puts("Operacao cancelada.");
        } else if (!strcmp(argv[0], "stat")) {
            FileManagerEntry info;
            if (argc != 2) puts("Uso: stat <caminho>");
            else {
                FileManagerResult r = fm_get_info(session, argv[1], &info);
                if (r.success) print_entry(&info);
                else print_result(r);
            }
        } else {
            fprintf(stderr, "[ERRO] comando desconhecido: %s\n", argv[0]);
        }
    }
    puts("Sessao encerrada.");
    return 0;
}
