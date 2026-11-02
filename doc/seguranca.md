# Segurança e semântica de filesystem

## Modelo de ameaça

O programa opera com as credenciais do usuário. Não eleva privilégios e não é sandbox. O kernel continua aplicando permissões e ACLs. Caminhos são entrada não confiável.

## Resolução de caminhos e dirfd

No Linux, o diretório pai é aberto com `openat2`, `O_PATH | O_DIRECTORY | O_CLOEXEC` e `RESOLVE_NO_MAGICLINKS`. Isso bloqueia magic links (por exemplo, links especiais de `/proc`) durante a resolução do diretório pai. Symlinks comuns continuam permitidos.

Se o kernel não oferecer `openat2` (`ENOSYS`, `EINVAL` ou `E2BIG`), o código usa `open(O_DIRECTORY)` como fallback. Nesse cenário, a proteção contra magic links não está disponível.

Depois de abrir o diretório pai, as operações finais usam `openat`, `mkdirat`, `fstatat`, `unlinkat` e `renameat`/`renameat2`. A listagem usa `fstatat` relativo ao descritor aberto. Erros de metadados diferentes de `ENOENT` são propagados; uma entrada removida durante a enumeração pode ser ignorada.

A abertura do pai fixa o diretório para a operação final e reduz TOCTOU na entrada final. Não existe raiz lógica confinada, `RESOLVE_BENEATH` ou `RESOLVE_IN_ROOT`; caminhos absolutos continuam permitidos. Não há isolamento contra um adversário que controla diretórios ancestrais ou as permissões do usuário.

## Links simbólicos

- Listagem e `stat` não seguem o link final.
- `touch` e `cp` recusam seguir o link final.
- `rm` remove o link final, não seu destino.
- Symlinks comuns em componentes intermediários continuam permitidos.
- Magic links são bloqueados por `openat2` quando suportado.
- `cd` segue links de diretório.

## Operações destrutivas

| Operação | Regra |
|---|---|
| `touch` / `mkdir` | Não substituem entrada existente |
| `cp` | Arquivo regular, destino novo; remove destino parcial em erro |
| `mv` | Não substitui destino; usa `renameat2(RENAME_NOREPLACE)` no Linux quando disponível |
| `rm` | Arquivo ou link; confirmação na UI; nunca remove diretórios |
| `rmdir` | Somente diretório vazio; confirmação na UI |

Uma barra final no caminho exige que a entrada final seja um diretório. Não há remoção recursiva. `EXDEV` em `mv` é retornado ao usuário; não há fallback implícito de cópia e remoção.

## Cópia e durabilidade

A cópia trata `EINTR`, escritas parciais e erros de fechamento. Não promete durabilidade após queda de energia: ainda não chama `fsync` no arquivo e no diretório. Não preserva ACLs, xattrs, ownership ou timestamps.

## Recomendações

1. Execute como usuário sem privilégios administrativos.
2. Faça backup antes de operar em dados críticos.
3. Inspecione o caminho antes de confirmar `rm` e `rmdir`.
4. Não trate a aplicação como sandbox.
5. Execute `make test` e `make test-asan` antes de distribuir.
