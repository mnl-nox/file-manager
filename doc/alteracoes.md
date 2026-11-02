# Registro de alterações

## 2026-11 — integração de arquitetura e segurança

- Expansão da CLI com `cp`, `mv`, `rm`, `rmdir` e `stat`.
- Parser independente com suporte a aspas, escapes e caminhos com espaços.
- Separação da UI e dos casos de uso.
- Introdução do `FileSystemPort` injetável e adapter POSIX.
- Operações de entrada final relativas a descritores de diretório.
- Resolução do diretório pai com `openat2(RESOLVE_NO_MAGICLINKS)` no Linux quando disponível.
- Testes GCC/Clang e ASan/UBSan.

## 2026-11 — hardening e experiência de projeto

- Erros de metadados de listagem deixam de ser ocultados; entradas removidas durante a enumeração podem ser ignoradas.
- Caminhos com barra final exigem diretório.
- `stat .` e `stat ..` são aceitos.
- Site estático responsivo em `site/`.
- CI valida build, testes, sanitizers e estrutura do site.
- GitHub Actions publica o site em push para `main` quando `site/` muda.
