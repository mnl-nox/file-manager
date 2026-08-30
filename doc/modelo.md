# Modelo

## Entidades principais

| Entidade        | Descricao                              | Atributos atuais                   |
| --------------- | -------------------------------------- | ---------------------------------- |
| Caminho         | Localizacao de um arquivo ou diretorio | texto recebido e caminho resolvido |
| Entrada         | Item encontrado em um diretorio        | nome e tipo                        |
| Diretorio atual | Contexto de navegacao da sessao        | `current_directory`                |
| Comando         | Acao solicitada pelo usuario           | nome e ate tres argumentos         |
| Resultado       | Retorno de uma operacao                | sucesso, codigo, caminho, mensagem |

## Operacoes

- `listar(caminho)`: retorna as entradas do diretorio.
- `navegar(caminho)`: altera o diretorio atual, se o destino for valido.
- `criar_arquivo(caminho)`: cria um arquivo vazio.
- `criar_diretorio(caminho)`: cria um diretorio.
- `copiar(origem, destino)`: duplica um arquivo.
- `mover(origem, destino)`: altera a localizacao de um item.
- `remover(caminho)`: remove um arquivo ou diretorio conforme as regras da
  operacao.
- `informacoes(caminho)`: consulta metadados usando as APIs do sistema.

## Contrato de retorno

Toda operacao deve retornar um resultado explicito. Em caso de falha, o
resultado deve conter ao menos:

- codigo ou categoria do erro;
- caminho envolvido, quando aplicavel;
- mensagem adequada para a interface.

A camada de interface nao deve precisar interpretar `errno` diretamente.

## Tipos implementados

O header `include/file_manager.h` declara `FileManagerSession`,
`FileManagerEntry`, `FileManagerList` e `FileManagerResult`. A UI usa esses
tipos e nao chama APIs POSIX diretamente.

## Regras iniciais

- Caminhos recebidos do usuario devem ser validados antes da operacao.
- Uma operacao destrutiva deve exigir confirmacao quando essa protecao estiver
  habilitada na interface.
- A listagem deve distinguir arquivos, diretorios e outros tipos.
- A sessao deve manter um unico diretorio atual por instancia do programa.
