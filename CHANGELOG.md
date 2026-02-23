# Changelog

Todas as mudanças notáveis neste projeto serão documentadas aqui.

Formato baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/),
seguindo [Semantic Versioning](https://semver.org/lang/pt-BR/).

---

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
- Geração HTML nativa via sed (removido pandoc)

### Removido
- `ollama` e `pandoc` das dependências opcionais
- Seção "Ollama — Modelos de IA Instalados"

### Renomeado
- Script renomeado de `mycomp_gen.sh` para `my_comp.sh`

---

[0.3.2]: https://github.com/petrinhu/my_comp/compare/v0.3.1...v0.3.2
[0.3.1]: https://github.com/petrinhu/my_comp/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/petrinhu/my_comp/releases/tag/v0.3.0
