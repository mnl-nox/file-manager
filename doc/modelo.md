# Modelo de domínio e contratos

Este documento complementa a [modelagem](modelagem.md) com os contratos públicos da implementação.

## Operações do domínio

| Caso de uso | Função de filesystem | Resultado |
|---|---|---|
| Inicializar sessão | `fm_session_init` | `FileManagerResult` |
| Navegar | `change_directory` no port | `FileManagerResult` |
| Listar | `list_directory` no port | `FileManagerList` + resultado |
| Criar arquivo | `create_file` no port | `FileManagerResult` |
| Criar diretório | `create_directory` no port | `FileManagerResult` |
| Copiar | `copy_file` no port | `FileManagerResult` |
| Mover/renomear | `move_entry` no port | `FileManagerResult` |
| Remover arquivo/link | `remove_file` no port | `FileManagerResult` |
| Remover diretório vazio | `remove_directory` no port | `FileManagerResult` |
| Consultar metadados | `get_info` no port | `FileManagerEntry` + resultado |

## Contrato de erro

As operações retornam um resultado explícito com indicador de sucesso, código de erro, caminho e mensagem. O campo `error_code` mantém o código associado ao erro do sistema, quando aplicável.

A camada de aplicação faz validação de comando e argumentos. A UI apresenta mensagens e conduz a confirmação para `rm` e `rmdir`.

## Regras de consistência

- Não confundir confirmação da UI com autorização de segurança.
- Não assumir que o caminho textual identifica permanentemente o mesmo objeto.
- Não assumir que uma operação com várias syscalls é transacional.
- Não omitir a liberação de `FileManagerList`.
- Não assumir que o port atual isola completamente a implementação POSIX: ele é uma tabela de encaminhamento para funções do core.
