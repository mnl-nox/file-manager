# Modelagem

## Modelo de dominio

O sistema representa uma sessao de navegacao que opera sobre entradas do
sistema de arquivos. Uma entrada pode ser um arquivo, diretorio ou outro tipo
reconhecido pelo sistema operacional.

```mermaid
classDiagram
    class Sessao {
        +string diretorioAtual
        +iniciar()
        +encerrar()
    }

    class Comando {
        +string nome
        +string[] argumentos
    }

    class Caminho {
        +string valor
        +resolver()
        +validar()
    }

    class Entrada {
        +string nome
        +TipoEntrada tipo
        +long tamanho
        +string permissoes
    }

    class Resultado {
        +bool sucesso
        +int codigo
        +string mensagem
    }

    Sessao --> Comando : recebe
    Sessao --> Caminho : usa
    Caminho --> Entrada : localiza
    Comando --> Resultado : produz
```

## Fluxo de dados

```mermaid
flowchart LR
    EntradaUsuario[Entrada do usuario]
    Parser[Parser de comandos]
    Validador[Validador de argumentos]
    Servico[Servico de arquivos]
    Sistema[Sistema de arquivos]
    Resposta[Resultado formatado]

    EntradaUsuario --> Parser
    Parser --> Validador
    Validador --> Servico
    Servico --> Sistema
    Sistema --> Servico
    Servico --> Resposta
    Resposta --> EntradaUsuario
```

## Estados de uma operacao

```mermaid
stateDiagram-v2
    [*] --> Recebida
    Recebida --> Validando
    Validando --> Executando: argumentos validos
    Validando --> Falha: argumentos invalidos
    Executando --> Sucesso: operacao concluida
    Executando --> Falha: erro do sistema
    Sucesso --> [*]
    Falha --> [*]
```

## Estruturas de dados previstas

- `Comando`: nome da acao e lista de argumentos.
- `Caminho`: texto original e, quando necessario, forma normalizada.
- `Entrada`: metadados retornados por `readdir` e `stat`.
- `Resultado`: status, codigo e mensagem para a camada de interface.
- `Sessao`: contexto do diretorio atual e configuracoes da execucao.

Os tipos concretos estao definidos em `include/file_manager.h`. Nesta primeira
versao, `Entrada` contem nome e tipo; tamanho e permissoes serao incluidos ao
implementar a consulta de metadados.
