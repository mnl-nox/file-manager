# Objetivos

## Objetivo geral

Construir um gerenciador de arquivos de terminal, escrito em C, que permita
realizar operacoes comuns sobre arquivos e diretorios de forma simples,
previsivel e extensivel.

## Objetivos especificos

- Listar o conteudo do diretorio atual.
- Navegar entre diretorios.
- Criar e remover arquivos e diretorios.
- Copiar e mover arquivos.
- Remover arquivos com confirmacao quando necessario.
- Exibir informacoes basicas, como tipo, tamanho e permissoes.
- Informar erros de forma clara, sem encerrar a aplicacao inesperadamente.
- Manter a camada de interface separada da camada de operacoes do sistema.

## Escopo inicial

A primeira versao deve funcionar em modo interativo no terminal e atender as
operacoes essenciais de navegacao e manipulacao de arquivos. A interface
visual em `ui/` e o armazenamento em `data/` ficam preparados para extensoes,
mas nao sao considerados implementados enquanto nao houver codigo nesses
modulos.

## Fora do escopo inicial

- Sincronizacao com nuvem.
- Gerenciamento de usuarios ou autenticacao.
- Indexacao global do sistema de arquivos.
- Operacoes privilegiadas ou alteracao de configuracoes do sistema.

## Criterios de sucesso

- O programa compila sem warnings com as flags definidas na stack.
- As operacoes basicas retornam sucesso ou erro de maneira verificavel.
- Caminhos invalidos e permissoes insuficientes sao tratados.
- A logica pode ser testada sem depender diretamente da interface.
