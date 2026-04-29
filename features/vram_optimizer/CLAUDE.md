# CLAUDE.md — vram_optimizer

Instruções específicas deste projeto para qualquer agente de IA. Complementam (não substituem) o `~/.claude/CLAUDE.md` global do usuário. Em conflito, **a instrução do usuário sempre vence**.

## Domínio em uma linha

Auditor de uso de VRAM/GPU em Linux: detecta processos consumindo VRAM, classifica candidatos a CPU-only (ex.: navegadores) e **localiza a configuração** que fez cada app subir para a GPU (flag de aceleração, variável `__NV_PRIME_RENDER_OFFLOAD`/`DRI_PRIME`, `.desktop` com wrapper, config de compositor, etc.).

## Stack

- **Linguagem**: C++23 (GCC 13+ / Clang 16+)
- **Framework**: Qt 6.x (Widgets para GUI; QML opcional)
- **Build**: CMake ≥ 3.21 com presets, `qt_add_executable`, `qt_standard_project_setup()`
- **DB (condicional)**: MySQL atrás de interface DIP. Fase 1 pode usar SQLite ou arquivo
- **OS**: Linux apenas (Fedora 43 é o ambiente de dev primário)
- **GPUs no MVP**: NVIDIA (NVML) + Intel (sysfs i915 / Level Zero); AMD e outras como extensão via `IGpuBackend`
- **Interfaces**: GUI Qt (`vram_optimizer_gui`) + CLI (`vram_optimizer_cli`) compartilhando o mesmo Application/Domain

## Autoridade técnica

Antes de codar, consultar nesta ordem de prioridade:

1. **`docs/CONTRACT.md`** — autoridade técnica do projeto (RFC 2119: MUST / SHOULD / MAY). Cobre OOP, SOLID, design patterns, 4 camadas, clean code, segurança, performance, git, testes, regras por linguagem/framework. Em conflito com qualquer outro doc, CONTRACT vence.
2. **`docs/TESTES.md`** — procedimentos T1–T12 (testes) e A1–A10 (auditoria) com comandos exatos.
3. **`docs/cpp_qt6_*.md`** — 9 documentos detalhados por bloco (segurança, arquitetura, C++23 moderno, Qt6, MySQL, performance, devops, qualidade) + índice mestre. Ver `README.md` para mapa.

Os docs foram **reaproveitados de outro projeto (PokemonTCGViewer)**; são genéricos para qualquer projeto C++23/Qt6/MySQL. Referências literais a `Carta`/`CartaPokemon`/`RepositorioCartas` devem ser ignoradas — manter apenas o **padrão estrutural** e adaptar ao domínio do `vram_optimizer` (`Processo`, `IGpuBackend`, `ServicoAuditarVram`).

## Arquitetura em 4 camadas (vocabulário oficial)

```
Presentation     Qt6 Widgets/QML, CLI argv  →  Application
Application      Services / Use Cases         →  Domain
Domain           Entidades, Interfaces puras  ←  Infrastructure (implementa)
Infrastructure   NVML, sysfs, MySQL/SQLite, parsers de config
```

- **Domain é puro**: zero `#include <Q...>` exceto tipos de valor (`QString`, `QVector`, `QDateTime`).
- **DI por construtor**: Services recebem `std::shared_ptr<IInterface>`. Nunca instanciam concretos.
- **GUI e CLI** são paralelas em `presentation/` e compartilham 100% das libs `application/` e `domain/`. Zero duplicação de regra de negócio.

Diretórios canônicos:
- `src/presentation/` — `views/`, `viewmodels/`, `delegates/`, `cli/`
- `src/application/` — `services/`, `dto/`
- `src/domain/` — `entities/`, `interfaces/`, `valueobjects/`
- `src/infrastructure/` — `gpu/`, `config/`, `repositories/`, `database/`

## Convenções rápidas (consolidadas do CONTRACT)

- **pt-br** em UI, logs visíveis, comentários, mensagens de commit. Acentuação completa sempre — nunca substituir `não`/`função`/`índice` pelo equivalente ASCII.
- **i18n**: toda string visível ao usuário em `tr()`. `QLocale::setDefault(QLocale::Portuguese, QLocale::Brazil)`. Timezone `America/Sao_Paulo`.
- **Charset**: utf8mb4 em servidor, banco, tabelas, conexão Qt.
- **Memory safety**: `std::unique_ptr` default; `std::shared_ptr` só com ownership compartilhado real. Sem `new`/`delete` raw em código de aplicação. Sem `NULL` (usar `nullptr`). Sem C-cast (`(int)x` → `static_cast<int>(x)`).
- **Qt connect**: ponteiro de membro tipado; nunca SIGNAL/SLOT macro. Lambda em connect SEMPRE com 3º argumento de contexto. `QPointer<T>` para observação de QObjects que podem ser destruídos.
- **SQL**: `QSqlQuery::prepare()` + `bindValue()` em 100% das queries. Concatenação de QString = falha de auditoria.
- **UI thread**: nunca bloqueia. I/O, rede e SQL fora da thread principal (`QtConcurrent`, `QThread::moveToThread`). `Qt::QueuedConnection` para signals cross-thread.
- **Theme**: **Breeze/KDE como default**, estilos embarcados em `.qrc` (sem dependência de KDE Plasma, `kf5`/`kf6` ou bibliotecas de tema do sistema). QSS centralizado em `:/temas/breeze/breeze.qss`. Aplicar paleta + QSS antes de criar qualquer widget. Sem `setStyleSheet` com cores hardcoded em widget individual. Detalhes em `docs/CONTRACT.md` §7.6.
- **Nomenclatura de identificadores**: classes PascalCase (`GerenciadorVram`); métodos camelCase; membros `m_` prefix; signals em passado (`vramAuditada`); slots em presente (`onVramAuditada`).
- **Nomenclatura de arquivos**: snake_case (preferência global do usuário) — ex.: `gerenciador_vram.cpp`, `gerenciador_vram.h`.
- **Conventional Commits**: `feat:`, `fix:`, `refactor:`, `chore:`, `docs:`, `test:`, `perf:`, `style:`, `revert:`. Em pt-br ou inglês conforme histórico do repo.
- **Nunca** commitar credenciais, tokens ou PII. Nunca logar dados pessoais (CPF, e-mail, senha, token).
- **DRY (regra dos 3)**: 1ª ocorrência escreve, 2ª tolera duplicação, 3ª extrai. Distinguir duplicação real (mesma razão de mudar) de coincidência (parecem similares hoje, divergem amanhã). Detalhes em `docs/CONTRACT.md` §3.6 e na memória `project_dry_principio.md`.

## Limites de qualidade (CI bloqueia se ultrapassar)

| Item | Limite |
| :--- | :--- |
| Linhas por função | ≤ 40 |
| Parâmetros por função | ≤ 4 |
| Aninhamento (if/loop) | ≤ 3 níveis |
| Linhas por classe | ≤ 300 |
| Profundidade de herança | ≤ 3 |
| Complexidade ciclomática | ≤ 15 |
| Cobertura mínima global | ≥ 60% (Domain ≥ 90%, Application ≥ 80%) |

Build estrito obrigatório: `-std=c++23 -Wall -Wextra -Wpedantic -Werror -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -D_FORTIFY_SOURCE=3 -fstack-protector-strong -fPIE` + linker `-pie -Wl,-z,relro -Wl,-z,now`.

## TDD obrigatório (Domain + Application)

Red → Green → Refactor. Bug fix começa por teste vermelho que reproduz. Cada método público de Service tem teste unitário. GTest+GMock para Domain/Application; QTest+QSignalSpy para classes Qt. Repositories testados com SQLite in-memory ou MySQL de teste.

## Pendências

A tabela de pendências e planejamento do projeto está em `TODO.md` na raiz. Atualizar via skill `/tab_pendencias` (`--show` para ver todas, `--main` para ativas, ou pedir adição de item específico). Nunca criar `PENDENCIAS.md` ou `TAREFAS.md` em paralelo.

## Skills a usar proativamente

- **`/tab_pendencias`** — único arquivo de pendências autorizado é `TODO.md` na raiz, formato da skill. NUNCA criar `PENDENCIAS.md`/`TAREFAS.md`.
- **`/memo_persistente`** — antes de `/clear`, troca Opus↔Sonnet, ou pausa longa.

## Anti-patterns específicos do projeto

- Acoplar lógica de detecção de GPU à GUI (deve estar em Application chamando `IGpuBackend`).
- Hardcoding de "navegador é candidato a CPU" em `if-else` — deve ser configurável (catálogo de heurísticas em data, não em código).
- Misturar NVIDIA-specific e Intel-specific no mesmo arquivo Infrastructure — separar em backends por fabricante.
- Ler `/proc/<pid>/environ` de outros usuários sem fallback gracioso para usuários sem privilégio.
- Tratar MySQL como obrigatório no MVP — está marcado "se necessário"; deve ficar atrás de interface no Domain.
- Criar arquivos `.md` de planejamento/análise intermediária sem pedido explícito do usuário.
