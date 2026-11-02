# Stack tecnológica

## Linguagem e padrão

- C11, selecionado explicitamente por `-std=c11` no Makefile.

## Plataforma e interfaces

- Linux como plataforma-alvo.
- libc e interfaces POSIX para arquivos e diretórios.
- Extensões Linux condicionais, incluindo `openat2(2)` e `renameat2(2)`.

A utilização de uma extensão Linux não implica que ela esteja disponível em todos os kernels. Os fallbacks possuem limitações documentadas em [segurança](seguranca.md).

## Ferramentas de build e validação

| Ferramenta | Uso |
|---|---|
| GCC | Compilação e testes no CI. |
| Clang | Compilação e testes no CI. |
| GNU Make | Automação de build e testes. |
| AddressSanitizer | Detecção de erros de memória em execução. |
| UndefinedBehaviorSanitizer | Detecção de classes de comportamento indefinido. |
| Git / GitHub Actions | Versionamento, revisão e integração contínua. |

## Flags do build padrão

O Makefile declara:

```text
-std=c11 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
```

## Comandos

```sh
make
make test
make test-asan
make clean
```

Para detalhes do ambiente e diagnóstico, consulte [desenvolvimento](desenvolvimento.md).
