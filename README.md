# my_comp v2.0 Alpha

Aplicação nativa de monitoramento e diagnóstico de sistemas Linux — evolução do script Bash legado (`legacy/my_comp v0.3.6`) para uma plataforma C++20/Qt6 de alta performance, com foco em **Fedora 43 + KDE Plasma 6 + Wayland** e filosofia **Zero-Cloud**.

## Destaques

- **Dashboard real-time** — CPU (load total + por core + temperatura), memória, armazenamento, rede, áudio (waveform + espectro FFT).
- **Hardware & firmware** — SMBIOS/DMI, topologia USB/Bluetooth, GPU (NVIDIA + Sysfs), firmware via D-Bus.
- **Atualizações de pacotes** — detecção multi-PM em runtime: DNF, APT, Pacman, Flatpak, Snap, NPM, Cargo.
- **Journal unificado** — filtros Hardware/Firmware, Kernel e Ambiente com busca online por *double-click*.
- **Serviços systemd** — listagem, controle (start/stop/restart) via D-Bus e criação de daemons user.
- **Segurança** — cofre AES-256-GCM (OpenSSL) via `ISecurityVault`, sem telemetria, 100% offline.
- **Exportação** — relatórios JSON, Markdown e HTML.

## Stack

- C++20 estrito · Qt6 (Core, Gui, Quick, QuickControls2, WaylandClient, Multimedia, DBus, Network, Concurrent, Test)
- CMake ≥ 3.16 + Ninja
- GoogleTest + CTest (unit + acceptance)
- OpenSSL (cripto)

## Arquitetura

Hexagonal em 3 camadas com dependência em sentido único:

```
Presentation (ViewModels + QML)  →  Domain (interfaces + modelos)  ←  Infrastructure (providers Linux, D-Bus, OpenSSL, QtAudio, exporters)
```

Detalhes completos em [`CLAUDE.md`](./CLAUDE.md).

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

## Licença

Distribuído sob **PolyForm Noncommercial License 1.0.0** — ver [`LICENSE`](./LICENSE).

## Repositório

`ssh://git@codeberg.org/petrinhu/my_comp.git`
