# Requisitos de software

Este documento define os requisitos iniciais do File Manager. Como o codigo
ainda nao foi implementado, os requisitos abaixo sao a linha de base para a
primeira versao e podem ser refinados durante a implementacao.

## Requisitos funcionais (RF)

| ID    | Requisito                                                      | Prioridade | Criterio de aceite                                                         |
| ----- | -------------------------------------------------------------- | ---------- | -------------------------------------------------------------------------- |
| RF-01 | O sistema deve iniciar no diretorio de trabalho atual.         | Alta       | Ao iniciar, o caminho atual e valido e pode ser exibido.                   |
| RF-02 | O usuario deve poder listar arquivos e diretorios.             | Alta       | A listagem informa nome e tipo de cada entrada.                            |
| RF-03 | O usuario deve poder navegar para outro diretorio.             | Alta       | Caminhos validos alteram o diretorio atual; caminhos invalidos geram erro. |
| RF-04 | O usuario deve poder criar arquivos vazios.                    | Alta       | O arquivo e criado no caminho informado quando houver permissao.           |
| RF-05 | O usuario deve poder criar diretorios.                         | Alta       | O diretorio e criado e pode ser localizado na listagem.                    |
| RF-06 | O usuario deve poder copiar arquivos.                          | Alta       | O destino e criado com o conteudo da origem.                               |
| RF-07 | O usuario deve poder mover ou renomear itens.                  | Alta       | O item deixa de existir na origem e aparece no destino.                    |
| RF-08 | O usuario deve poder remover arquivos e diretorios permitidos. | Alta       | O sistema solicita confirmacao quando aplicavel e informa o resultado.     |
| RF-09 | O usuario deve poder consultar metadados de um item.           | Media      | O sistema exibe tipo, tamanho e permissoes quando disponiveis.             |
| RF-10 | O sistema deve exibir mensagens para sucesso e falha.          | Alta       | Cada comando termina com uma resposta compreensivel.                       |
| RF-11 | O usuario deve poder encerrar a sessao pelo comando de saida.  | Alta       | O programa finaliza sem deixar recursos abertos.                           |

## Requisitos nao funcionais (RNF)

| ID     | Requisito                                                                      | Prioridade | Verificacao                                               |
| ------ | ------------------------------------------------------------------------------ | ---------- | --------------------------------------------------------- |
| RNF-01 | O programa deve ser implementado em C11 ou superior.                           | Alta       | Compilacao com o padrao definido.                         |
| RNF-02 | A compilacao deve ocorrer sem warnings com `-Wall -Wextra -pedantic`.          | Alta       | Saida do compilador sem warnings.                         |
| RNF-03 | A aplicacao deve ter como alvo inicial sistemas Unix-like com APIs POSIX.      | Alta       | Execucao validada em Linux.                               |
| RNF-04 | A interface deve funcionar no terminal sem depender de interface grafica.      | Alta       | Execucao em uma sessao de shell.                          |
| RNF-05 | A logica de arquivos deve ser separada da interface.                           | Alta       | Testes da camada de servico sem entrada interativa.       |
| RNF-06 | Erros de sistema e permissoes devem ser tratados sem encerramento abrupto.     | Alta       | Testes com caminhos invalidos e permissoes insuficientes. |
| RNF-07 | O programa nao deve depender de banco de dados na primeira versao.             | Alta       | Execucao usando apenas o sistema de arquivos.             |
| RNF-08 | A memoria alocada deve ser liberada antes do encerramento.                     | Alta       | Analise com Sanitizers ou Valgrind.                       |
| RNF-09 | A documentacao deve permanecer em Markdown e incluir diagramas atualizaveis.   | Media      | Revisao de `doc/` e dos diagramas Mermaid/draw.io.        |
| RNF-10 | O codigo deve ser organizado em modulos com interfaces publicas em `include/`. | Media      | Revisao da estrutura e dos headers.                       |

## Dependencias e restricoes

- Acoes destrutivas devem respeitar permissoes do sistema operacional.
- O comportamento de links simbolicos e diretorios recursivos deve ser
  definido antes de implementar essas operacoes.
- O banco de dados e uma possibilidade futura, nao um requisito da primeira
  versao.
