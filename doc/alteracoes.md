# Registro de alteracoes

## Versao inicial funcional

- Implementada a camada de servico em `src/file_manager.c`, com sessao,
  listagem, navegacao e criacao de arquivos e diretorios.
- Adicionado o contrato publico `include/file_manager.h` para separar a UI
  das operacoes de arquivos.
- Adicionada a interface textual em `ui/terminal_ui.c`, com cabecalho, ajuda,
  prompt com diretorio atual, listagem em tabela e mensagens de sucesso/erro.
- Simplificado `src/main.c` para inicializar a sessao e iniciar a UI.
- Criado o `Makefile` com os alvos `make`, `make test` e `make clean`.
- Criados testes de servico em `test/test_file_manager.c` para inicializacao,
  criacao, listagem e navegacao.

## Limitacoes conhecidas

- O parser atual separa argumentos por espacos; caminhos com espacos ainda nao
  sao suportados.
- Copia, movimentacao, remocao e consulta de metadados continuam pendentes.
