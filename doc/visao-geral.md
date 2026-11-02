# Visão geral e escopo

## 1. Identificação

File Manager é um utilitário de terminal para operações básicas de filesystem em Linux, implementado em C11. O projeto também funciona como material de estudo sobre interfaces de sistema, decomposição de software, gerenciamento explícito de recursos e segurança de caminhos.

## 2. Problema e motivação

Operações simples sobre arquivos são uma oportunidade concreta para estudar a fronteira entre uma aplicação em espaço de usuário e o kernel. A aplicação transforma comandos textuais em operações de filesystem e precisa lidar com erros, permissões, nomes, links simbólicos, concorrência e recursos do sistema.

O projeto busca tornar esse percurso observável e documentado, evitando esconder a semântica do sistema operacional atrás de uma abstração excessivamente ampla.

## 3. Objetivos

1. Disponibilizar uma CLI pequena para operações frequentes.
2. Separar parsing, orquestração de casos de uso e acesso ao filesystem.
3. Definir um contrato substituível para operações de filesystem.
4. Explicitar comportamento de erro, ownership e limites de segurança.
5. Manter compilação e testes acessíveis com ferramentas comuns do ecossistema C.

## 4. Público-alvo

- estudantes de programação de sistemas e sistemas operacionais;
- desenvolvedores C interessados em APIs POSIX/Linux;
- contribuidores de projetos open source de pequeno porte;
- pesquisadores e profissionais que desejem inspecionar uma implementação didática de operações de filesystem.

## 5. Escopo funcional

O escopo atual inclui listagem, navegação, criação de arquivos e diretórios, cópia de arquivos regulares, movimentação/renomeação, remoção de arquivos e links, remoção de diretórios vazios e consulta de metadados.

A aplicação não pretende substituir um gerenciador de arquivos gráfico, um shell completo ou ferramentas especializadas como `cp`, `mv`, `rm`, `find` e utilitários de recuperação.

## 6. Não objetivos

Não fazem parte do escopo implementado:

- confinamento em sandbox ou jail;
- elevação de privilégios;
- cópia recursiva de diretórios;
- remoção recursiva;
- preservação completa de ACLs, atributos estendidos, ownership e timestamps;
- sincronização transacional ou garantia de durabilidade após queda de energia;
- suporte multiplataforma garantido;
- interface gráfica ou TUI avançada;
- shell scripting, pipes, redirecionamentos ou expansão de variáveis.

## 7. Princípios de engenharia

- **Explicitabilidade:** operações e efeitos colaterais devem ser claros.
- **Separação de responsabilidades:** UI, parser, aplicação e filesystem têm papéis distintos.
- **Falha observável:** erros relevantes não devem ser silenciosamente convertidos em sucesso.
- **Ownership explícito:** recursos alocados e descritores devem ter responsáveis definidos.
- **Verificabilidade:** mudanças devem incluir testes adequados ao comportamento alterado.
- **Honestidade de segurança:** mecanismos de mitigação não devem ser descritos como garantias mais amplas do que realmente oferecem.
