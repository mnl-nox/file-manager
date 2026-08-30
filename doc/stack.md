# Stack utilizada

## Linguagem

- **C11 ou superior**: linguagem principal do gerenciador.

## Bibliotecas e APIs

- **libc**: entrada e saida, memoria, strings e tratamento de erros.
- **APIs POSIX**: diretorios, metadados, caminhos, arquivos e permissoes.
- **stdio.h**, **stdlib.h**, **string.h**, **dirent.h**, **sys/stat.h** e
  **errno.h**, conforme a necessidade de cada modulo.

As dependencias devem permanecer pequenas e preferencialmente disponiveis no
ambiente do sistema.

## Compilacao

Com GCC ou Clang:

```bash
make
```

O `Makefile` compila `src/file_manager.c`, `src/main.c` e
`ui/terminal_ui.c` com `-std=c11 -Wall -Wextra -pedantic`. Os testes podem ser
executados com `make test`.

## Ferramentas de desenvolvimento

- Git para controle de versao.
- Make ou tarefa equivalente para automatizar a compilacao.
- GDB para depuracao.
- Valgrind ou Sanitizers para detectar problemas de memoria.
- Testes automatizados para validar operacoes de arquivo em diretorios
  temporarios.

## Ambiente alvo

O alvo inicial e um sistema Unix-like, especialmente Linux, por depender de
APIs POSIX para manipulacao de diretorios e metadados.
