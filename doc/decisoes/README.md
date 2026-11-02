# Registros de decisões arquiteturais

Os Architecture Decision Records (ADRs) registram decisões relevantes, contexto, alternativas e consequências. São documentos históricos: uma decisão substituída deve receber novo ADR, não ser silenciosamente reescrita.

| ADR | Decisão | Estado |
|---|---|---|
| [0001 — Separação por responsabilidades e port de filesystem](0001-separacao-e-filesystem-port.md) | Introduzir interface injetável entre aplicação e operações de filesystem. | Aceita |
| [0002 — Resolução de diretório pai com openat2](0002-openat2-no-magiclinks.md) | Usar `openat2(RESOLVE_NO_MAGICLINKS)` no Linux quando disponível. | Aceita com limitações documentadas |
