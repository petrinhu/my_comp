# my_comp v2.0 *(codename Alpha)*

[![CI](https://github.com/petrinhu/my_comp/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/petrinhu/my_comp/actions/workflows/ci.yml)
[![Versão](https://img.shields.io/github/v/tag/petrinhu/my_comp?include_prereleases&sort=semver&label=vers%C3%A3o)](CHANGELOG.md)
[![Licença: AGPL-3.0](https://img.shields.io/github/license/petrinhu/my_comp?label=licen%C3%A7a)](LICENSE)
[![Último commit](https://img.shields.io/github/last-commit/petrinhu/my_comp?label=%C3%BAltimo%20commit)](https://github.com/petrinhu/my_comp/commits/main)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![Qt 6](https://img.shields.io/badge/Qt-6.4%2B-41CD52?logo=qt&logoColor=white)](https://doc.qt.io/qt-6/)
[![CMake + Ninja](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-064F8C?logo=cmake&logoColor=white)](CMakeLists.txt)
[![Plataforma](https://img.shields.io/badge/plataforma-Linux%20%C2%B7%20Wayland-FCC624?logo=linux&logoColor=black)](#execução)
[![Testes: GoogleTest](https://img.shields.io/badge/testes-GoogleTest%20%2B%20CTest-4285F4?logo=google&logoColor=white)](#testes-e-gates-de-qualidade)
[![Conventional Commits](https://img.shields.io/badge/Conventional%20Commits-1.0.0-FE5196?logo=conventionalcommits&logoColor=white)](https://www.conventionalcommits.org/pt-br/v1.0.0/)
[![Zero-Cloud](https://img.shields.io/badge/privacidade-100%25%20offline-2EA44F)](GODS_LAWS.md#mc-10)

> Versão SemVer atual: ver [`VERSION`](./VERSION) e [`CHANGELOG.md`](./CHANGELOG.md).
> O `v2.0` no título é **codename** (segunda geração pós-bash legacy v0.3.6), independente do SemVer real, que hoje está em `v0.1.0-alpha.N` e cortará `v1.0.0-beta.0` quando o item `BT-04` do `TODO.md` ativar.

Aplicação nativa de monitoramento e diagnóstico de sistemas Linux: evolução do script Bash legado (`legacy/my_comp v0.3.6`) para uma plataforma C++23/Qt6, com foco em **Fedora 43 + KDE Plasma 6 + Wayland** e filosofia **Zero-Cloud**.

## Destaques

- **Dashboard em tempo real**: CPU (carga total, por core e temperatura), memória, armazenamento, rede, áudio (waveform + espectro FFT).
- **Hardware e firmware**: SMBIOS/DMI, topologia USB/Bluetooth, GPU (NVIDIA + sysfs), firmware via D-Bus.
- **Auditoria de VRAM**: uso de VRAM por processo (contextos compute **e** gráficos), classificação em cascata de 5 regras (feedback → catálogo → categoria `.desktop` → bibliotecas compute → comportamento) e localização das configs de origem (Chromium, Firefox, `.desktop`, `__NV_PRIME_RENDER_OFFLOAD`/`DRI_PRIME`). Também na CLI: `my_comp vram [--json]`.
- **Atualizações de pacotes**: detecção multi-PM em runtime (DNF, APT, Pacman, Flatpak, Snap, NPM, Cargo); instalação com elevação via polkit (`pkexec`) e validação do nome do pacote.
- **Journal unificado**: filtros Hardware/Firmware, Kernel e Ambiente.
- **Serviços systemd**: listagem, start/stop/restart e criação de daemons de usuário.
- **Segurança**: cofre AES-256-GCM (OpenSSL) via `ISecurityVault`; sem telemetria. A medição de latência (ping externo) é **opt-in** e vem desligada.
- **Exportação**: relatórios JSON, Markdown e HTML.

## Stack

- C++23 estrito · Qt 6.4+ (Core, Gui, Quick, QuickControls2, WaylandClient, Multimedia, DBus, Network, Concurrent, Test)
- CMake ≥ 3.16 + Ninja
- GoogleTest + CTest (unit + acceptance); pacote do sistema, com fallback por FetchContent fixado em commit
- OpenSSL (cripto)

## Arquitetura

4 camadas com dependência em sentido único, mais o composition root:

```
Presentation  →  Application  →  Domain  ←  Infrastructure
                         ↑ composição (DI manual): src/main.cpp + src/composition/
```

| Diretório | Conteúdo |
| :--- | :--- |
| `src/domain/` | Entidades e interfaces `IPascalCase` (ports). Sem Qt GUI, OpenSSL nem processo externo. |
| `src/application/` | Casos de uso e serviços (orquestração). |
| `src/infrastructure/` | Coletores Linux (`/proc`, `/sys`), D-Bus, OpenSSL, QtAudio, NVML, exportadores. |
| `src/presentation/` | ViewModels (`Q_OBJECT`), CLI e QML (`Main.qml` + `views/` + `components/` + `Theme` singleton). |
| `src/composition/` | Um módulo `compor_<feature>()` por feature; único lugar que instancia infraestrutura. |

Regras completas: [`GODS_LAWS.md`](./GODS_LAWS.md) (leis MC-01 a MC-13) e [`CLAUDE.md`](./CLAUDE.md).

## Documentação

- [`GODS_LAWS.md`](GODS_LAWS.md): leis do projeto (camadas, nomes, memória, Qt, TDD, gates, release, segurança).
- [`docs/CONTRACT.md`](docs/CONTRACT.md): princípios universais (OOP, SOLID, Clean Code, segurança) em RFC 2119.
- [`docs/TESTES.md`](docs/TESTES.md): procedimentos T1–T12 (testes) e A1–A10 (auditoria) com comandos exatos.
- [`docs/AUDITORIAS.md`](docs/AUDITORIAS.md): checklists operacionais por bloco.
- [`docs/claude/release.md`](docs/claude/release.md): ferramental de release (git-cliff, hooks, `deploy.sh`).
- [`docs/wiki/`](docs/wiki/): fonte das páginas da wiki do GitHub.

## Build

```bash
cmake -B build -G Ninja
cmake --build build -j$(nproc)
```

Dependências no Fedora:

```bash
sudo dnf install cmake ninja-build gcc-c++ qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtmultimedia-devel qt6-qtwayland-devel openssl-devel gtest-devel gmock-devel
```

## Testes e gates de qualidade

```bash
cd build && ctest --output-on-failure
```

Pré-CI local com todos os gates automatizáveis (build, ctest, clang-tidy, cppcheck, valgrind):

```bash
./scripts/ci.sh
```

Gates obrigatórios para a Beta na [MC-07](GODS_LAWS.md#mc-07). Cobertura é medida, sem meta numérica. Testes que dependem do sistema real (bateria, podman, lsusb) ficam em `acceptance` e são pulados quando o recurso não existe.

## Execução

```bash
./build/my_comp              # GUI (Qt Quick)
./build/my_comp vram --json  # auditoria de VRAM na linha de comando
```

Usa Wayland quando há sessão Wayland (`WAYLAND_DISPLAY`); em X11 o Qt escolhe `xcb`. `QT_QPA_PLATFORM` explícito sempre prevalece.

## Planejamento e pendências

Tabela viva com itens funcionais, gates de qualidade e roadmap Beta em [`TODO.md`](./TODO.md).

## Release e versionamento

[SemVer 2.0](https://semver.org/spec/v2.0.0.html) com pré-release, bump automático pelos commits Conventional via [git-cliff](https://git-cliff.org) (regras na [MC-12](GODS_LAWS.md#mc-12)). O `CHANGELOG.md` é gerado a cada release.

```bash
./deploy.sh                              # releaseia commits já criados e pusha
./deploy.sh "feat(x): mensagem"          # commita pendências + releaseia + pusha
./scripts/release.sh --dry-run           # preview da próxima versão sem alterar nada
```

`git push` direto é bloqueado por hook (`scripts/hooks/pre-push`). Setup em clone novo:

```bash
git config core.hooksPath scripts/hooks
cargo install git-cliff   # ou: pip install git-cliff / dnf install git-cliff
```

Detalhes em [`docs/claude/release.md`](docs/claude/release.md).

## Licença

Distribuído sob **GNU Affero General Public License v3.0 (AGPL-3.0)**: ver [`LICENSE`](./LICENSE).

## Repositório

[github.com/petrinhu/my_comp](https://github.com/petrinhu/my_comp) · `git@github.com:petrinhu/my_comp.git`
