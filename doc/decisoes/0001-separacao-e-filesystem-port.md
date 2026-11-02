# ADR-0001: separação por responsabilidades e port de filesystem

- **Estado:** Aceita
- **Contexto:** branch principal após a integração do port.
- **Decisão:** a aplicação consome um contrato `FileSystemPort`, e a composição injeta uma implementação POSIX.
- **Data de registro:** 2026-11

## Contexto

A interface de terminal, o parsing de comandos, a validação dos casos de uso e as operações concretas de filesystem possuem motivos diferentes para mudar. Misturar essas responsabilidades dificulta testes isolados e aumenta o acoplamento.

## Decisão

Manter componentes separados para UI, parser e aplicação. Definir `FileSystemPort` como tabela de funções que representa as operações requeridas pela aplicação. O adapter POSIX fornece os ponteiros para as funções concretas.

## Alternativas consideradas

1. **Syscalls diretamente na UI:** rejeitada por acoplar interação com sistema operacional.
2. **Aplicação dependente diretamente de funções globais POSIX:** rejeitada por dificultar substituição do backend nos testes.
3. **Port injetável:** adotada por tornar explícita a fronteira de dependência e permitir adapters alternativos.

## Consequências

- A aplicação pode ser testada com uma implementação substituta do port.
- A composição das dependências fica explícita.
- O adapter atual é fino: a implementação das operações e syscalls ainda está em `src/file_manager.c`.
- A separação não deve ser descrita como isolamento completo do backend; a extração do core POSIX é uma evolução possível.
