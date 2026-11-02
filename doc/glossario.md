# Glossário técnico

| Termo | Definição neste projeto |
|---|---|
| Adapter | Implementação concreta que conecta a aplicação a uma interface externa. O adapter POSIX atual encaminha chamadas para funções do core. |
| API | Interface de programação oferecida por biblioteca ou sistema. |
| ASan | AddressSanitizer, detector de erros de memória em tempo de execução. |
| C11 | Edição de 2011 do padrão da linguagem C, publicada como ISO/IEC 9899:2011. |
| dirfd | Descritor de arquivo que identifica um diretório e pode servir de referência para operações relativas. |
| errno | Mecanismo tradicional POSIX para comunicar a causa de falhas de chamadas de sistema e funções de biblioteca. |
| Filesystem | Organização lógica e implementação de armazenamento de arquivos e diretórios. |
| Magic link | Link especial do Linux, como entradas de `/proc`, cuja resolução possui semântica diferente de symlinks comuns. |
| POSIX | Família de normas IEEE/Open Group que padroniza interfaces de sistemas operacionais. |
| Port | Contrato de operações consumido pela aplicação e implementável por adapters distintos. |
| Race condition | Resultado dependente de interleaving entre operações concorrentes. |
| Symlink | Link simbólico que referencia outro caminho no filesystem. |
| TOCTOU | Time-of-check to time-of-use: condição de corrida entre verificação de um objeto e seu uso posterior. |
| UBSan | UndefinedBehaviorSanitizer, detector de determinadas formas de comportamento indefinido. |
| Umask | Máscara do processo que restringe permissões solicitadas durante a criação de arquivos e diretórios. |

## Convenções

- **Entrada final (leaf):** último componente de um caminho.
- **Diretório pai:** diretório que contém a entrada final.
- **Destino exclusivo:** destino cuja criação falha se já existir.
- **Confinamento:** garantia de que a resolução de caminhos permanece dentro de uma raiz definida.
