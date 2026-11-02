# Guia de contribuição

Obrigado pelo interesse em contribuir com o File Manager. Este documento define um fluxo mínimo para manter alterações reproduzíveis, revisáveis e coerentes com o escopo do projeto.

## 1. Antes de começar

- Leia o [README](README.md) e a [documentação técnica](doc/README.md).
- Consulte issues e Pull Requests existentes para evitar trabalho duplicado.
- Para mudanças de comportamento, descreva o problema e a semântica desejada antes de implementar.
- Para alterações de segurança, leia [SECURITY.md](SECURITY.md) e [doc/seguranca.md](doc/seguranca.md).

## 2. Fluxo recomendado

1. Crie uma branch a partir de `main`.
2. Faça uma alteração focada, evitando misturar refatoração ampla e mudança funcional sem necessidade.
3. Adicione ou atualize testes.
4. Execute build e testes localmente.
5. Atualize documentação e requisitos afetados.
6. Abra Pull Request com motivação, escopo, riscos e evidências de validação.

## 3. Padrões de código

- Mantenha compatibilidade com C11.
- Preserve os warnings habilitados no Makefile.
- Verifique retornos de chamadas de sistema e funções de biblioteca.
- Preserve o valor de `errno` antes de executar operações de limpeza que possam alterá-lo.
- Libere memória e descritores em caminhos de sucesso e falha.
- Evite `system()` para operações de filesystem.
- Não introduza dependências externas sem justificar custo, manutenção e superfície de ataque.
- Não transforme mensagens de erro em sucesso silencioso.

## 4. Testes obrigatórios

Antes de solicitar revisão:

```sh
make clean
make
make test
make test-asan
```

Quando aplicável, execute também com Clang:

```sh
make clean
make CC=clang
make test CC=clang
```

Mudanças de resolução de caminhos, symlinks ou operações destrutivas exigem testes de regressão específicos e atualização da análise de segurança.

## 5. Documentação

Atualize a documentação correspondente quando alterar:
- comandos ou semântica de operações;
- contratos de API ou ownership;
- requisitos;
- arquitetura ou dependências;
- comportamento de segurança;
- build, testes ou publicação.

Registre decisões arquiteturais relevantes em ADRs. ADRs são registros históricos; decisões posteriores devem ser registradas em novos arquivos.

## 6. Pull Requests

Uma PR deve conter:
- contexto e problema;
- resumo das mudanças;
- comportamento anterior e novo, quando aplicável;
- testes executados e resultados;
- limitações conhecidas;
- riscos de compatibilidade ou segurança;
- documentação atualizada.

Mantenha o diff focado. Não inclua binários gerados, arquivos temporários, segredos ou dados locais.

## 7. Commits

Prefira mensagens curtas e descritivas, por exemplo:

- `feat: add metadata display`
- `fix: preserve errno on cleanup`
- `docs: clarify path resolution guarantees`
- `test: cover symlink destination handling`

O formato é uma convenção de contribuição; não representa uma validação automática obrigatória no repositório.
