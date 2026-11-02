# Diagramas de análise e projeto

Os diagramas são mantidos em Mermaid para que alterações arquiteturais possam ser revisadas junto ao código.

## Diagrama de componentes

```mermaid
flowchart TD
    UI["Terminal UI"] --> PARSER["Command parser"]
    PARSER --> APP["Application"]
    APP --> PORT["FileSystemPort"]
    PORT --> ADAPTER["POSIX adapter"]
    ADAPTER --> CORE["src/file_manager.c"]
    CORE --> KERNEL["Linux / filesystem"]
    ROOT["main.c"] -. "composition" .-> UI
    ROOT -. "dependency injection" .-> APP
```

O adapter é uma tabela de encaminhamento. A implementação das operações e syscalls ainda reside em `src/file_manager.c`.

## Diagrama de sequência: listagem

```mermaid
sequenceDiagram
    actor User as Usuário
    participant UI as Terminal UI
    participant Parser
    participant App as Application
    participant Port as FileSystemPort
    participant Core as Filesystem core
    participant Kernel as Kernel

    User->>UI: ls [caminho]
    UI->>Parser: linha de comando
    Parser-->>UI: argc / argv
    UI->>App: executar comando
    App->>Port: list_directory()
    Port->>Core: fm_list_directory()
    Core->>Kernel: opendir / readdir / fstatat
    Kernel-->>Core: entradas / errno
    Core-->>Port: lista e resultado
    Port-->>App: resultado
    App-->>UI: dados ou erro
    UI-->>User: renderização
```

## Diagrama de caso de uso

```mermaid
flowchart LR
    User["Usuário"]
    User --> LS["Listar diretório"]
    User --> CD["Navegar"]
    User --> CREATE["Criar arquivo/diretório"]
    User --> COPY["Copiar arquivo"]
    User --> MOVE["Mover/renomear"]
    User --> REMOVE["Remover arquivo/link"]
    User --> RMDIR["Remover diretório vazio"]
    User --> STAT["Consultar metadados"]
```

Para responsabilidades, contratos e limites de cada componente, consulte [arquitetura](arquitetura.md). Para comportamento de caminhos e syscalls, consulte [segurança](seguranca.md).
