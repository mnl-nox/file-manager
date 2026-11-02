# ADR-0002: resolução de diretório pai com openat2

- **Estado:** Aceita com limitações documentadas
- **Contexto:** implementação Linux atual.
- **Decisão:** tentar abrir o diretório pai com `openat2(2)`, usando `RESOLVE_NO_MAGICLINKS`.
- **Data de registro:** 2026-11

## Contexto

Operações que primeiro validam um caminho e depois acessam a entrada podem ser expostas a corridas de substituição de componentes. Operações relativas a um descritor de diretório reduzem a dependência de uma nova resolução textual para a entrada final.

## Decisão

Para operações que utilizam `resolve_parent()`, abrir o diretório pai com `openat2(2)` no Linux, utilizando `O_PATH | O_DIRECTORY | O_CLOEXEC` e `RESOLVE_NO_MAGICLINKS`. Em seguida, operar sobre a entrada final com interfaces *at, como `openat`, `fstatat`, `unlinkat` e `renameat`.

Se a syscall não existir ou a ABI não for aceita, utilizar fallback com `open(2)` e `O_DIRECTORY`.

## Alternativas consideradas

- **Resolução textual seguida de syscall por caminho:** mais simples, mas reabre a janela para alterações concorrentes de caminho.
- **`openat2` com `RESOLVE_BENEATH` ou `RESOLVE_IN_ROOT`:** poderia suportar um modelo de confinamento mais forte, mas exigiria definir uma raiz e semântica de caminho que o produto ainda não possui.
- **Somente interfaces POSIX `*at`:** úteis para operações relativas a dirfd, mas não substituem por si só as políticas de resolução de caminhos fornecidas por `openat2`.

## Consequências e limites

- Magic links são bloqueados na resolução do diretório pai quando `openat2` está disponível.
- Symlinks comuns continuam permitidos.
- Caminhos absolutos continuam aceitos; não há confinamento a uma raiz.
- O fallback não oferece bloqueio de magic links.
- Outras operações, como navegação via `cd` e parte da listagem, seguem fluxos próprios e não devem ser presumidas como cobertas por este mecanismo.
- O uso de dirfd reduz algumas janelas TOCTOU, mas não torna todas as operações atomicamente seguras contra um adversário local.
