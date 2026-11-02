# Site e publicação contínua

## 1. Implementação

O site é uma apresentação estática independente do executável C. É composto por HTML semântico e CSS responsivo, sem framework obrigatório. Os arquivos-fonte ficam em `site/`.

## 2. CI

O workflow `.github/workflows/ci.yml` executa:
- build com GCC e Clang;
- testes automatizados;
- ASan e UBSan;
- validação estrutural mínima do HTML/CSS.

A validação estrutural não equivale a uma auditoria completa de acessibilidade, HTML ou segurança web.

## 3. CD

O workflow `.github/workflows/deploy-site.yml` é acionado:
- em push para `main` quando `site/**` ou o próprio workflow de deploy muda;
- manualmente por `workflow_dispatch`.

O artefato publicado é exclusivamente o conteúdo de `site/`.

## 4. Configuração necessária no GitHub

No repositório:
1. Acesse **Settings → Pages**.
2. Defina **Build and deployment → Source** como **GitHub Actions**.
3. Verifique se o workflow possui as permissões `contents: read`, `pages: write` e `id-token: write`.
4. Consulte **Actions → Deploy website** para verificar o estado e a URL do deployment.

URL esperada: <https://mnl-nox.github.io/file-manager/>.

A configuração do workflow não comprova, por si só, que o domínio esteja publicado ou que DNS/Pages estejam corretamente habilitados. A confirmação deve ser feita no resultado do deployment e no acesso HTTP ao endereço.

## 5. Atualização

1. Edite os arquivos de `site/`.
2. Verifique localmente HTML, CSS e responsividade.
3. Abra Pull Request.
4. Aguarde o CI.
5. Faça merge em `main`.
6. Confirme a execução do workflow de deploy.

Alterações exclusivas em código C ou Markdown não iniciam publicação do site, a menos que o workflow de deploy também seja alterado.

## 6. Segurança operacional

O site é público. Não inclua tokens, credenciais, dados pessoais ou informações internas. Evite dependências externas desnecessárias e mantenha links e conteúdo revisados.
