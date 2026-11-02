<div align="center">

# File Manager

**Gerenciador de arquivos para terminal · Linux · C11 · POSIX**

[![CI](https://github.com/mnl-nox/file-manager/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/mnl-nox/file-manager/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C11](https://img.shields.io/badge/Language-C11-00599C.svg)](https://en.cppreference.com/w/c/11)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)](https://www.kernel.org/)

[Site do projeto](https://mnl-nox.github.io/file-manager/) · [Documentação](doc/README.md) · [Relatar problema](https://github.com/mnl-nox/file-manager/issues) · [Contribuir](CONTRIBUTING.md)

</div>

---

## Resumo

File Manager é uma aplicação de terminal para navegação e operações básicas sobre arquivos e diretórios em sistemas Linux. O software é desenvolvido em C11, utiliza interfaces POSIX para operações de filesystem e mantém uma separação explícita entre interface, interpretação de comandos, lógica de aplicação e acesso ao sistema de arquivos.

O projeto tem caráter educacional e experimental: seu código pode ser utilizado para estudar programação de sistemas, APIs POSIX, gerenciamento de recursos, resolução de caminhos, testes e limites de segurança em operações de filesystem.

> **Estado de segurança:** esta aplicação não é uma sandbox, não estabelece uma raiz de confinamento e não substitui ferramentas especializadas de administração ou recuperação de dados. Leia [SECURITY.md](SECURITY.md) e a [análise de segurança](doc/seguranca.md) antes de utilizá-la sobre dados importantes.

## Objetivos do projeto

- Oferecer um conjunto pequeno e explícito de operações de filesystem.
- Manter os comandos e a interface separados da lógica de operações.
- Tornar o contrato de acesso ao filesystem substituível para testes.
- Documentar semântica, ownership, falhas e limitações de segurança.
- Fornecer uma base legível para estudo e contribuição em C/POSIX.

## Funcionalidades

| Comando | Sintaxe | Descrição |
|---|---|---|
| `ls` | `ls [caminho]` | Lista entradas, tipo, tamanho, permissões e data de modificação. |
| `cd` | `cd <diretório>` | Altera o diretório de trabalho lógico da sessão. |
| `pwd` | `pwd` | Exibe o diretório atual da sessão. |
| `touch` | `touch <arquivo>` | Cria um arquivo vazio sem substituir uma entrada existente. |
| `mkdir` | `mkdir <diretório>` | Cria um diretório. |
| `cp` | `cp <origem> <destino>` | Copia um arquivo regular para um destino novo. |
| `mv` | `mv <origem> <destino>` | Move ou renomeia uma entrada, sem sobrescrita intencional. |
| `rm` | `rm <arquivo|link>` | Remove arquivo ou link simbólico após confirmação. |
| `rmdir` | `rmdir <diretório>` | Remove somente diretórios vazios, após confirmação. |
| `stat` | `stat <caminho>` | Exibe metadados da entrada. |
| `help` | `help` | Exibe a ajuda disponível. |
| `exit` | `exit` | Encerra a aplicação. |

O parser reconhece argumentos delimitados por aspas simples ou duplas e escapes com barra invertida fora de aspas simples. Exemplo:

```text
mkdir "meus documentos"
touch "meus documentos/notas de campo.txt"
cp "meus documentos/notas de campo.txt" "copia de notas.txt"
```

## Requisitos

- Sistema operacional: Linux (plataforma-alvo).
- Compilador C com suporte a C11: GCC ou Clang.
- GNU Make.
- Ferramentas de build e headers de desenvolvimento correspondentes à distribuição.

O código utiliza extensões Linux quando disponíveis, como `openat2(2)` e `renameat2(2)`, e implementa fallback para algumas operações quando a syscall não está disponível. Os fallbacks não têm necessariamente as mesmas propriedades de segurança ou atomicidade.

## Compilação e execução

```sh
git clone https://github.com/mnl-nox/file-manager.git
cd file-manager

make
make test
make test-asan

./file-manager
```

Para uma compilação de depuração:

```sh
make clean
make debug
```

Para remover os binários gerados:

```sh
make clean
```

## Arquitetura em uma visão

```text
Entrada do terminal
        |
        v
UI (ui/terminal_ui.c)
        |
        v
Parser (src/command_parser.c)
        |
        v
Aplicação (src/file_manager_application.c)
        |
        v
FileSystemPort (include/filesystem_port.h)
        |
        v
Adapter POSIX (adapters/posix/filesystem_posix.c)
        |
        v
Operações e syscalls (src/file_manager.c)
        |
        v
Kernel / filesystem
```

O adapter atual fornece uma tabela de funções que conecta o contrato `FileSystemPort` às funções implementadas em `src/file_manager.c`. A implementação das operações POSIX ainda permanece concentrada nesse módulo; portanto, o adapter é uma fronteira de despacho, não uma camada que isolou todas as syscalls em outro componente.

Consulte [Visão arquitetural e estrutura do código](doc/arquitetura.md) para responsabilidades, dependências, ownership e pontos de evolução.

## Segurança e limitações conhecidas

- A aplicação executa com as credenciais e permissões do processo; não eleva privilégios.
- A aplicação não é sandbox e não restringe caminhos a uma raiz.
- A resolução do diretório pai utiliza `openat2(2)` com `RESOLVE_NO_MAGICLINKS` no Linux quando a syscall está disponível. Links simbólicos comuns continuam permitidos.
- O fallback para `open(2)`, quando `openat2(2)` não está disponível ou a ABI não é aceita, não oferece o bloqueio de magic links.
- `cp` aceita somente arquivos regulares, cria um destino novo e não preserva ACLs, xattrs, ownership ou timestamps.
- `mv` pode falhar com `EXDEV`; não realiza cópia e remoção como fallback. Sem `renameat2(RENAME_NOREPLACE)`, a verificação de destino seguida de `renameat` não é uma operação atômica contra escritores concorrentes.
- `rm` não remove diretórios; `rmdir` remove somente diretórios vazios.
- Não há promessa de durabilidade após falha de energia: a cópia não sincroniza explicitamente arquivo e diretório com `fsync(2)`.

A descrição completa está em [Segurança, modelo de ameaças e limites](doc/seguranca.md).

## Qualidade e integração contínua

O workflow [CI](.github/workflows/ci.yml) executa:

1. Build com GCC e Clang.
2. Testes de filesystem, parser e aplicação.
3. Testes com AddressSanitizer e UndefinedBehaviorSanitizer.
4. Verificações estruturais básicas dos arquivos do site.

O workflow [Deploy website](.github/workflows/deploy-site.yml) publica o diretório `site/` no GitHub Pages após alterações elegíveis na branch `main`. A configuração do Pages deve utilizar GitHub Actions como origem de publicação.

## Documentação

A documentação técnica está organizada em [doc/README.md](doc/README.md) e cobre:

- arquitetura e organização do código;
- especificação funcional e restrições;
- modelo de ameaças e semântica de filesystem;
- decisões arquiteturais registradas;
- compilação, testes e integração contínua;
- referências técnicas e acadêmicas.

## Contribuição

Contribuições são bem-vindas. Antes de abrir uma alteração, leia [CONTRIBUTING.md](CONTRIBUTING.md), verifique as [issues abertas](https://github.com/mnl-nox/file-manager/issues) e execute os testes localmente.

Relatos de vulnerabilidade não devem ser publicados como issues abertas. Consulte [SECURITY.md](SECURITY.md).

## Citação acadêmica

Para citar este software em trabalhos acadêmicos, consulte [CITATION.cff](CITATION.cff). As referências bibliográficas e normativas utilizadas na documentação estão em [doc/referencias.md](doc/referencias.md).

## Licença

Distribuído sob a licença MIT. Consulte [LICENSE](LICENSE).
