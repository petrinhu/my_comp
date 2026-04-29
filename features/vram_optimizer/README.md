# vram_optimizer

> Auditor de uso de VRAM/GPU em Linux. Detecta processos consumindo VRAM, classifica candidatos a CPU-only (ex.: navegadores) e localiza a configuração que fez cada app subir para a GPU.

> *Linux VRAM/GPU usage auditor. Detects processes using VRAM, classifies CPU-only candidates (e.g. browsers), and pinpoints the configuration that pushed each app to the GPU.*

---

## Status / Status

Projeto em fase de **bootstrap**. Ainda não há código, build ou release.

*Project in **bootstrap** phase. No code, build, or release yet.*

## Objetivo / Goal

Em sistemas Linux com GPU única ou compartilhada, aplicações "leves" (navegadores, editores, chats) com aceleração de hardware ligada por padrão ocupam VRAM que jogos, modelos LLM locais e cargas de render/inferência precisariam. O `vram_optimizer`:

1. Detecta processos usando VRAM agora e quanto.
2. Classifica quais são candidatos a "rodar só em CPU".
3. **Localiza a origem da configuração** que disparou o uso de GPU (flag de navegador, `__NV_PRIME_RENDER_OFFLOAD`, `.desktop` com wrapper, config de compositor, etc.).
4. Sugere onde alterar para liberar VRAM.

*On Linux systems with a single or shared GPU, lightweight apps (browsers, editors, chats) with hardware acceleration on by default consume VRAM that games, local LLMs, and render/inference workloads need. `vram_optimizer` detects, classifies, locates the configuration source, and suggests fixes.*

## Stack

- C++23 (GCC 13+ / Clang 16+)
- Qt 6.x
- CMake ≥ 3.21
- MySQL (condicional / *optional*)
- Linux only no MVP — NVIDIA + Intel; AMD e outras via extensão

## Interfaces

- `vram_optimizer_gui` — GUI Qt 6 (Widgets)
- `vram_optimizer_cli` — CLI headless com saída tabular ou JSON

Ambas compartilham as mesmas libs internas (Application + Domain) — zero duplicação de regra de negócio.

## Documentação / Documentation

| Documento | Conteúdo / Content |
| :--- | :--- |
| [CLAUDE.md](CLAUDE.md) | Instruções do projeto para agentes de IA / *Project instructions for AI agents* |
| [docs/CONTRACT.md](docs/CONTRACT.md) | Contrato técnico RFC 2119 (autoridade) / *Technical contract* |
| [docs/TESTES.md](docs/TESTES.md) | Procedimentos de teste (T1–T12) e auditoria (A1–A10) |
| [docs/cpp_qt6_indice_mestre.md](docs/cpp_qt6_indice_mestre.md) | Índice das 8 auditorias técnicas detalhadas |
| [docs/cpp_qt6_auditoria_seguranca.md](docs/cpp_qt6_auditoria_seguranca.md) | Memory safety, SQL injection, hardening, LGPD |
| [docs/cpp_qt6_auditoria_arquitetura.md](docs/cpp_qt6_auditoria_arquitetura.md) | 4 camadas, SOLID, DRY, TDD |
| [docs/cpp_qt6_auditoria_cpp23_moderno.md](docs/cpp_qt6_auditoria_cpp23_moderno.md) | Smart pointers, RAII, ranges, concorrência |
| [docs/cpp_qt6_auditoria_qt6_especifico.md](docs/cpp_qt6_auditoria_qt6_especifico.md) | Widgets, Model/View, QML, signals/slots, i18n |
| [docs/cpp_qt6_auditoria_mysql.md](docs/cpp_qt6_auditoria_mysql.md) | Schema, queries, transações, migrations, backup |
| [docs/cpp_qt6_auditoria_performance.md](docs/cpp_qt6_auditoria_performance.md) | Event loop, N+1, profiling, observabilidade |
| [docs/cpp_qt6_auditoria_devops_build.md](docs/cpp_qt6_auditoria_devops_build.md) | CMake, CI/CD, empacotamento, versionamento |
| [docs/cpp_qt6_auditoria_qualidade_codigo.md](docs/cpp_qt6_auditoria_qualidade_codigo.md) | Code smells, complexidade, nomenclatura |

## Arquitetura / Architecture

Arquitetura em quatro camadas com dependência unidirecional:

```
Presentation     Qt6 Widgets / QML / CLI
        │
        ▼
Application      Services / Use Cases
        │
        ▼
Domain           Entidades / Interfaces puras / Value Objects
        ▲
        │
Infrastructure   NVML / sysfs / MySQL / parsers de config
                 (implementa interfaces do Domain)
```

Detalhes em `docs/cpp_qt6_auditoria_arquitetura.md`.

## Licença / License

A definir. */ TBD.*
