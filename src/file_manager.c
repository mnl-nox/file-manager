#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include "file_manager.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static FileManagerResult result_make(int ok, int code, const char *path, const char *fmt, ...) {
    FileManagerResult r;
    va_list ap;
    r.success = ok;
    r.error_code = code;
    r.path[0] = '\0';
    r.message[0] = '\0';
    if (path) (void)snprintf(r.path, sizeof(r.path), "%s", path);
    va_start(ap, fmt);
    (void)vsnprintf(r.message, sizeof(r.message), fmt, ap);
    va_end(ap);
    return r;
}

static FileManagerResult resolve_path(const FileManagerSession *s, const char *p, char out[FM_PATH_SIZE]) {
    int n;
    if (!s || !p || !*p) return result_make(0, EINVAL, p, "caminho vazio ou sessao invalida");
    n = p[0] == '/' ? snprintf(out, FM_PATH_SIZE, "%s", p)
                    : snprintf(out, FM_PATH_SIZE, "%s/%s", s->current_directory, p);
    if (n < 0 || n >= FM_PATH_SIZE) {
        out[0] = '\0';
        return result_make(0, ENAMETOOLONG, p, "caminho excede o limite suportado");
    }
    return result_make(1, 0, out, "ok");
}

static FileManagerEntryType type_from_mode(mode_t mode) {
    if (S_ISREG(mode)) return FM_ENTRY_FILE;
    if (S_ISDIR(mode)) return FM_ENTRY_DIRECTORY;
    if (S_ISLNK(mode)) return FM_ENTRY_SYMLINK;
    return FM_ENTRY_OTHER;
}

static void entry_from_stat(FileManagerEntry *e, const char *name, const struct stat *st) {
    (void)snprintf(e->name, sizeof(e->name), "%s", name ? name : "");
    e->type = type_from_mode(st->st_mode);
    e->size = st->st_size;
    e->mode = st->st_mode;
    e->modified_at = st->st_mtime;
}

static int compare_entries(const void *a, const void *b) {
    const FileManagerEntry *x = a, *y = b;
    return strcmp(x->name, y->name);
}

FileManagerResult fm_session_init(FileManagerSession *s) {
    if (!s) return result_make(0, EINVAL, NULL, "sessao nao pode ser nula");
    if (!getcwd(s->current_directory, sizeof(s->current_directory)))
        return result_make(0, errno, NULL, "getcwd: %s", strerror(errno));
    return result_make(1, 0, s->current_directory, "sessao inicializada");
}

FileManagerResult fm_change_directory(FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE];
    struct stat st;
    FileManagerResult r;
    if (!s) return result_make(0, EINVAL, p, "sessao nao pode ser nula");
    r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (stat(path, &st) != 0) return result_make(0, errno, path, "acesso ao diretorio: %s", strerror(errno));
    if (!S_ISDIR(st.st_mode)) return result_make(0, ENOTDIR, path, "o caminho nao e um diretorio");
    if (!realpath(path, s->current_directory))
        return result_make(0, errno, path, "resolucao do diretorio: %s", strerror(errno));
    return result_make(1, 0, s->current_directory, "diretorio alterado");
}

FileManagerResult fm_list_directory(const FileManagerSession *s, const char *p, FileManagerList *list) {
    char path[FM_PATH_SIZE], child[FM_PATH_SIZE];
    const char *target;
    DIR *dir;
    struct dirent *de;
    size_t cap = 0;
    int read_error = 0;
    if (!s || !list) return result_make(0, EINVAL, p, "sessao e lista sao obrigatorias");
    list->entries = NULL;
    list->count = 0;
    if (!p || !*p) target = s->current_directory;
    else {
        FileManagerResult r = resolve_path(s, p, path);
        if (!r.success) return r;
        target = path;
    }
    dir = opendir(target);
    if (!dir) return result_make(0, errno, target, "abertura do diretorio: %s", strerror(errno));
    errno = 0;
    while ((de = readdir(dir)) != NULL) {
        struct stat st;
        int n;
        FileManagerEntry *tmp;
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) { errno = 0; continue; }
        n = snprintf(child, sizeof(child), "%s/%s", target, de->d_name);
        if (n < 0 || (size_t)n >= sizeof(child)) { read_error = ENAMETOOLONG; break; }
        if (lstat(child, &st) != 0) { errno = 0; continue; } /* entry disappeared or became inaccessible */
        if (list->count == cap) {
            size_t next = cap ? cap * 2 : 16;
            if (next < cap || next > ((size_t)-1) / sizeof(*list->entries)) { read_error = ENOMEM; break; }
            tmp = realloc(list->entries, next * sizeof(*list->entries));
            if (!tmp) { read_error = ENOMEM; break; }
            list->entries = tmp;
            cap = next;
        }
        entry_from_stat(&list->entries[list->count++], de->d_name, &st);
        errno = 0;
    }
    if (!read_error && errno != 0) read_error = errno;
    if (closedir(dir) != 0 && !read_error) read_error = errno;
    if (read_error) {
        fm_list_free(list);
        return result_make(0, read_error, target, "listagem incompleta: %s", strerror(read_error));
    }
    if (list->count > 1) qsort(list->entries, list->count, sizeof(*list->entries), compare_entries);
    return result_make(1, 0, target, "diretorio listado");
}

FileManagerResult fm_create_file(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE];
    FileManagerResult r = resolve_path(s, p, path);
    int fd;
    if (!r.success) return r;
    fd = open(path, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0644);
    if (fd < 0) return result_make(0, errno, path, "criacao do arquivo: %s", strerror(errno));
    if (close(fd) != 0) return result_make(0, errno, path, "fechamento do arquivo: %s", strerror(errno));
    return result_make(1, 0, path, "arquivo criado");
}

FileManagerResult fm_create_directory(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE];
    FileManagerResult r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (mkdir(path, 0755) != 0) return result_make(0, errno, path, "criacao do diretorio: %s", strerror(errno));
    return result_make(1, 0, path, "diretorio criado");
}

FileManagerResult fm_copy_file(const FileManagerSession *s, const char *src, const char *dst) {
    char a[FM_PATH_SIZE], b[FM_PATH_SIZE];
    struct stat st;
    char buffer[65536];
    int in = -1, out = -1, saved = 0;
    ssize_t nr;
    FileManagerResult r = resolve_path(s, src, a);
    if (!r.success) return r;
    r = resolve_path(s, dst, b);
    if (!r.success) return r;
    if (lstat(a, &st) != 0) return result_make(0, errno, a, "origem: %s", strerror(errno));
    if (!S_ISREG(st.st_mode)) return result_make(0, EINVAL, a, "cp aceita somente arquivos regulares; links simbolicos nao sao seguidos");
    in = open(a, O_RDONLY | O_NOFOLLOW);
    if (in < 0) return result_make(0, errno, a, "abertura da origem: %s", strerror(errno));
    out = open(b, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, st.st_mode & 0777);
    if (out < 0) { saved = errno; close(in); return result_make(0, saved, b, "destino nao criado: %s", strerror(saved)); }
    while ((nr = read(in, buffer, sizeof(buffer))) != 0) {
        ssize_t off = 0;
        if (nr < 0) { if (errno == EINTR) continue; saved = errno; break; }
        while (off < nr) {
            ssize_t nw = write(out, buffer + off, (size_t)(nr - off));
            if (nw < 0 && errno == EINTR) continue;
            if (nw <= 0) { saved = nw == 0 ? EIO : errno; break; }
            off += nw;
        }
        if (saved) break;
    }
    if (close(in) != 0 && !saved) saved = errno;
    if (close(out) != 0 && !saved) saved = errno;
    if (saved) {
        (void)unlink(b);
        return result_make(0, saved, b, "copia incompleta, destino removido: %s", strerror(saved));
    }
    return result_make(1, 0, b, "arquivo copiado");
}

FileManagerResult fm_move_entry(const FileManagerSession *s, const char *src, const char *dst) {
    char a[FM_PATH_SIZE], b[FM_PATH_SIZE];
    struct stat st;
    FileManagerResult r = resolve_path(s, src, a);
    if (!r.success) return r;
    r = resolve_path(s, dst, b);
    if (!r.success) return r;
    if (lstat(a, &st) != 0) return result_make(0, errno, a, "origem: %s", strerror(errno));
    if (lstat(b, &st) == 0) return result_make(0, EEXIST, b, "destino ja existe; nenhuma sobrescrita foi feita");
    if (errno != ENOENT) return result_make(0, errno, b, "verificacao do destino: %s", strerror(errno));
    if (rename(a, b) != 0) return result_make(0, errno, a, "movimentacao: %s", strerror(errno));
    return result_make(1, 0, b, "item movido/renomeado");
}

FileManagerResult fm_remove_file(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE];
    struct stat st;
    FileManagerResult r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (lstat(path, &st) != 0) return result_make(0, errno, path, "item: %s", strerror(errno));
    if (S_ISDIR(st.st_mode)) return result_make(0, EISDIR, path, "rm nao remove diretorios; use rmdir para diretorios vazios");
    if (unlink(path) != 0) return result_make(0, errno, path, "remocao: %s", strerror(errno));
    return result_make(1, 0, path, "arquivo ou link removido");
}

FileManagerResult fm_remove_directory(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE];
    FileManagerResult r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (rmdir(path) != 0) return result_make(0, errno, path, "rmdir (somente diretorios vazios): %s", strerror(errno));
    return result_make(1, 0, path, "diretorio vazio removido");
}

FileManagerResult fm_get_info(const FileManagerSession *s, const char *p, FileManagerEntry *info) {
    char path[FM_PATH_SIZE];
    struct stat st;
    FileManagerResult r;
    if (!info) return result_make(0, EINVAL, p, "saida de metadados obrigatoria");
    r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (lstat(path, &st) != 0) return result_make(0, errno, path, "metadados: %s", strerror(errno));
    entry_from_stat(info, path, &st);
    return result_make(1, 0, path, "metadados obtidos");
}

void fm_list_free(FileManagerList *list) {
    if (!list) return;
    free(list->entries);
    list->entries = NULL;
    list->count = 0;
}

const char *fm_entry_type_label(FileManagerEntryType type) {
    switch (type) {
        case FM_ENTRY_FILE: return "arquivo";
        case FM_ENTRY_DIRECTORY: return "diretorio";
        case FM_ENTRY_SYMLINK: return "link";
        default: return "outro";
    }
}
