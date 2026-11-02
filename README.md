# File Manager

Gerenciador de arquivos interativo para Linux, escrito em C11 e baseado em APIs POSIX.

**Site:** https://mnl-nox.github.io/file-manager/  
**Código-fonte:** https://github.com/mnl-nox/file-manager

## Visão geral

O File Manager oferece uma interface de terminal para navegar e manipular arquivos e diretórios. O projeto prioriza responsabilidades separadas, operações explícitas, testes automatizados e comportamento de filesystem documentado.

## Requisitos

- Linux (alvo principal).
- GCC ou Clang com suporte a C11.
- GNU Make.

## Compilar e testar

```sh
git clone https://github.com/mnl-nox/file-manager.git
cd file-manager
make
make test
make test-asan
./file-manager
```

## Comandos

| Comando | Comportamento |
|---|---|
| `ls [caminho]` | Lista entradas, tipos, tamanhos, permissões e modificação |
| `cd <diretório>` | Altera o diretório da sessão |
| `pwd` | Exibe o diretório atual |
| `touch <arquivo>` | Cria arquivo vazio; não sobrescreve |
| `mkdir <diretório>` | Cria diretório |
| `cp <origem> <destino>` | Copia arquivo regular para destino inexistente |
| `mv <origem> <destino>` | Move ou renomeia sem sobrescrita intencional |
| `rm <arquivo|link>` | Remove arquivo ou link simbólico após confirmação |
| `rmdir <diretório>` | Remove somente diretório vazio após confirmação |
| `stat <caminho>` | Exibe metadados |
| `help`, `exit` | Ajuda e encerramento |

Use aspas para caminhos com espaços:

```text
mkdir "meus documentos"
touch "meus documentos/notas de campo.txt"
cp "meus documentos/notas de campo.txt" "copia de notas.txt"
```

## Arquitetura

A UI terminal, parser, aplicação e backend filesystem são separados. A aplicação recebe um `FileSystemPort` injetável; o adapter POSIX implementa operações reais. Consulte [doc/arquitetura.md](doc/arquitetura.md).

## Segurança e limites

O programa opera com as permissões do usuário atual e **não é uma sandbox**. No Linux, o diretório pai é resolvido com `openat2(RESOLVE_NO_MAGICLINKS)` quando disponível, e as operações finais usam descritores de diretório. O fallback para kernels sem `openat2` não bloqueia magic links.

- `cp` copia somente arquivos regulares e não preserva ACLs, xattrs, ownership ou timestamps.
- `mv` pode falhar com `EXDEV`; não faz fallback de cópia e remoção.
- Não existe remoção recursiva.
- Symlinks comuns em componentes intermediários são permitidos.
- O projeto não confina caminhos a uma raiz lógica.

Leia [doc/seguranca.md](doc/seguranca.md) antes de operar sobre dados críticos.

## Site e publicação

O site estático fica em [site/](site/). Pull requests validam build, testes, sanitizers e estrutura básica do site. Após merge em `main`, alterações em `site/` são publicadas automaticamente pelo GitHub Actions. Consulte [doc/site-e-deploy.md](doc/site-e-deploy.md).

## Documentação

- [Arquitetura](doc/arquitetura.md)
- [Segurança](doc/seguranca.md)
- [Requisitos](doc/requisitos.md)
- [Site e CI/CD](doc/site-e-deploy.md)
- [Registro de alterações](doc/alteracoes.md)

## Licença

MIT. Consulte [LICENSE](LICENSE).
