# Objetivos e escopo

Este arquivo preserva o endereço documental histórico. A definição atual do projeto está centralizada em [visao-geral.md](visao-geral.md).

## Objetivo geral

Desenvolver um gerenciador de arquivos de terminal para Linux, em C11, que permita operações explícitas sobre arquivos e diretórios e sirva como base de estudo de programação de sistemas e interfaces POSIX.

## Objetivos específicos atuais

- listar entradas e metadados básicos;
- navegar entre diretórios;
- criar arquivos e diretórios;
- copiar arquivos regulares;
- mover ou renomear entradas;
- remover arquivos, links simbólicos e diretórios vazios;
- consultar metadados;
- separar UI, parser, aplicação e contrato de filesystem;
- manter testes automatizados e documentação de segurança.

## Fora do escopo

Não há suporte a sandbox, operações privilegiadas, remoção recursiva, cópia recursiva, sincronização em nuvem, indexação global ou interface gráfica.

Consulte [requisitos](requisitos.md) para o estado verificável de cada funcionalidade.
