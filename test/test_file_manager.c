#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "file_manager.h"

#include <errno.h>
#include <fcntl.h>
#if defined(__linux__)
#include <linux/openat2.h>
#include <sys/syscall.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int (*test_fn)(void);
static char temp_dir[] = "/tmp/file-manager-tests-XXXXXX";

static int check(int condition, const char *message) {
    if (!condition) fprintf(stderr, "FALHOU: %s\n", message);
    return condition;
}

static int make_path(char out[FM_PATH_SIZE], const char *name) {
    int n = snprintf(out, FM_PATH_SIZE, "%s/%s", temp_dir, name);
    return n >= 0 && n < FM_PATH_SIZE;
}

static int exists(const char *path) {
    struct stat st;
    return lstat(path, &st) == 0;
}

static int write_fixture(const char *path, const char *data) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    size_t len = strlen(data), off = 0;
    if (fd < 0) return 0;
    while (off < len) {
        ssize_t n = write(fd, data + off, len - off);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { close(fd); return 0; }
        off += (size_t)n;
    }
    return close(fd) == 0;
}

static int file_equals(const char *path, const char *expected) {
    char buf[128];
    int fd = open(path, O_RDONLY);
    ssize_t n;
    if (fd < 0) return 0;
    n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (n < 0) return 0;
    buf[n] = '\0';
    return strcmp(buf, expected) == 0;
}

static void session_at_temp(FileManagerSession *s) {
    (void)snprintf(s->current_directory, sizeof(s->current_directory), "%s", temp_dir);
}

static int test_session(void) {
    FileManagerSession s;
    FileManagerEntry root_info;
    FileManagerResult r = fm_session_init(&s);
    char cwd[FM_PATH_SIZE];
    if (!check(r.success, "sessao deve inicializar")) return 0;
    if (!getcwd(cwd, sizeof(cwd))) return 0;
    if (!check(strcmp(cwd, s.current_directory) == 0, "sessao deve refletir cwd")) return 0;
    if (!check(fm_get_info(&s, "/", &root_info).success && root_info.type == FM_ENTRY_DIRECTORY, "stat da raiz deve funcionar")) return 0;
    if (!check(fm_get_info(&s, ".", &root_info).success && root_info.type == FM_ENTRY_DIRECTORY, "stat . deve funcionar")) return 0;
    if (!check(fm_get_info(&s, "..", &root_info).success && root_info.type == FM_ENTRY_DIRECTORY, "stat .. deve funcionar")) return 0;
    return 1;
}

static int test_create_and_list(void) {
    FileManagerSession s;
    FileManagerList list;
    FileManagerResult r;
    char file[FM_PATH_SIZE], dir[FM_PATH_SIZE], nested[FM_PATH_SIZE];
    size_t i;
    int saw_file = 0, saw_dir = 0;
    session_at_temp(&s);
    if (!make_path(file, "alpha.txt") || !make_path(dir, "docs") || !make_path(nested, "docs/nested.txt")) return 0;
    if (!check(fm_create_file(&s, "alpha.txt").success, "touch cria arquivo")) return 0;
    if (!check(!fm_create_file(&s, "alpha.txt").success, "touch nao sobrescreve")) return 0;
    if (!check(fm_create_directory(&s, "docs").success, "mkdir cria diretorio")) return 0;
    if (!check(fm_create_file(&s, "docs/nested.txt").success, "openat cria entrada sob diretorio pai aberto")) return 0;
    if (!check(fm_remove_file(&s, "docs/nested.txt").success, "unlinkat remove entrada sob diretorio pai aberto")) return 0;
    r = fm_list_directory(&s, NULL, &list);
    if (!check(r.success, "ls deve funcionar")) return 0;
    for (i = 0; i < list.count; ++i) {
        if (!strcmp(list.entries[i].name, "alpha.txt") && list.entries[i].type == FM_ENTRY_FILE) saw_file = 1;
        if (!strcmp(list.entries[i].name, "docs") && list.entries[i].type == FM_ENTRY_DIRECTORY) saw_dir = 1;
    }
    fm_list_free(&list);
    return check(saw_file && saw_dir, "listagem deve identificar arquivo e diretorio");
}

static int test_copy_and_metadata(void) {
    FileManagerSession s;
    FileManagerEntry info;
    char src[FM_PATH_SIZE], dst[FM_PATH_SIZE];
    session_at_temp(&s);
    if (!make_path(src, "alpha.txt") || !make_path(dst, "copy.txt")) return 0;
    if (!write_fixture(src, "conteudo de teste")) return 0;
    if (!check(fm_copy_file(&s, "alpha.txt", "copy.txt").success, "cp deve copiar")) return 0;
    if (!check(file_equals(dst, "conteudo de teste"), "cp deve preservar conteudo")) return 0;
    if (!check(!fm_copy_file(&s, "alpha.txt", "copy.txt").success, "cp nao sobrescreve destino")) return 0;
    if (!check(fm_get_info(&s, "copy.txt", &info).success, "stat deve obter metadados")) return 0;
    return check(info.type == FM_ENTRY_FILE && info.size == (off_t)strlen("conteudo de teste"), "stat deve informar tipo e tamanho");
}

static int test_move_and_remove(void) {
    FileManagerSession s;
    char from[FM_PATH_SIZE], to[FM_PATH_SIZE];
    session_at_temp(&s);
    if (!make_path(from, "move-me") || !make_path(to, "moved")) return 0;
    if (!write_fixture(from, "x")) return 0;
    if (!check(fm_move_entry(&s, "move-me", "moved").success, "mv deve mover")) return 0;
    if (!check(!exists(from) && exists(to), "mv deve atualizar origem e destino")) return 0;
    if (!write_fixture(from, "nao sobrescrever")) return 0;
    if (!check(!fm_move_entry(&s, "move-me", "moved").success, "mv rejeita destino existente")) return 0;
    if (!check(exists(from) && file_equals(to, "x"), "mv nao altera origem nem destino ocupado")) return 0;
    if (!check(fm_remove_file(&s, "move-me").success, "rm remove origem de teste")) return 0;
    if (!check(fm_remove_file(&s, "moved").success, "rm remove arquivo")) return 0;
    if (!check(!exists(to), "rm remove entrada")) return 0;
    if (!check(!fm_remove_file(&s, "docs").success, "rm nao remove diretorio")) return 0;
    if (!write_fixture(from, "preservar")) return 0;
    if (!check(!fm_remove_file(&s, "move-me/").success && exists(from),
               "rm com barra final deve rejeitar arquivo e preservar a entrada")) return 0;
    if (!check(fm_create_directory(&s, "slash-dir").success, "cria diretorio para teste de barra final")) return 0;
    if (!check(fm_remove_directory(&s, "slash-dir/").success,
               "rmdir deve aceitar diretorio com barra final")) return 0;
    return 1;
}

static int test_symlink_and_empty_directory_policy(void) {
    FileManagerSession s;
    FileManagerList list;
    FileManagerEntry info;
    char link_path[FM_PATH_SIZE], empty_path[FM_PATH_SIZE], docs_path[FM_PATH_SIZE];
    char dir_link[FM_PATH_SIZE], through_link[FM_PATH_SIZE];
    size_t i;
    int saw_link = 0;
    session_at_temp(&s);
    if (!make_path(link_path, "alpha-link") || !make_path(empty_path, "empty") || !make_path(docs_path, "docs") || !make_path(dir_link, "docs-alias") || !make_path(through_link, "docs/through-link.txt")) return 0;
    if (symlink("alpha.txt", link_path) != 0) return 0;
    if (!check(fm_list_directory(&s, NULL, &list).success, "listagem com link deve funcionar")) return 0;
    for (i = 0; i < list.count; ++i)
        if (!strcmp(list.entries[i].name, "alpha-link") && list.entries[i].type == FM_ENTRY_SYMLINK) saw_link = 1;
    fm_list_free(&list);
    if (!check(saw_link, "link simbolico deve ser identificado")) return 0;
    if (!check(fm_get_info(&s, "alpha-link", &info).success && info.type == FM_ENTRY_SYMLINK, "stat nao deve seguir link")) return 0;
    if (!check(fm_remove_file(&s, "alpha-link").success, "rm remove o link em si")) return 0;
    if (symlink("docs", dir_link) != 0) return 0;
    if (!check(fm_create_file(&s, "docs-alias/through-link.txt").success && exists(through_link), "openat2 deve permitir symlink de diretorio comum")) return 0;
    if (!check(fm_remove_file(&s, "docs-alias/through-link.txt").success, "remove entrada atraves de symlink de diretorio")) return 0;
    (void)unlink(dir_link);
    if (!check(fm_create_directory(&s, "empty").success, "cria diretorio vazio")) return 0;
    if (!check(fm_remove_directory(&s, "docs").success, "rmdir remove diretorio vazio")) return 0;
    if (!check(fm_remove_directory(&s, "empty").success, "rmdir remove diretorio vazio")) return 0;
    (void)docs_path;
    return check(!exists(empty_path), "diretorio vazio deve desaparecer");
}


static int test_magic_link_rejected(void) {
#if defined(__linux__) && defined(SYS_openat2)
    FileManagerSession s;
    struct open_how how;
    char link_path[FM_PATH_SIZE], proc_target[FM_PATH_SIZE];
    int probe, dirfd;
    FileManagerResult result;
    memset(&how, 0, sizeof(how));
    how.flags = O_PATH | O_DIRECTORY | O_CLOEXEC;
    how.resolve = RESOLVE_NO_MAGICLINKS;
    probe = (int)syscall(SYS_openat2, AT_FDCWD, temp_dir, &how, sizeof(how));
    if (probe < 0 && (errno == ENOSYS || errno == EINVAL || errno == E2BIG))
        return check(1, "openat2 indisponivel; teste de magic link ignorado");
    if (probe < 0) return check(0, "probe openat2 deve funcionar");
    close(probe);
    dirfd = open(temp_dir, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (dirfd < 0) return 0;
    if (!make_path(link_path, "magic-parent")) { close(dirfd); return 0; }
    (void)snprintf(proc_target, sizeof(proc_target), "/proc/self/fd/%d", dirfd);
    if (symlink(proc_target, link_path) != 0) { close(dirfd); return 0; }
    session_at_temp(&s);
    result = fm_create_file(&s, "magic-parent/should-not-exist");
    (void)unlink(link_path);
    close(dirfd);
    return check(!result.success && result.error_code == ELOOP,
                 "openat2 deve rejeitar magic link em componente intermediario");
#else
    return 1;
#endif
}

static void cleanup(void) {
    const char *files[] = {"alpha.txt", "copy.txt", "moved", "move-me", "alpha-link"};
    char path[FM_PATH_SIZE];
    size_t i;
    for (i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
        if (make_path(path, files[i])) (void)unlink(path);
    if (make_path(path, "empty")) (void)rmdir(path);
    if (make_path(path, "docs/nested.txt")) (void)unlink(path);
    if (make_path(path, "docs/through-link.txt")) (void)unlink(path);
    if (make_path(path, "docs-alias")) (void)unlink(path);
    if (make_path(path, "magic-parent")) (void)unlink(path);
    if (make_path(path, "docs")) (void)rmdir(path);
    (void)rmdir(temp_dir);
}

int main(void) {
    test_fn tests[] = {test_session, test_create_and_list, test_copy_and_metadata,
                       test_move_and_remove, test_symlink_and_empty_directory_policy,
                       test_magic_link_rejected};
    const char *names[] = {"session", "create_and_list", "copy_and_metadata",
                           "move_and_remove", "symlink_and_empty_directory_policy",
                           "magic_link_rejected"};
    size_t i;
    if (!mkdtemp(temp_dir)) { perror("mkdtemp"); return 1; }
    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        if (!tests[i]()) {
            fprintf(stderr, "Teste interrompido: %s\n", names[i]);
            cleanup();
            return 1;
        }
        printf("[OK] %s\n", names[i]);
    }
    cleanup();
    puts("Todos os testes passaram.");
    return 0;
}
