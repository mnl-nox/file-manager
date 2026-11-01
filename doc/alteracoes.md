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
