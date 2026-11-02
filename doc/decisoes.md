# Decisões de projeto

Os registros formais de decisões arquiteturais estão no diretório [decisoes/](decisoes/).

| Registro legado | Estado documental |
|---|---|
| DEC-001 — linguagem C11+ | Consolidado em [visão geral](visao-geral.md) e [requisitos](requisitos.md). O build atual utiliza `-std=c11`. |
| DEC-002 — interface de terminal | Mantido; ver [visão geral](visao-geral.md). |
| DEC-003 — APIs POSIX/libc | Atualizado: o projeto é Linux-first e utiliza interfaces POSIX e extensões Linux. Ver [stack e arquitetura](arquitetura.md). |
| DEC-004 — separação de interface e filesystem | Evoluiu para o contrato `FileSystemPort`; ver [ADR-0001](decisoes/0001-separacao-e-filesystem-port.md). |
| DEC-005 — banco de dados fora do escopo | Sem impacto na implementação atual; ver [não objetivos](visao-geral.md#6-nao-objetivos). |
| DEC-006 — Mermaid e draw.io | Mermaid permanece utilizado nos documentos Markdown. |
| DEC-007 — UI sem bibliotecas externas | Mantido como diretriz; o parser atual aceita aspas e escapes. |

Este arquivo é mantido como índice de compatibilidade para links existentes. Novas decisões devem ser registradas como ADRs em [decisoes/](decisoes/), sem alterar retrospectivamente o histórico.
