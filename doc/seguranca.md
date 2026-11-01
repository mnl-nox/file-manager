# Segurança e semântica de filesystem

## Modelo de ameaça

O programa opera com as credenciais do usuário. Não eleva privilégios e não é sandbox. O kernel continua aplicando permissões e ACLs. Caminhos são entrada não confiável.

## Dirfd e operações relativas

As operações de folha (touch, mkdir, cp, mv, rm, rmdir, stat) agora abrem o diretório pai e executam a syscall final relativa ao descritor:
- openat para criação e abertura;
- mkdirat para diretórios;
- fstatat com AT_SYMLINK_NOFOLLOW para metadados;
- unlinkat para remoção;
- renameat2 com RENAME_NOREPLACE no Linux, com fallback renameat sujeito a corrida na checagem de destino.

A listagem usa fstatat(dirfd, name, ..., AT_SYMLINK_NOFOLLOW). A cópia abre a origem com O_NOFOLLOW | O_NONBLOCK, valida o descritor com fstat e cria o destino com O_EXCL.

O descritor mantém estável o diretório pai depois que ele foi aberto: substituir posteriormente seu nome no namespace não redireciona a operação final. Entretanto, a abertura do caminho do diretório pai ainda resolve componentes ancestrais por pathname e pode seguir symlinks. Não existe resolução integral openat2 com RESOLVE_BENEATH/RESOLVE_IN_ROOT, nem raiz de sessão presa a um dirfd. Portanto, isso reduz a janela TOCTOU na entrada final, mas não constitui confinamento nem proteção completa contra adversário local que controle os ancestrais durante a resolução.

## Links simbólicos

- Listagem e stat não seguem o link final.
- touch e cp recusam seguir o link final.
- rm remove o link final, não seu destino.
- Componentes intermediários podem seguir symlinks conforme semântica POSIX.
- cd segue links de diretório.

## Operações destrutivas

| Operação | Regra |
|---|---|
| touch / mkdir | Não substituem entrada existente |
| cp | Arquivo regular, destino novo; remove destino parcial em erro |
| mv | Não substitui destino; atômico no Linux com renameat2 |
| rm | Arquivo ou link; confirmação na UI |
| rmdir | Somente diretório vazio; confirmação na UI |

Não há remoção recursiva. EXDEV em mv é retornado ao usuário; não há fallback implícito de cópia e remoção.

## Cópia e durabilidade

A cópia trata EINTR, escritas parciais e erros de fechamento. Não promete durabilidade após queda de energia: ainda não chama fsync no arquivo e no diretório. Não preserva ACLs, xattrs, ownership ou timestamps.

## Recomendações

1. Execute como usuário sem privilégios administrativos.
2. Faça backup antes de operações em massa.
3. Inspecione o caminho antes de confirmar rm e rmdir.
4. Não trate a aplicação como sandbox ou ferramenta resistente a adversário local que controla diretórios ancestrais.
5. Execute make test e make test-asan antes de distribuir.
