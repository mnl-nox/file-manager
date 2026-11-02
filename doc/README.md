# Documentação técnica

<div align="center">

**FILE MANAGER / DOCUMENTATION**

*Especificação, arquitetura, segurança e processo de desenvolvimento*

</div>

| Campo | Valor |
|---|---|
| Projeto | File Manager |
| Linguagem | C11 |
| Plataforma-alvo | Linux |
| Interfaces de sistema | POSIX e extensões Linux documentadas |
| Licença | MIT |
| Estado documental | Revisão técnica contínua |

Esta documentação descreve o comportamento implementado no repositório. Quando uma seção apresenta uma proposta ou uma limitação, isso é indicado explicitamente; não se deve interpretar uma decisão futura como funcionalidade já implementada.

## Índice

### Fundamentos do projeto
- [Visão geral e escopo](visao-geral.md) — objetivos, público, escopo e não objetivos.
- [Requisitos de software](requisitos.md) — requisitos funcionais, não funcionais e critérios de verificação.
- [Glossário](glossario.md) — termos técnicos e convenções utilizadas.

### Projeto e implementação
- [Arquitetura](arquitetura.md) — componentes, dependências, fluxo e ownership.
- [Segurança](seguranca.md) — modelo de ameaças, resolução de caminhos, operações e limitações.
- [Decisões arquiteturais](decisoes/README.md) — ADRs e justificativas registradas.

### Engenharia e manutenção
- [Referência de build e testes](desenvolvimento.md) — ambiente, comandos e validação local.
- [Site e publicação](site-e-deploy.md) — GitHub Pages e CI/CD.
- [Registro de alterações](alteracoes.md) — histórico resumido das mudanças.

### Fontes
- [Referências técnicas e acadêmicas](referencias.md) — normas, manuais e bibliografia.

## Convenções documentais

- **Implementado:** comportamento presente no código da branch de referência.
- **Parcial:** existe implementação, mas com restrições explicitadas.
- **Planejado:** proposta ainda não incorporada ao código.
- **Não suportado:** comportamento que o programa deliberadamente não oferece.

Os caminhos de arquivos são relativos à raiz do repositório. As referências a syscalls e interfaces devem ser lidas em conjunto com as versões de kernel e libc utilizadas no ambiente de execução.

## Escopo de validade

A documentação evolui junto com o código. Em caso de divergência, a implementação e os testes automatizados definem o comportamento executável atual; a divergência deve ser reportada e corrigida por meio de uma alteração documental ou de código revisável.
