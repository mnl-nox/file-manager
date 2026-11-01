# Requisitos de software

Estado dos requisitos nesta entrega da branch `feat/complete-file-manager`.

## Requisitos funcionais

| ID | Requisito | Prioridade | Estado / aceite |
|---|---|---:|---|
| RF-01 | Iniciar no diretorio de trabalho atual | Alta | Implementado |
| RF-02 | Listar entradas e tipos | Alta | Implementado; inclui metadados e links |
| RF-03 | Navegar entre diretorios | Alta | Implementado |
| RF-04 | Criar arquivo vazio | Alta | Implementado; falha se destino existir |
| RF-05 | Criar diretorio | Alta | Implementado |
| RF-06 | Copiar arquivo | Alta | Implementado para arquivos regulares; destino novo |
| RF-07 | Mover/renomear item | Alta | Implementado; limitações de atomicidade descritas em seguranca.md |
| RF-08 | Remover arquivo/link e diretorio vazio | Alta | Implementado; confirmação na UI; sem recursão |
| RF-09 | Consultar metadados | Media | Implementado: tipo, tamanho, modo e mtime |
| RF-10 | Exibir resultados e erros | Alta | Implementado |
| RF-11 | Encerrar sessão | Alta | Implementado |
| RF-12 | Interpretar caminhos com espaços | Alta | Implementado via aspas e escapes no lexer |

## Requisitos nao funcionais

| ID | Requisito | Prioridade | Estado |
|---|---|---:|---|
| RNF-01 | C11 | Alta | Implementado no Makefile |
| RNF-02 | Warnings habilitados | Alta | Flags rigorosas habilitadas; CI deve confirmar por compilador |
| RNF-03 | Alvo Linux/POSIX | Alta | Implementação POSIX com extensão Linux `O_NOFOLLOW` |
| RNF-04 | Interface terminal | Alta | Implementado |
| RNF-05 | Serviço sem dependência de UI | Alta | Implementado |
| RNF-06 | Erros de sistema tratados | Alta | Implementado nos caminhos principais |
| RNF-07 | Sem banco de dados | Alta | Implementado |
| RNF-08 | Ownership e liberação de memória | Alta | Lista liberada por `fm_list_free`; validar com sanitizers |
| RNF-09 | Documentação Markdown | Media | Implementado |
| RNF-10 | Módulos com headers públicos e responsabilidades separadas | Media | Parser e application separados; FileSystemPort/adapter POSIX permanece planejado |

## Restrições e limites

- O programa opera com as permissões POSIX do usuário e não é sandbox.
- `cp` não copia diretórios nem metadados estendidos.
- `mv` não faz fallback para cópia quando recebe `EXDEV`.
- Não existe remoção recursiva.
- O hardening contra TOCTOU com descritores de diretório permanece pendente.
- O comportamento detalhado está em [seguranca.md](seguranca.md).
