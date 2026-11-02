# Guia de desenvolvimento

## 1. Ambiente

Ambiente de referência:
- Linux;
- GCC ou Clang com suporte a C11;
- GNU Make;
- Git.

A aplicação utiliza APIs POSIX e algumas extensões específicas do Linux. A disponibilidade e semântica de extensões podem depender da versão do kernel.

## 2. Obter o código

```sh
git clone https://github.com/mnl-nox/file-manager.git
cd file-manager
```

## 3. Comandos de build

| Comando | Finalidade |
|---|---|
| `make` | Compilação otimizada padrão. |
| `make clean` | Remove executáveis gerados. |
| `make debug` | Compila com símbolos de depuração e otimização reduzida. |
| `make test` | Compila e executa as suítes de teste. |
| `make test-asan` | Executa testes com AddressSanitizer e UndefinedBehaviorSanitizer. |

Exemplo de ciclo local:

```sh
make clean
make
make test
make test-asan
```

Build com compilador alternativo:

```sh
make clean
make CC=clang
make test CC=clang
```

## 4. Organização dos testes

- `test/test_file_manager.c`: operações concretas de filesystem.
- `test/test_command_parser.c`: parsing de comandos.
- `test/test_application.c`: validação e despacho dos casos de uso.

Os testes devem usar diretórios temporários próprios, limpar os artefatos criados e evitar depender de arquivos pessoais ou de estado externo da máquina.

## 5. Depuração

Para gerar binários com símbolos:

```sh
make clean
make debug
```

O executável pode ser inspecionado com ferramentas usuais do Linux, por exemplo:

```sh
file ./file-manager
ldd ./file-manager
strace -f ./file-manager
```

Use `strace` apenas em ambientes nos quais a inspeção de chamadas de sistema seja permitida. A saída pode conter caminhos e informações sensíveis do usuário.

## 6. Integração contínua

O workflow de CI realiza build e testes com GCC e Clang, executa ASan/UBSan e verifica a presença dos arquivos essenciais do site. O workflow de deploy é separado e publica somente o conteúdo de `site/`.

A aprovação do CI não substitui revisão de segurança, análise de semântica POSIX ou testes em diferentes filesystems.

## 7. Diagnóstico de falhas

- **Build falha:** confira versão do compilador, headers de desenvolvimento e warnings.
- **Teste falha isoladamente:** preserve a saída completa e reproduza com o mesmo compilador.
- **ASan/UBSan reporta erro:** trate como defeito até demonstrar o contrário; não desative o sanitizer para mascarar a falha.
- **Comportamento difere entre kernels:** verifique disponibilidade de `openat2` e `renameat2`, além do filesystem utilizado.
