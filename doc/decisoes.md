# Decisoes de projeto

Este registro documenta decisoes tecnicas e seus motivos. Ele deve ser
atualizado quando uma decisao for revisada.

## DEC-001: Linguagem C11+

- **Status:** Aceita
- **Decisao:** Implementar o projeto em C11 ou superior.
- **Motivo:** Mantem o projeto proximo das APIs de sistema e do objetivo de
  estudo de C, com recursos modernos do padrao.
- **Consequencia:** A compilacao deve declarar explicitamente o padrao C.

## DEC-002: Interface inicial no terminal

- **Status:** Aceita
- **Decisao:** A primeira versao sera interativa e executada no terminal.
- **Motivo:** Reduz o escopo inicial e atende diretamente ao objetivo de
  manipulacao de arquivos.
- **Consequencia:** `ui/` deve concentrar entrada e saida textual; uma UI
  grafica pode ser adicionada posteriormente.

## DEC-003: Uso de APIs POSIX e libc

- **Status:** Aceita
- **Decisao:** Usar `dirent`, `stat`, `mkdir`, `rename`, `remove` e APIs
  relacionadas, conforme a necessidade.
- **Motivo:** Sao as interfaces naturais para arquivos e diretorios em Linux.
- **Consequencia:** A portabilidade inicial fica direcionada a sistemas
  Unix-like.

## DEC-004: Separacao entre interface e servico de arquivos

- **Status:** Aceita
- **Decisao:** A interface nao deve chamar diretamente cada operacao de baixo
  nivel; deve usar uma camada de servico.
- **Motivo:** Facilita testes, tratamento uniforme de erros e futuras UIs.
- **Consequencia:** Contratos publicos devem ficar em `include/`.

## DEC-005: Banco de dados fora da primeira versao

- **Status:** Provisoria
- **Decisao:** Nao escolher nem adicionar banco de dados neste momento.
- **Motivo:** O dominio inicial pode ser atendido pelo sistema de arquivos e a
  necessidade de persistencia adicional ainda nao foi definida.
- **Consequencia:** `data/` sera reservado para exemplos, testes ou futuras
  configuracoes; uma escolha de banco exige nova decisao documentada.

## DEC-006: Mermaid e draw.io para diagramas

- **Status:** Aceita
- **Decisao:** Manter diagramas simples em Mermaid no Markdown e permitir
  diagramas editaveis em draw.io.
- **Motivo:** Mermaid facilita revisao junto ao codigo; draw.io atende a
  diagramas visuais mais detalhados.
- **Consequencia:** Alteracoes de arquitetura devem atualizar os dois formatos
  quando ambos representarem o mesmo fluxo.
