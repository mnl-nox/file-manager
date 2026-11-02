# Segurança, modelo de ameaças e semântica de filesystem

## 1. Resumo executivo

File Manager manipula entradas de filesystem com as permissões do processo que o executa. O programa não eleva privilégios e **não é uma sandbox**. A resolução de caminhos usa `openat2(2)` com `RESOLVE_NO_MAGICLINKS` no Linux quando disponível, mas não impõe confinamento a uma raiz nem bloqueia links simbólicos comuns nos componentes intermediários.

O uso recomendado é em ambiente de desenvolvimento ou estudo, com dados descartáveis ou previamente protegidos por backup. Não o utilize como mecanismo de isolamento de processos não confiáveis.

## 2. Ativos e fronteiras de confiança

| Ativo | Risco relevante |
|---|---|
| Arquivos e diretórios do usuário | Exclusão, criação ou movimentação não intencional. |
| Integridade dos caminhos | Symlinks, substituição concorrente e componentes inesperados. |
| Conteúdo copiado | Cópia incompleta ou destino parcial em caso de erro. |
| Recursos do processo | Vazamento de descritores ou memória em caminhos de falha. |
| Estado da sessão | Divergência entre caminho textual e estado atual do filesystem. |

As entradas de comando e os nomes de caminhos devem ser tratados como dados não confiáveis. As permissões do kernel continuam sendo a autoridade final de acesso.

## 3. Modelo de ameaças

### Considerado
- caminhos inexistentes, inválidos ou excessivamente longos;
- entradas finais que são symlinks ou tipos de arquivo inesperados;
- erros de permissão e erros de I/O;
- entradas removidas durante uma listagem;
- falhas durante a cópia e escrita parcial;
- componentes de caminho que incluem magic links, quando `openat2` está disponível.

### Não coberto ou não garantido
- confinamento a um diretório-raiz;
- proteção completa contra um adversário local que controla diretórios ancestrais;
- prevenção de todas as condições TOCTOU em toda a aplicação;
- isolamento contra processos com as mesmas credenciais;
- durabilidade transacional após queda de energia;
- preservação integral de metadados estendidos;
- proteção contra bugs do kernel, filesystem, libc ou hardware.

## 4. Resolução de caminhos

A sessão mantém o diretório atual como caminho textual. Caminhos relativos são convertidos em caminhos absolutos por concatenação com esse diretório.

Para operações que utilizam `resolve_parent()`, o core:
1. resolve o caminho textual;
2. separa o componente final do diretório pai;
3. abre o diretório pai;
4. executa a operação final relativamente ao descritor de diretório.

No Linux, a abertura do pai tenta `openat2(2)` com:
- `O_PATH | O_DIRECTORY | O_CLOEXEC`;
- `RESOLVE_NO_MAGICLINKS`.

Isso impede a resolução de magic links durante a abertura do diretório pai. **Não** impede symlinks comuns, não aplica `RESOLVE_BENEATH` ou `RESOLVE_IN_ROOT`, não confina caminhos absolutos e não cria uma sandbox.

Quando `openat2` não existe ou a ABI não é aceita (`ENOSYS`, `EINVAL` ou `E2BIG`), o código usa `open(2)` com `O_DIRECTORY`. Esse fallback não oferece o bloqueio de magic links.

A listagem usa `opendir(3)`, `readdir(3)` e `fstatat(2)` relativo ao descritor do diretório aberto. Se uma entrada desaparecer durante a enumeração, ela é ignorada; outros erros de metadados interrompem a listagem e são retornados.

## 5. Semântica de links simbólicos

| Operação | Comportamento observado |
|---|---|
| `ls` | Classifica a entrada final sem seguir o symlink, por `AT_SYMLINK_NOFOLLOW`. |
| `stat` | Para entradas comuns, consulta metadados sem seguir o symlink final. Os casos especiais `.` e `..` usam `stat(2)`. |
| `touch` | Cria com `O_EXCL | O_NOFOLLOW`; não substitui a entrada final. |
| `cp` | Abre origem com `O_NOFOLLOW`, valida por `fstat(2)` que é arquivo regular e cria destino novo. |
| `mv` | Move a entrada final como objeto de diretório, sem seguir o symlink final na verificação. |
| `rm` | Remove o link final com `unlinkat(2)`, não o objeto apontado. |
| `rmdir` | Remove somente diretório vazio. |
| `cd` | Utiliza `stat(2)` e `realpath(3)`; segue symlinks de diretório. |

Symlinks comuns em componentes intermediários do caminho continuam permitidos.

## 6. Operações e efeitos colaterais

| Operação | Garantia implementada | Limite relevante |
|---|---|---|
| `touch` | Criação exclusiva com `O_CREAT | O_EXCL`. | Modo final é influenciado pelo umask do processo. |
| `mkdir` | Criação relativa ao dirfd com `mkdirat(2)`. | Modo final é influenciado pelo umask. |
| `cp` | Aceita arquivo regular, destino exclusivo, trata `EINTR` e escritas parciais. | Não preserva ACLs, xattrs, ownership ou timestamps; não garante durabilidade. |
| `mv` | Tenta `renameat2(RENAME_NOREPLACE)` no Linux. | Fallback verifica destino e depois chama `renameat`; existe janela de corrida. `EXDEV` é retornado. |
| `rm` | Usa `unlinkat(2)`; recusa diretórios. | A confirmação é uma proteção de interface, não uma fronteira de segurança. |
| `rmdir` | Usa `unlinkat(AT_REMOVEDIR)`. | Só remove diretórios vazios; confirmação na UI. |

### 6.1 Cópia

A origem é aberta sem seguir o symlink final e com `O_NONBLOCK`, depois validada como arquivo regular por `fstat(2)`. O destino é criado com `O_EXCL`, evitando substituir uma entrada preexistente. Em erro de leitura, escrita ou fechamento, o destino parcial é removido quando possível.

A operação não é transacional. Uma falha no processo, queda de energia ou falha ao remover o destino parcial pode deixar estado que exige inspeção manual. Não há chamadas explícitas a `fsync(2)`.

### 6.2 Movimentação

Quando disponível, `renameat2(RENAME_NOREPLACE)` evita substituir o destino em uma única operação do kernel. Se essa interface não estiver disponível, o código consulta o destino e depois usa `renameat(2)`; a sequência não é atômica em relação a outros processos que possam criar o destino entre as duas chamadas.

Não há fallback de cópia e remoção para movimentação entre filesystems. O erro `EXDEV` é propagado.

## 7. Controles recomendados ao operador

1. Execute com uma conta sem privilégios administrativos.
2. Trabalhe em diretórios de teste e mantenha backup dos dados importantes.
3. Confira origem e destino antes de confirmar operações destrutivas.
4. Não execute sobre diretórios controlados por usuários ou processos não confiáveis esperando isolamento.
5. Não interprete `openat2` como garantia de confinamento.
6. Atualize o sistema operacional e mantenha kernel e libc suportados.
7. Execute `make test` e `make test-asan` antes de distribuir alterações.

## 8. Relato responsável de vulnerabilidades

Não publique detalhes exploráveis em issues abertas. Consulte [SECURITY.md](../SECURITY.md) para o procedimento de comunicação responsável.
