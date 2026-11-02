# Requisitos de software

Estado dos requisitos na branch principal após a integração do adapter POSIX e do hardening de caminhos.

## Requisitos funcionais

| ID | Requisito | Estado |
|---|---|---|
| RF-01 | Iniciar no diretório de trabalho atual | Implementado |
| RF-02 | Listar entradas, tipos e metadados | Implementado |
| RF-03 | Navegar entre diretórios | Implementado |
| RF-04 | Criar arquivo vazio sem sobrescrita | Implementado |
| RF-05 | Criar diretório | Implementado |
| RF-06 | Copiar arquivos regulares para destino novo | Implementado |
| RF-07 | Mover/renomear sem sobrescrita intencional | Implementado |
| RF-08 | Remover arquivos/links e diretórios vazios | Implementado; confirmação na UI |
| RF-09 | Consultar tipo, tamanho, modo e mtime | Implementado |
| RF-10 | Interpretar caminhos com espaços, aspas e escapes | Implementado |
| RF-11 | Usar backend substituível por contrato | Implementado |

## Requisitos não funcionais

| ID | Requisito | Estado |
|---|---|---|
| RNF-01 | C11 e warnings habilitados | Implementado |
| RNF-02 | GCC e Clang no CI | Implementado |
| RNF-03 | Testes e sanitizers | Implementado |
| RNF-04 | Separação UI / parser / application / filesystem | Implementado |
| RNF-05 | Linux como plataforma alvo; APIs POSIX para filesystem | Implementado |
| RNF-06 | Site responsivo, sem framework obrigatório | Implementado em `site/` |
| RNF-07 | Deploy automático via GitHub Actions | Configurado; depende de Pages habilitado |

## Restrições conhecidas

- O programa opera com as permissões do usuário e não é sandbox.
- `cp` não copia diretórios nem preserva ACLs, xattrs, ownership ou timestamps.
- `mv` não faz fallback de cópia quando recebe `EXDEV`; fallback sem `renameat2` não oferece atomicidade contra escritores concorrentes.
- Não existe remoção recursiva, busca, filtros, modo TUI ou operações em lote.
- O fallback sem `openat2` não bloqueia magic links.
- A resolução de caminhos não confina a aplicação a uma raiz lógica.
