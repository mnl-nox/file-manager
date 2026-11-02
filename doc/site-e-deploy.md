# Site e publicação contínua

## Stack

O site é estático e independente do binário C: HTML semântico, CSS responsivo sem framework e GitHub Pages. A fonte fica em `site/`; o workflow publica somente esse diretório.

## Fluxo CI/CD

1. Pull requests para `main` executam build GCC/Clang, testes, sanitizers e validação estrutural do site.
2. Push em `main` que altere `site/**` ou o workflow de deploy inicia a publicação.
3. O workflow configura Pages, empacota `site/` e publica o artefato.
4. URL esperada: https://mnl-nox.github.io/file-manager/.

Alterações exclusivas em C ou na documentação Markdown não disparam deploy. Atualize os arquivos em `site/` para publicar uma nova versão do site.

## Configuração do GitHub

O workflow usa `contents: read`, `pages: write` e `id-token: write`. Se a primeira execução indicar que Pages não está habilitado, abra Settings → Pages e selecione **GitHub Actions** como fonte de publicação. Não são necessários tokens pessoais ou secrets customizados.

## Atualização

Edite `site/index.html` e `site/styles.css`, envie um pull request e aguarde o CI. Após o merge em `main`, o deploy é automático. Consulte Actions → Deploy website para o resultado e a URL publicada.

O conteúdo do site é público. Não inclua segredos, tokens ou dados privados.
