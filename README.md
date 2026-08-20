# File Manager em C

![Banner do File Manager](assets/file-manager-banner.svg)

---

<div align="center">
	<img src="https://img.shields.io/badge/C-C11%2B%2B-A8B9CC?logo=c&logoColor=white" alt="C11 ou superior">
	<img src="https://img.shields.io/badge/API-POSIX-222222?logo=linux&logoColor=white" alt="POSIX">
	<img src="https://img.shields.io/badge/OS-Linux-FCC624?logo=linux&logoColor=black" alt="Linux">
	<img src="https://img.shields.io/badge/Compiler-GCC-5C2D91?logo=gnu&logoColor=white" alt="GCC">
	<br>
	<img src="https://img.shields.io/badge/Tools-Git-F05032?logo=git&logoColor=white" alt="Git">
	<img src="https://img.shields.io/badge/Docs-Markdown-000000?logo=markdown&logoColor=white" alt="Markdown">
	<img src="https://img.shields.io/badge/Diagrams-draw.io-F08705?logo=diagrams.net&logoColor=white" alt="draw.io">
</div>

---

Gerenciador de arquivos simples desenvolvido em C para praticar manipulacao
de diretorios, arquivos e comandos no terminal.

## Objetivo

Este repositorio serve como base para um gerenciador de arquivos de linha de
comando, com foco em uma implementacao pequena, clara e facil de expandir.

## Funcionalidades previstas

- Listar arquivos e diretorios
- Navegar entre diretorios
- Criar, copiar, mover e remover arquivos
- Criar e remover diretorios
- Exibir informacoes basicas sobre arquivos

## Requisitos

- Compilador C com suporte ao padrao C11 ou superior
- Sistema operacional compatível com as APIs de arquivos utilizadas pelo projeto

## Stack e ferramentas

### Stack principal

- **C11 ou superior** para a implementacao do gerenciador.
- **GCC ou Clang** para compilacao.
- **libc e APIs POSIX** para arquivos, diretorios, metadados e permissoes.
- **Linux** como ambiente alvo inicial.
- **Git** para controle de versao.

### Documentacao

- **Markdown** para README e documentacao tecnica.
- **draw.io** para criacao e manutencao de diagramas de arquitetura e UML.

### Banco de dados

O uso de banco de dados permanece **opcional** e ainda nao foi definido. A
versao inicial deve operar diretamente sobre o sistema de arquivos, sem
introduzir uma dependencia de banco antes da avaliacao dos requisitos.

## Compilacao

Quando os fontes estiverem disponiveis, compile o projeto com um compilador C.
Por exemplo:

```bash
gcc -std=c11 -Wall -Wextra -o file-manager *.c
```

Execute com:

```bash
./file-manager
```

## Repositorio de referencia

Este projeto utiliza como referencia o repositorio [c-lab](https://github.com/mnl-nox/c-lab).

## Documentacao

A documentacao tecnica esta em [`doc/`](doc/), incluindo objetivos,
arquitetura, modelo, diagramas UML e stack utilizada.

## Licenca

Distribuido sob a [Licenca MIT](LICENSE).
