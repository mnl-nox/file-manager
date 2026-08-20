# UML

Os diagramas usam Mermaid e podem ser visualizados em editores que suportam
Markdown com Mermaid.

## Casos de uso

```mermaid
flowchart LR
    U[Usuario]
    L[Listar diretorio]
    N[Navegar]
    C[Criar item]
    M[Copiar ou mover]
    R[Remover item]
    I[Consultar informacoes]

    U --> L
    U --> N
    U --> C
    U --> M
    U --> R
    U --> I
```

## Componentes

```mermaid
flowchart TB
    UI[Interface de terminal]
    CMD[Interpretador de comandos]
    FS[Servico de arquivos]
    ERR[Tratamento de erros]
    OS[Sistema operacional]

    UI --> CMD
    CMD --> FS
    FS --> ERR
    FS --> OS
    ERR --> UI
```

## Sequencia: listar diretorio

```mermaid
sequenceDiagram
    actor Usuario
    participant UI as Interface
    participant CMD as Comandos
    participant FS as Servico de arquivos
    participant SO as Sistema operacional

    Usuario->>UI: informa listar [caminho]
    UI->>CMD: envia comando e argumentos
    CMD->>FS: solicita listagem
    FS->>SO: abre e le diretorio
    SO-->>FS: entradas ou erro
    FS-->>CMD: resultado padronizado
    CMD-->>UI: dados formatados
    UI-->>Usuario: exibe listagem
```

## Observacao

Os diagramas representam a arquitetura planejada. Devem ser atualizados quando
os modulos e contratos publicos forem implementados.
