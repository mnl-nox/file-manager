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
- src/file_manager_application.c: validação e orquestração; não chama syscalls nem imprime.
- include/filesystem_port.h: contrato de filesystem consumido pela aplicação.
- adapters/posix/filesystem_posix.c: implementação concreta do contrato POSIX.
- src/file_manager.c: serviço de filesystem e tradução de erros POSIX.
- src/main.c: composition root.

A aplicação recebe um FileSystemPort por injeção. Os testes substituem a operação de navegação por um fake, comprovando que o caso de uso não precisa conhecer a implementação POSIX. O adapter atual delega para o serviço POSIX existente.

## Ownership

- O serviço aloca FileManagerList.entries; o consumidor deve chamar fm_list_free.
- A aplicação devolve listas ao chamador e não as imprime.
- O adapter não retém ponteiros de sessão, argumentos ou resultados após retornar.

## Limites

O contrato está separado, mas o serviço POSIX ainda é um módulo concentrado. A extração futura deve dividir resolução de caminhos, operações de diretório e cópia em módulos menores.
