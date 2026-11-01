# Arquitetura

## Camadas

terminal_ui -> command_parser
      |
      v
file_manager_application -> FileSystemPort
                                  |
                                  v
                         adapters/posix/filesystem_posix.c
                                  |
                                  v
                           src/file_manager.c

- ui/terminal_ui.c: entrada, confirmação e renderização.
- src/command_parser.c: lexer sem dependência da interface.
- src/file_manager_application.c: validação e orquestração.
- include/filesystem_port.h: contrato injetável consumido pela aplicação.
- adapters/posix/filesystem_posix.c: adapter POSIX.
- src/file_manager.c: operações e resolução de caminhos.
- src/main.c: composition root.

A aplicação recebe FileSystemPort por injeção. Testes podem substituir operações por um fake. No Linux, o adapter abre diretórios pais com openat2 e RESOLVE_NO_MAGICLINKS quando disponível, então executa operações finais com dirfd. O fallback usa open quando openat2 não existe.

## Ownership

- O serviço aloca FileManagerList.entries; o consumidor chama fm_list_free.
- A aplicação devolve listas e não as imprime.
- O adapter não retém ponteiros após retornar.

## Limites

O serviço POSIX continua concentrado em um módulo. A próxima etapa deve separar path resolution, directory operations e copy engine, além de ampliar testes de concorrência e falhas de I/O.
