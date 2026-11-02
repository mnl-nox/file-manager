# Especificação de requisitos de software

## 1. Convenções

Os requisitos abaixo descrevem a versão atualmente presente na branch principal.

- **Implementado:** comportamento coberto pelo código atual.
- **Implementado com restrições:** funcionalidade disponível com limitações descritas.
- **Não implementado:** não há suporte no código atual.

## 2. Requisitos funcionais

| ID | Requisito | Estado | Verificação principal |
|---|---|---|---|
| RF-01 | Inicializar sessão a partir do diretório atual do processo. | Implementado | Testes de sessão. |
| RF-02 | Listar entradas e metadados básicos. | Implementado | Testes de listagem. |
| RF-03 | Alterar diretório da sessão. | Implementado | Testes de navegação. |
| RF-04 | Criar arquivo vazio sem sobrescrever entrada existente. | Implementado | Testes de criação. |
| RF-05 | Criar diretório. | Implementado | Testes de criação. |
| RF-06 | Copiar arquivo regular para destino inexistente. | Implementado com restrições | Testes de cópia e erros. |
| RF-07 | Mover ou renomear entrada sem sobrescrita intencional. | Implementado com restrições | Testes de movimentação. |
| RF-08 | Remover arquivo ou link simbólico. | Implementado | Testes de remoção e confirmação na aplicação. |
| RF-09 | Remover diretório vazio. | Implementado | Testes de remoção e confirmação na aplicação. |
| RF-10 | Consultar tipo, tamanho, modo e data de modificação. | Implementado | Testes de metadados. |
| RF-11 | Interpretar aspas e escapes na linha de comando. | Implementado | Suíte do parser. |
| RF-12 | Despachar operações por contrato de filesystem injetável. | Implementado | Suíte da aplicação. |

## 3. Requisitos não funcionais

| ID | Requisito | Estado | Evidência |
|---|---|---|---|
| RNF-01 | Compilar como C11. | Implementado | `Makefile`. |
| RNF-02 | Habilitar warnings relevantes no build padrão. | Implementado | `-Wall -Wextra -Wpedantic -Wconversion -Wshadow`. |
| RNF-03 | Construir com GCC e Clang no CI. | Implementado | `.github/workflows/ci.yml`. |
| RNF-04 | Executar testes automatizados no CI. | Implementado | `make test`. |
| RNF-05 | Executar testes com ASan e UBSan. | Implementado | `make test-asan`. |
| RNF-06 | Separar UI, parser, aplicação e contrato de filesystem. | Implementado | Diretórios `src/`, `ui/`, `include/` e `adapters/`. |
| RNF-07 | Manter documentação de segurança e limites. | Implementado | Este conjunto documental. |
| RNF-08 | Disponibilizar apresentação web responsiva. | Implementado | Diretório `site/`. |
| RNF-09 | Publicar o site por GitHub Actions. | Configurado | `.github/workflows/deploy-site.yml`; requer Pages habilitado. |

## 4. Restrições técnicas

- Plataforma-alvo: Linux.
- Linguagem: C11.
- Dependências de build: compilador C e GNU Make.
- Interfaces: POSIX e extensões Linux utilizadas condicionalmente.
- Não há suporte garantido para outras famílias de sistemas operacionais.
- O tamanho máximo de caminho e nome é limitado pelas constantes definidas em `include/file_manager.h`.

## 5. Restrições funcionais

Não são oferecidos:
- cópia de diretórios;
- remoção recursiva;
- busca de arquivos;
- filtros ou operações em lote;
- preservação integral de metadados;
- sincronização explícita para durabilidade;
- confinamento em raiz de filesystem.

## 6. Critérios de aceitação para mudanças

Uma alteração deve:
1. manter o build C11 sem warnings novos não justificados;
2. incluir testes para novos caminhos de sucesso e falha;
3. liberar recursos alocados em todos os caminhos de execução;
4. preservar códigos de erro relevantes;
5. atualizar esta especificação se o comportamento externo mudar;
6. atualizar a análise de segurança quando alterar resolução de caminhos, symlinks ou operações destrutivas.
