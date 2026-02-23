# Changelog

Todas as mudanças notáveis neste projeto serão documentadas aqui.

Formato baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/),
seguindo [Semantic Versioning](https://semver.org/lang/pt-BR/).

---

## [0.3.6] — 2026-02-23

### Adicionado
- `python3` movido para dependências **obrigatórias** — é usado na geração de HTML e na coleta de usuários; ausência causaria falha silenciosa

### Documentação
- README: geração HTML descrita como Python3 (não mais sed); `nvme-cli` adicionado aos recomendados e ao `dnf install`
- agents.md: `python3` adicionado às obrigatórias; `pandoc`/`ollama` removidos da tabela; `run_cmd` documentado como script temporário (não mais `eval`); `nvme-cli` adicionado à tabela de recomendadas

## [0.3.5] — 2026-02-23

### Corrigido
- `USER/passwd` — código Python multilinhas agora escrito em arquivo temporário `.py` antes de chamar `run_cmd`; quebras de linha não sobreviviam ao `printf` interno do `run_cmd`

## [0.3.4] — 2026-02-23

### Corrigido
- Arquivos de saída (`.md`, `.html`, `.log`) agora recebem `chown` para o usuário real (`$SUDO_USER`) ao final — não ficam mais presos como root
- `USER/passwd` — substituído `awk` (problema persistente de quoting no script temporário) por `python3` inline, que já está disponível no sistema

## [0.3.3] — 2026-02-23

### Corrigido
- `USER/passwd` e `CPU/flags` — removido `bash -c` redundante; como `run_cmd` já executa via script temporário, o `bash -c` aninhado impedia as aspas de chegarem corretamente ao awk e ao grep

## [0.3.2] — 2026-02-23

### Corrigido
- `run_cmd` reescrito para usar arquivo de script temporário — elimina dupla expansão de shell que corrompia aspas em `awk`, pipes e estruturas complexas
- `xrandr` — lógica `if/then` substituída por `test &&/||` evitando exit 2 espúrio no Wayland

## [0.3.1] — 2026-02-23

### Corrigido
- `awk` na listagem de usuários do sistema — erro de quoting resolvido
- `CPU/flags` — pipe fora do `bash -c` era interpretado pelo shell externo
- `xrandr` no Wayland — saída de erro suprimida corretamente quando `$DISPLAY` ausente
- `blkid` em filesystems virtuais (`devpts`, `bpf`, `selinuxfs`, `fuse.portal`) — adicionados ao filtro de exclusão
- `systemd-detect-virt` — exit 1 é resultado válido (bare metal), não mais registrado como erro
- `generate_html` — substituído `sed` frágil por gerador Python3 com escape correto de HTML
- `CPU/governor` — glob `cpu*` substituído por leitura de `cpu0` com verificação de existência
- Timeout de 30s aplicado a todos os comandos via `run_cmd`
- Seções lentas removidas: `du` no home, `find` de extensões, `lsof +D $HOME`

## [0.3.0] — 2026-02-23

### Adicionado
- Publicação inicial no GitHub
- `README.md`, `CHANGELOG.md`, `agents.md`, `LICENSE`, `.gitignore`
- Timeout global de 30s por comando
- Geração HTML nativa (removido pandoc; geração via sed — substituída por Python3 em v0.3.1)

### Removido
- `ollama` e `pandoc` das dependências opcionais
- Seção "Ollama — Modelos de IA Instalados"

### Renomeado
- Script renomeado de `mycomp_gen.sh` para `my_comp.sh`

---

[0.3.6]: https://github.com/petrinhu/my_comp/compare/v0.3.5...v0.3.6
[0.3.5]: https://github.com/petrinhu/my_comp/compare/v0.3.4...v0.3.5
[0.3.4]: https://github.com/petrinhu/my_comp/compare/v0.3.3...v0.3.4
[0.3.3]: https://github.com/petrinhu/my_comp/compare/v0.3.2...v0.3.3
[0.3.2]: https://github.com/petrinhu/my_comp/compare/v0.3.1...v0.3.2
[0.3.1]: https://github.com/petrinhu/my_comp/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/petrinhu/my_comp/releases/tag/v0.3.0
