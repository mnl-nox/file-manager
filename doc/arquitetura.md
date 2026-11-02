# Arquitetura

## Objetivo

Separar apresentação, interpretação de comandos, casos de uso e operações de filesystem. A implementação evita abstrações genéricas prematuras; a fronteira de filesystem ainda é POSIX e poderá ser extraída para um adapter substituível na próxima etapa.

## Componentes e fluxo

```text
src/main.c
  |-- inicializa sessão
  |-- inicia terminal UI
          |
          v
ui/terminal_ui.c
  |-- leitura e renderização
  |-- solicita execução à aplicação
          |
          +------> src/command_parser.c
          |           |-- lexer sem dependências externas
          |
          +------> src/file_manager_application.c
                      |-- valida comandos e aridade
                      |-- coordena casos de uso
                      |-- retorna resultados tipados
                              |
                              v
                       include/file_manager.h
                              |
                              v
                       src/file_manager.c
                              |-- operações POSIX
                              |-- regras de filesystem
                              |-- tradução de errno
```

## Responsabilidades e contratos

- `ui/terminal_ui.c` contém stdin/stdout, prompts de confirmação e renderização. Não executa syscalls de filesystem.
- `src/command_parser.c` tokeniza uma linha in-place; não conhece UI, sessão ou filesystem.
- `src/file_manager_application.c` valida e despacha comandos sem entrada/saída de terminal. Retorna um resultado tipado e sinaliza operações que exigem confirmação.
- `src/file_manager.c` contém o serviço POSIX e não imprime nem lê stdin/stdout.
- `FileManagerResult` transporta sucesso, código de erro, caminho e mensagem.
- `FileManagerList` possui memória alocada pelo serviço; o consumidor deve chamar `fm_list_free`.
- A UI é responsável por pedir confirmação; a aplicação só executa a operação destrutiva quando recebe `confirmed=1`.

## Próxima separação estrutural

O próximo refactor deve introduzir um contrato `FileSystemPort` e mover a implementação POSIX para `adapters/posix/`. A aplicação passará a depender do contrato, permitindo fake filesystem em testes sem trocar o sistema de arquivos real. Esta etapa ainda não está implementada; a camada de aplicação atual continua chamando o serviço POSIX público.

## Dependências

C11, libc e APIs POSIX. `O_NOFOLLOW` é uma extensão disponível nos sistemas Linux alvo. Em Linux, `renameat2(RENAME_NOREPLACE)` é usado para movimentação sem substituição quando suportado. Não há dependências de terceiros.
