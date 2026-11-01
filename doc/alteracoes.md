# Registro de alterações

## Entrega: operações essenciais e CLI robusta

- Ampliado o contrato de entrada com tamanho, permissões, timestamp e tipo de link simbólico.
- Melhorada a listagem para usar `lstat`, tratar erros de leitura/fechamento e ordenar entradas.
- Adicionadas cópia de arquivos regulares, movimentação/renomeação, remoção de arquivos/links, remoção de diretórios vazios e metadados.
- Cópia cria destino exclusivamente, trata escrita parcial/EINTR e tenta limpar destino incompleto.
- Parser da UI agora aceita aspas simples/duplas, escapes e caminhos com espaços.
- Remoção interativa exige confirmação.
- Testes substituem shell cleanup por fixtures POSIX em diretório temporário.
- Makefile inclui build rigoroso, testes e alvo ASan/UBSan.
- Adicionada documentação de segurança e semântica.

## Limitações remanescentes

- Não há isolamento contra TOCTOU em diretórios ancestrais concorrentes.
- `mv` não garante no-replace atômico em todos os POSIX.
- `cp` não copia diretórios nem metadados estendidos.
- CI e refactor em camadas adicionais são etapas subsequentes.

- Extraído o lexer para `src/command_parser.c`, com contrato próprio e testes unitários independentes.
- Em Linux, `mv` utiliza `renameat2(RENAME_NOREPLACE)` quando disponível.
- CI passa a executar AddressSanitizer e UndefinedBehaviorSanitizer.

- Extraída a camada de aplicação para `src/file_manager_application.c`, com despacho de comandos sem I/O de terminal.
- UI passou a renderizar resultados tipados e delegar casos de uso à aplicação.
- Adicionados testes unitários da camada de aplicação, inclusive fluxo de confirmação destrutiva.
