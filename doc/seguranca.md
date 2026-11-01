# Segurança e semântica de filesystem

## Modelo de ameaça

O programa opera com as credenciais do usuário. Não eleva privilégios e não é sandbox. O kernel continua aplicando permissões e ACLs. Caminhos são entrada não confiável.

## Resolução de caminhos e dirfd

No Linux, o diretório pai é aberto com openat2, O_PATH | O_DIRECTORY | O_CLOEXEC e RESOLVE_NO_MAGICLINKS. Isso faz a resolução dos componentes do caminho ocorrer em uma única operação do kernel, bloqueando magic links (por exemplo, links especiais de /proc) e produzindo um descritor estável para a operação final. Symlinks comuns continuam permitidos, preservando a semântica normal de navegação.

Se o kernel não oferecer openat2 (ENOSYS, EINVAL ou E2BIG), o código usa open com O_DIRECTORY como fallback. Nesse cenário, a proteção contra magic links não está disponível e a resolução de ancestrais mantém as limitações da API pathname.

Depois de abrir o diretório pai, as operações finais usam openat, mkdirat, fstatat, unlinkat e renameat/renameat2. A listagem usa fstatat relativo ao descritor do diretório aberto. A cópia abre a origem com O_NOFOLLOW | O_NONBLOCK, valida o descritor com fstat e cria o destino com O_EXCL.

O descritor mantém estável o diretório pai depois de aberto: substituir posteriormente seu nome no namespace não redireciona a operação final. Isso reduz TOCTOU na entrada final. Não existe raiz lógica confinada, RESOLVE_BENEATH ou RESOLVE_IN_ROOT; caminhos absolutos continuam permitidos e não há sandbox. O programa não promete isolamento contra um adversário que tenha controle sobre a semântica dos caminhos e permissões do usuário.

A suíte de regressão verifica que symlinks comuns de diretório continuam funcionais e que magic links via /proc/self/fd são rejeitados quando openat2 está disponível.

## Links simbólicos

- Listagem e stat não seguem o link final.
- touch e cp recusam seguir o link final.
- rm remove o link final, não seu destino.
- Symlinks comuns em componentes intermediários continuam permitidos.
- Magic links são bloqueados pelo openat2 quando suportado.
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
4. Não trate a aplicação como sandbox.
5. Execute make test e make test-asan antes de distribuir.
