# Histórico de alterações

Este arquivo registra marcos técnicos do projeto. Para o histórico completo e autoria de cada alteração, consulte o log Git e as Pull Requests.

## 2026-11 — expansão funcional e CLI

- Inclusão de operações de cópia, movimentação, remoção e consulta de metadados.
- Parser independente com suporte a aspas e escapes.
- Separação inicial entre UI, parser e aplicação.
- Ampliação das suítes de testes.

## 2026-11 — port de filesystem e hardening

- Introdução do contrato `FileSystemPort` e adapter POSIX.
- Uso de descritores de diretório e operações *at em operações de entrada final.
- Tentativa de resolução do diretório pai por `openat2(RESOLVE_NO_MAGICLINKS)` no Linux.
- Testes de regressão para magic links e casos de caminho.
- Documentação dos limites de fallback, symlinks e condições de corrida.

## 2026-11 — publicação e documentação

- Site estático responsivo em `site/`.
- CI para GCC, Clang, testes, ASan/UBSan e validação básica do site.
- Deploy do site por GitHub Actions e GitHub Pages.
- Reorganização documental com visão geral, arquitetura, requisitos, desenvolvimento, glossário, ADRs e referências.
