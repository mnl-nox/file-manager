# Arquitetura

## Visao geral

A aplicacao deve ser organizada em camadas pequenas, com responsabilidades
bem definidas:

```text
+----------------------+
| Interface de terminal|
|        (ui/)         |
+----------+-----------+
           |
           v
+----------------------+
| Comandos e controle  |
|        (src/)        |
+----------+-----------+
           |
           v
+----------------------+
| Operacoes de arquivo |
|        (src/)        |
+----------+-----------+
           |
           v
+----------------------+
| Sistema operacional  |
| POSIX / libc         |
+----------------------+
```

## Responsabilidades

### Interface (`ui/`)

- Ler comandos e argumentos.
- Exibir listagens, resultados e mensagens de erro.
- Nao executar diretamente chamadas de baixo nivel quando houver uma camada
  de servico disponivel.

### Controle e comandos (`src/`)

- Interpretar a entrada do usuario.
- Validar argumentos e caminhos.
- Encaminhar cada comando para o servico correspondente.
- Converter resultados em estados que a interface possa apresentar.

### Operacoes de arquivo (`src/` e `include/`)

- Encapsular chamadas como `opendir`, `readdir`, `stat`, `mkdir`, `remove`,
  `rename` e operacoes de leitura e escrita.
- Padronizar retorno de sucesso e erro.
- Evitar que detalhes do sistema operacional vazem para a interface.

### Dados (`data/`)

No escopo inicial, `data/` pode armazenar arquivos de teste, exemplos ou
configuracoes locais. O gerenciador deve tratar os arquivos do sistema como
dados externos e nao depender de um banco de dados.

## Fluxo de uma operacao

1. O usuario informa um comando no terminal.
2. A interface separa comando e argumentos.
3. O controlador valida a entrada.
4. O servico de arquivos executa a operacao.
5. O resultado e devolvido a interface.
6. A interface apresenta sucesso ou erro.

## Principios

- Separacao de responsabilidades.
- Dependencias explicitas por headers em `include/`.
- Tratamento de erros proximo da chamada que pode falhar.
- Funcoes pequenas e testaveis.
- Portabilidade sempre que nao comprometer a clareza.
