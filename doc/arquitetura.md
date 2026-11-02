# Arquitetura

## Visão de camadas

```text
Terminal UI
    |
Command parser
    |
Application / use cases
    |
FileSystemPort (interface injetável)
    |
POSIX adapter
    |
Linux / POSIX filesystem
```

- `ui/terminal_ui.c`: entrada, confirmação e renderização.
- `src/command_parser.c`: lexer independente, com aspas e escapes.
- `src/file_manager_application.c`: validação e orquestração sem I/O de terminal.
- `include/filesystem_port.h`: contrato de filesystem injetável.
- `adapters/posix/filesystem_posix.c`: adapter POSIX concreto.
- `src/file_manager.c`: operações, resolução de caminhos e tradução de erros.
- `src/main.c`: composition root e injeção das dependências.

A aplicação consome o contrato `FileSystemPort`. Testes podem substituir o adapter real por um fake. A UI não executa syscalls diretamente.

## Fluxo de uma operação

1. A UI lê a linha e encaminha o texto ao parser.
2. O parser produz comando e argumentos, sem executar operações.
3. A aplicação valida aridade e argumentos e seleciona o caso de uso.
4. O serviço chama o port injetado.
5. O adapter POSIX executa a operação e devolve resultado tipado.
6. A UI renderiza sucesso ou erro e solicita confirmação antes de operações destrutivas.

## Ownership

- O serviço aloca `FileManagerList.entries`; o consumidor deve chamar `fm_list_free`.
- O adapter não retém ponteiros de entrada após o retorno.
- Descritores de diretório e arquivo são fechados pelo caminho que os abriu.
- A camada de aplicação não é proprietária de recursos internos do adapter.

## Testes e site

Testes de filesystem usam diretórios temporários; parser e aplicação têm suítes próprias. `make test-asan` executa AddressSanitizer e UndefinedBehaviorSanitizer.

O site estático em `site/` é separado do runtime. GitHub Actions valida os arquivos em PR e publica o diretório após push em `main`. Consulte [site-e-deploy.md](site-e-deploy.md).
