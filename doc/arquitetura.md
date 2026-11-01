# Arquitetura

## Objetivo

Manter a interface substituível e concentrar regras de filesystem em um serviço sem dependência de UI. A arquitetura atual é pequena por decisão: evitar abstrações genéricas antes de existirem múltiplos adaptadores reais.

## Componentes e fluxo

```text
src/main.c
  |-- inicializa FileManagerSession
  |-- inicia terminal UI
          |
          v
ui/terminal_ui.c
  |-- leitura da entrada
  |-- chamada ao parser
  |-- validação de aridade e despacho
  |-- renderização de resultados
          |
          +------> src/command_parser.c
          |           |-- lexer com aspas e escapes
          |
          +------> include/file_manager.h
                      |
                      v
                  src/file_manager.c
                      |-- resolução de caminhos da sessão
                      |-- operações POSIX
                      |-- tradução de errno para FileManagerResult
```

## Contratos

- A UI não chama diretamente APIs de filesystem.
- O parser não conhece UI, sessão ou filesystem; recebe uma linha e devolve tokens.
- O serviço não imprime nem lê stdin/stdout.
- `FileManagerResult` transporta sucesso, código de erro, caminho e mensagem.
- `FileManagerList` possui memória alocada pelo serviço; o consumidor deve chamar `fm_list_free`.
- Entradas carregam tipo, tamanho, modo e timestamp de modificação.
- Os métodos de criação e cópia não sobrescrevem destinos existentes.

## Próxima separação estrutural

O próximo passo arquitetural é extrair casos de uso para `application/` e manter POSIX como adapter de filesystem. Isso deve ocorrer sem criar interfaces artificiais: primeiro separar os módulos que já possuem responsabilidades independentes, depois introduzir ports apenas onde houver necessidade de substituição em testes.

## Dependências

C11, libc e APIs POSIX. `O_NOFOLLOW` é uma extensão disponível nos sistemas Linux alvo. Em Linux, `renameat2(RENAME_NOREPLACE)` é usado para movimentação sem substituição quando suportado. Não há dependências de terceiros.
