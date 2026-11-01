#define _GNU_SOURCE
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
#if defined(__linux__)
#include <linux/openat2.h>
#include <sys/syscall.h>
#ifndef RENAME_NOREPLACE
#define RENAME_NOREPLACE (1U << 0)
#endif
#endif
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


/*
 * Resolve a leaf relative to an opened parent directory. The parent is pinned
 * by dirfd before the final operation, so replacing the parent pathname after
 * this point cannot redirect the leaf operation.
 */
static FileManagerResult resolve_parent(
    const FileManagerSession *s,
    const char *p,
    int *parent_fd,
    char name[FM_NAME_SIZE],
    char resolved[FM_PATH_SIZE]
) {
    char *slash;
    size_t name_len, parent_len;
    int fd;
    FileManagerResult r;
    if (!parent_fd || !name || !resolved)
        return result_make(0, EINVAL, p, "saida de resolucao invalida");
    *parent_fd = -1;
    r = resolve_path(s, p, resolved);
    if (!r.success) return r;
    {
        size_t len = strlen(resolved);
        while (len > 1 && resolved[len - 1] == '/') resolved[--len] = '\0';
    }
    slash = strrchr(resolved, '/');
    if (!slash || slash[1] == '\0')
        return result_make(0, EINVAL, resolved, "o caminho precisa identificar uma entrada");
    name_len = strlen(slash + 1);
    if (name_len >= FM_NAME_SIZE)
        return result_make(0, ENAMETOOLONG, resolved, "nome excede o limite suportado");
    if (!strcmp(slash + 1, ".") || !strcmp(slash + 1, ".."))
        return result_make(0, EINVAL, resolved, "operacao sobre . ou .. nao permitida");
    (void)snprintf(name, FM_NAME_SIZE, "%s", slash + 1);
    parent_len = (size_t)(slash - resolved);
    if (parent_len == 0) parent_len = 1;
    resolved[parent_len] = '\0';
    fd = open_parent_directory(resolved);
    if (fd < 0) return result_make(0, errno, resolved, "abertura do diretorio pai: %s", strerror(errno));
    *parent_fd = fd;
    return result_make(1, 0, resolved, "diretorio pai aberto");
}

static int open_parent_directory(const char *path) {
#if defined(__linux__) && defined(SYS_openat2)
    struct open_how how;
    int fd;
    memset(&how, 0, sizeof(how));
    how.flags = O_PATH | O_DIRECTORY | O_CLOEXEC;
    how.resolve = RESOLVE_NO_MAGICLINKS;
    fd = (int)syscall(SYS_openat2, AT_FDCWD, path, &how, sizeof(how));
    if (fd >= 0) return fd;
    /* Fall back only when the kernel does not implement this openat2 ABI. */
    if (errno != ENOSYS && errno != EINVAL && errno != E2BIG) return -1;
#endif
    return open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
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
    char path[FM_PATH_SIZE];
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
        FileManagerEntry *tmp;
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) { errno = 0; continue; }
        if (fstatat(dirfd(dir), de->d_name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
            errno = 0;
            continue;
        }
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
    char path[FM_PATH_SIZE], name[FM_NAME_SIZE];
    int parent = -1, fd;
    FileManagerResult r = resolve_parent(s, p, &parent, name, path);
    if (!r.success) return r;
    fd = openat(parent, name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0644);
    if (fd < 0) { int e = errno; close(parent); return result_make(0, e, p, "criacao do arquivo: %s", strerror(e)); }
    if (close(fd) != 0) { int e = errno; close(parent); return result_make(0, e, p, "fechamento do arquivo: %s", strerror(e)); }
    if (close(parent) != 0) return result_make(0, errno, p, "fechamento do diretorio pai: %s", strerror(errno));
    return result_make(1, 0, p, "arquivo criado");
}

FileManagerResult fm_create_directory(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE], name[FM_NAME_SIZE];
    int parent = -1;
    FileManagerResult r = resolve_parent(s, p, &parent, name, path);
    if (!r.success) return r;
    if (mkdirat(parent, name, 0755) != 0) { int e = errno; close(parent); return result_make(0, e, p, "criacao do diretorio: %s", strerror(e)); }
    if (close(parent) != 0) return result_make(0, errno, p, "fechamento do diretorio pai: %s", strerror(errno));
    return result_make(1, 0, p, "diretorio criado");
}

FileManagerResult fm_copy_file(const FileManagerSession *s, const char *src, const char *dst) {
    char src_path[FM_PATH_SIZE], dst_path[FM_PATH_SIZE];
    char src_name[FM_NAME_SIZE], dst_name[FM_NAME_SIZE], buffer[65536];
    struct stat st;
    int src_parent = -1, dst_parent = -1, in = -1, out = -1, saved = 0;
    ssize_t nr;
    FileManagerResult r = resolve_parent(s, src, &src_parent, src_name, src_path);
    if (!r.success) return r;
    r = resolve_parent(s, dst, &dst_parent, dst_name, dst_path);
    if (!r.success) { close(src_parent); return r; }
    in = openat(src_parent, src_name, O_RDONLY | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK);
    if (in < 0) { saved = errno; close(src_parent); close(dst_parent); return result_make(0, saved, src, "abertura da origem: %s", strerror(saved)); }
    if (fstat(in, &st) != 0) {
        saved = errno; close(in); close(src_parent); close(dst_parent);
        return result_make(0, saved, src, "metadados da origem: %s", strerror(saved));
    }
    if (!S_ISREG(st.st_mode)) {
        close(in); close(src_parent); close(dst_parent);
        return result_make(0, EINVAL, src, "cp aceita somente arquivos regulares; links simbolicos nao sao seguidos");
    }
    out = openat(dst_parent, dst_name, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, st.st_mode & 0777);
    if (out < 0) {
        saved = errno; close(in); close(src_parent); close(dst_parent);
        return result_make(0, saved, dst, "destino nao criado: %s", strerror(saved));
    }
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
    close(src_parent);
    if (saved) {
        (void)unlinkat(dst_parent, dst_name, 0);
        close(dst_parent);
        return result_make(0, saved, dst, "copia incompleta, destino parcial removido: %s", strerror(saved));
    }
    if (close(dst_parent) != 0) return result_make(0, errno, dst, "fechamento do diretorio destino: %s", strerror(errno));
    return result_make(1, 0, dst, "arquivo copiado");
}

FileManagerResult fm_move_entry(const FileManagerSession *s, const char *src, const char *dst) {
    char src_path[FM_PATH_SIZE], dst_path[FM_PATH_SIZE];
    char src_name[FM_NAME_SIZE], dst_name[FM_NAME_SIZE];
    struct stat st;
    int src_parent = -1, dst_parent = -1, saved;
    FileManagerResult r = resolve_parent(s, src, &src_parent, src_name, src_path);
    if (!r.success) return r;
    r = resolve_parent(s, dst, &dst_parent, dst_name, dst_path);
    if (!r.success) { close(src_parent); return r; }
    if (fstatat(src_parent, src_name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
        saved = errno; close(src_parent); close(dst_parent);
        return result_make(0, saved, src, "origem: %s", strerror(saved));
    }
#if defined(__linux__) && defined(SYS_renameat2)
    if (syscall(SYS_renameat2, src_parent, src_name, dst_parent, dst_name, RENAME_NOREPLACE) == 0) {
        close(src_parent); close(dst_parent);
        return result_make(1, 0, dst, "item movido/renomeado");
    }
    if (errno != ENOSYS && errno != EINVAL && errno != EOPNOTSUPP) {
        saved = errno; close(src_parent); close(dst_parent);
        return result_make(0, saved, dst, "movimentacao sem sobrescrita: %s", strerror(saved));
    }
#endif
    if (fstatat(dst_parent, dst_name, &st, AT_SYMLINK_NOFOLLOW) == 0) {
        close(src_parent); close(dst_parent);
        return result_make(0, EEXIST, dst, "destino ja existe; nenhuma sobrescrita foi feita");
    }
    if (errno != ENOENT) {
        saved = errno; close(src_parent); close(dst_parent);
        return result_make(0, saved, dst, "verificacao do destino: %s", strerror(saved));
    }
    if (renameat(src_parent, src_name, dst_parent, dst_name) != 0) {
        saved = errno; close(src_parent); close(dst_parent);
        return result_make(0, saved, src, "movimentacao: %s", strerror(saved));
    }
    close(src_parent); close(dst_parent);
    return result_make(1, 0, dst, "item movido/renomeado");
}

FileManagerResult fm_remove_file(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE], name[FM_NAME_SIZE];
    struct stat st;
    int parent = -1, saved;
    FileManagerResult r = resolve_parent(s, p, &parent, name, path);
    if (!r.success) return r;
    if (fstatat(parent, name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
        saved = errno; close(parent); return result_make(0, saved, p, "item: %s", strerror(saved));
    }
    if (S_ISDIR(st.st_mode)) { close(parent); return result_make(0, EISDIR, p, "rm nao remove diretorios; use rmdir para diretorios vazios"); }
    if (unlinkat(parent, name, 0) != 0) {
        saved = errno; close(parent); return result_make(0, saved, p, "remocao: %s", strerror(saved));
    }
    close(parent);
    return result_make(1, 0, p, "arquivo ou link removido");
}

FileManagerResult fm_remove_directory(const FileManagerSession *s, const char *p) {
    char path[FM_PATH_SIZE], name[FM_NAME_SIZE];
    int parent = -1, saved;
    FileManagerResult r = resolve_parent(s, p, &parent, name, path);
    if (!r.success) return r;
    if (unlinkat(parent, name, AT_REMOVEDIR) != 0) {
        saved = errno; close(parent); return result_make(0, saved, p, "rmdir (somente diretorios vazios): %s", strerror(saved));
    }
    close(parent);
    return result_make(1, 0, p, "diretorio vazio removido");
}

FileManagerResult fm_get_info(const FileManagerSession *s, const char *p, FileManagerEntry *info) {
    char path[FM_PATH_SIZE], name[FM_NAME_SIZE];
    struct stat st;
    int parent = -1, saved;
    FileManagerResult r;
    if (!info) return result_make(0, EINVAL, p, "saida de metadados obrigatoria");
    r = resolve_path(s, p, path);
    if (!r.success) return r;
    if (!strcmp(path, "/")) {
        parent = open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        if (parent < 0) return result_make(0, errno, p, "abertura da raiz: %s", strerror(errno));
        if (fstat(parent, &st) != 0) { saved = errno; close(parent); return result_make(0, saved, p, "metadados: %s", strerror(saved)); }
        entry_from_stat(info, "/", &st);
        close(parent);
        return result_make(1, 0, p, "metadados obtidos");
    }
    r = resolve_parent(s, p, &parent, name, path);
    if (!r.success) return r;
    if (fstatat(parent, name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
        saved = errno; close(parent); return result_make(0, saved, p, "metadados: %s", strerror(saved));
    }
    entry_from_stat(info, name, &st);
    close(parent);
    return result_make(1, 0, p, "metadados obtidos");
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
