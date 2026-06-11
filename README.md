# my_comp v2.0 *(codename Alpha)*

> Versão SemVer atual: ver [`VERSION`](./VERSION) e [`CHANGELOG.md`](./CHANGELOG.md).
> O `v2.0` no título é **codename** (segunda geração pós-bash legacy v0.3.6) — independente do SemVer real, que hoje está em `v0.1.x-alpha` e cortará `v1.0.0-beta.0` quando o item `BT-04` do `TODO.md` ativar.

Aplicação nativa de monitoramento e diagnóstico de sistemas Linux — evolução do script Bash legado (`legacy/my_comp v0.3.6`) para uma plataforma C++23/Qt6 de alta performance, com foco em **Fedora 43 + KDE Plasma 6 + Wayland** e filosofia **Zero-Cloud**.

## Destaques

- **Dashboard real-time** — CPU (load total + por core + temperatura), memória, armazenamento, rede, áudio (waveform + espectro FFT).
- **Hardware & firmware** — SMBIOS/DMI, topologia USB/Bluetooth, GPU (NVIDIA + Sysfs), firmware via D-Bus.
- **VRAM/GPU optimizer** — auditor de uso de VRAM por processo + classificação CPU-only em cascata (5 regras) + localização de configs de origem (Chromium, Firefox, .desktop, env vars `__NV_PRIME_RENDER_OFFLOAD`/`DRI_PRIME`).
- **Atualizações de pacotes** — detecção multi-PM em runtime: DNF, APT, Pacman, Flatpak, Snap, NPM, Cargo.
- **Journal unificado** — filtros Hardware/Firmware, Kernel e Ambiente com busca online por *double-click*.
- **Serviços systemd** — listagem, controle (start/stop/restart) via D-Bus e criação de daemons user.
- **Segurança** — cofre AES-256-GCM (OpenSSL) via `ISecurityVault`, sem telemetria, 100% offline.
- **Exportação** — relatórios JSON, Markdown e HTML.

## Stack

- C++23 estrito · Qt6 (Core, Gui, Quick, QuickControls2, WaylandClient, Multimedia, DBus, Network, Concurrent, Test)
- CMake ≥ 3.16 + Ninja
- GoogleTest + CTest (unit + acceptance)
- OpenSSL (cripto)

## Arquitetura

4 camadas físicas com dependência em sentido único:

```
Presentation  →  Application  →  Domain  ←  Infrastructure
```

Detalhes completos em [`CLAUDE.md`](./CLAUDE.md).

## Documentação técnica

- [`docs/CONTRACT.md`](docs/CONTRACT.md) — princípios universais (OOP, SOLID, Clean Code, segurança) em RFC 2119.
- [`docs/TESTES.md`](docs/TESTES.md) — procedimentos T1–T12 (testes) e A1–A10 (auditoria) com comandos exatos.
- [`docs/AUDITORIAS.md`](docs/AUDITORIAS.md) — checklists operacionais por bloco (segurança, arquitetura, C++23, Qt6, performance, devops, qualidade).

## Build

```bash
cmake -B build -G Ninja
cmake --build build -j$(nproc)
```

## Testes

```bash
cd build && ctest --output-on-failure
```

Cobertura mínima esperada: **≥ 70 %** em `src/domain/` e serviços. Gates completos (clang-tidy, cppcheck, Valgrind, ASan, WCAG AA) em `CLAUDE.md` §7.

## Execução

```bash
./build/my_comp
```

Binário Qt Quick; requer sessão Wayland ativa.

## Planejamento e pendências

Tabela viva com todos os itens funcionais, gates de qualidade e roadmap Beta em [`TODO.md`](./TODO.md).

## Release & versionamento

Esquema **[SemVer 2.0](https://semver.org/spec/v2.0.0.html)** com pré-release. Bump calculado automaticamente pelos commits Conventional via **[git-cliff](https://git-cliff.org)**. `CHANGELOG.md` é gerado/atualizado em cada release (não editar manualmente acima do marcador).

Comando único para release + push (build + bump + CHANGELOG + tag + push com `--follow-tags`):

```bash
./deploy.sh                              # releaseia commits já criados e pusha
./deploy.sh "feat(x): mensagem"          # commita pendências + releaseia + pusha
./scripts/release.sh --dry-run           # preview da próxima versão sem alterar nada
```

`git push` direto é bloqueado por hook (`scripts/hooks/pre-push`) — força usar `deploy.sh` para garantir release semântica consistente. Setup inicial em clone novo:

```bash
git config core.hooksPath scripts/hooks
cargo install git-cliff   # ou: dnf install git-cliff (quando disponível no Fedora)
```

Detalhes completos em [`CLAUDE.md`](./CLAUDE.md) §11.

## Licença

Distribuído sob **GNU Affero General Public License v3.0 (AGPL-3.0)**: ver [`LICENSE`](./LICENSE).

## Repositório

`ssh://git@codeberg.org/petrinhu/my_comp.git`
