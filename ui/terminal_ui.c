#define _POSIX_C_SOURCE 200809L
#include "terminal_ui.h"
#include "command_parser.h"
#include "file_manager_application.h"
#include "filesystem_port.h"

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

static void print_list(const FileManagerList *list) {
    size_t i;
    printf("%-10s %10s %4s %-16s %s\n", "TIPO", "BYTES", "MODO", "MODIFICADO", "NOME");
    separator();
    if (!list->count) puts("(diretorio vazio)");
    for (i = 0; i < list->count; ++i) print_entry(&list->entries[i]);
    printf("%zu entrada(s)\n", list->count);
}

static int confirm_action(const char *verb, const char *path) {
    char answer[16];
    printf("%s '%s'? [y/N] ", verb, path);
    if (!fgets(answer, sizeof(answer), stdin)) return 0;
    return answer[0] == 'y' || answer[0] == 'Y';
}

int fm_terminal_ui_run(FileManagerSession *session) {
    const FileSystemPort *filesystem = fm_posix_filesystem_port();
    char line[INPUT_SIZE];
    if (!session) return 1;
    separator();
    puts("FILE MANAGER | POSIX / C11");
    separator();
    help();
    {
        char *initial_argv[] = {"ls"};
        FileManagerAppResult initial = fm_application_execute(session, filesystem, 1, initial_argv, 0);
        if (initial.kind == FM_APP_LIST) { print_list(&initial.list); fm_list_free(&initial.list); }
        else if (initial.kind == FM_APP_OPERATION) print_result(initial.operation);
    }

    for (;;) {
        char *argv[MAX_ARGUMENTS] = {0};
        int argc, ch;
        FileManagerAppResult result;
        printf("\nfm:%s$ ", session->current_directory);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) { putchar('\n'); break; }
        if (!strchr(line, '\n') && !feof(stdin)) {
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            fputs("[ERRO] comando excede o limite de entrada\n", stderr);
            continue;
        }
        line[strcspn(line, "\n")] = '\0';
        argc = fm_parse_command(line, argv, MAX_ARGUMENTS);
        if (argc == 0) continue;
        if (argc < 0) {
            fputs(argc == -1 ? "[ERRO] aspas ou escape incompleto\n" : "[ERRO] argumentos demais\n", stderr);
            continue;
        }

        result = fm_application_execute(session, filesystem, argc, argv, 0);
        if (result.kind == FM_APP_CONFIRM) {
            if (confirm_action(result.message, result.value))
                result = fm_application_execute(session, filesystem, argc, argv, 1);
            else {
                puts("Operacao cancelada.");
                continue;
            }
        }

        switch (result.kind) {
            case FM_APP_EXIT:
                puts("Sessao encerrada.");
                return 0;
            case FM_APP_HELP:
                help();
                break;
            case FM_APP_PWD:
                puts(result.value);
                break;
            case FM_APP_LIST:
                print_list(&result.list);
                fm_list_free(&result.list);
                break;
            case FM_APP_INFO:
                print_entry(&result.info);
                break;
            case FM_APP_OPERATION:
                print_result(result.operation);
                break;
            case FM_APP_USAGE:
                puts(result.message);
                break;
            case FM_APP_UNKNOWN:
                fprintf(stderr, "[ERRO] %s\n", result.message);
                break;
            case FM_APP_CONFIRM:
                /* Confirmation is resolved before reaching the renderer. */
                break;
        }
    }
    puts("Sessao encerrada.");
    return 0;
}
