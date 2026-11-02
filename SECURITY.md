# Política de segurança

## Escopo

Esta política cobre o código-fonte e os workflows mantidos neste repositório. O File Manager é um utilitário de terminal em C11 para Linux e não deve ser tratado como sandbox, mecanismo de confinamento ou ferramenta de administração privilegiada.

## Versões

Enquanto o projeto não publicar uma política formal de versões suportadas, a referência para correções é a branch `main`. Não há compromisso de suporte de segurança para releases ou forks não mantidos pelos contribuidores do repositório.

## Como reportar uma vulnerabilidade

**Não publique vulnerabilidades exploráveis, provas de conceito com dados reais ou detalhes de exploração em uma issue pública.**

Utilize o recurso **Report a vulnerability** na aba **Security** do repositório, quando disponível, para iniciar uma comunicação privada por GitHub Security Advisories. Inclua:
- versão ou commit afetado;
- plataforma, kernel e compilador;
- passos mínimos para reproduzir;
- impacto observado e pré-condições;
- mitigação temporária, se conhecida.

Se o recurso privado não estiver habilitado, consulte os canais de contato apresentados no perfil dos mantenedores do repositório. Não presuma que uma issue pública seja um canal confidencial.

## Expectativa de resposta

O projeto é mantido em regime open source e não estabelece SLA de resposta ou prazo garantido para correções. Os mantenedores devem avaliar a reprodução, impacto, versões afetadas e possibilidade de divulgação coordenada.

## Orientações de uso

- Execute com privilégios mínimos.
- Faça backup antes de operações destrutivas.
- Não utilize o programa para isolar workloads não confiáveis.
- Revise a [análise de segurança](doc/seguranca.md), especialmente os limites de `openat2`, symlinks e TOCTOU.
