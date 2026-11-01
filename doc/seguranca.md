# Segurança e semântica de filesystem

## Modelo de ameaça

O File Manager é uma ferramenta local executada com as credenciais do usuário. Não eleva privilégios, não cria sandbox e não tenta contornar permissões do kernel. O usuário deve considerar qualquer caminho informado como entrada não confiável.

## Links simbólicos

- A listagem usa `lstat` e apresenta links como `link`.
- A consulta de metadados usa `lstat`, descrevendo o próprio link.
- A criação de arquivo usa `O_EXCL | O_NOFOLLOW`.
- A cópia rejeita origem que não seja arquivo regular e abre a origem com `O_NOFOLLOW`.
- A remoção de arquivo usa `lstat` seguido de `unlink`, que remove o link final sem percorrer seu destino.
- `cd` segue links de diretório, comportamento esperado para navegação.

## Operações destrutivas

| Operação | Regra |
|---|---|
| touch | Falha se destino já existir |
| mkdir | Falha se destino já existir |
| cp | Somente arquivo regular; destino deve ser novo |
| mv | Verifica destino antes de renomear; não substitui intencionalmente |
| rm | Arquivo ou link; exige confirmação na UI |
| rmdir | Diretório vazio; exige confirmação na UI |

Não há remoção recursiva. Para remover árvores, use uma ferramenta especializada depois de inspecionar o alvo.

## Concorrência e TOCTOU

Algumas operações são compostas por verificações e syscalls subsequentes usando caminhos absolutos. Em um ambiente no qual outro processo malicioso possa renomear ou substituir diretórios ancestrais simultaneamente, existe risco de corrida TOCTOU. No Linux, `mv` usa `renameat2(RENAME_NOREPLACE)` quando o kernel oferece suporte; o fallback POSIX usa verificação seguida de `rename` e não é atômico contra escritores concorrentes.

Uma versão endurecida para esse modelo deve operar com descritores de diretório (`openat`, `fstatat`, `unlinkat`, `renameat2(RENAME_NOREPLACE)` quando disponível), manter o dirfd da raiz lógica e definir explicitamente o comportamento em `EXDEV`. Esta entrega não deve ser apresentada como sandbox ou como ferramenta resistente a adversário local concorrente.

## Cópia e integridade

A cópia trata leituras interrompidas por `EINTR`, escritas parciais e falhas de fechamento. Se ocorrer erro, tenta remover o destino parcial. Não promete durabilidade após queda de energia: não chama `fsync` no arquivo e no diretório.

## Recomendações operacionais

1. Execute como usuário sem privilégios administrativos.
2. Faça backup antes de operações em massa.
3. Inspecione o caminho antes de confirmar `rm` ou `rmdir`.
4. Não execute em diretórios controlados por processos não confiáveis se a resistência a TOCTOU for requisito.
5. Revise o código e rode `make test-asan` antes de distribuir binários.
