# my_comp v2.0 — Planejamento

**Última auditoria registrada:** 2026-04-19 (reabertura). Os 43 itens funcionais herdados da TABELA.md anterior entram como `🔍 Pendente verificação` + Estado Auditado `—` e só migram para `✅ Concluído` + `✓` depois que a Fase 2 (rebuild + ctest + clang-tidy + valgrind + smoke manual) confirmar cada um. Qualquer falha vira nova linha no grupo correspondente com `Pré-requisito` apontando para o ID original.

**Ordem da tabela:** do núcleo (fundação + infraestrutura) para a periferia (features visuais), terminando em QA e roadmap Beta. A auditoria percorre a tabela de cima para baixo, então falhas no núcleo são descobertas antes de se investir em sintomas na UI.

**Legenda rápida:**
- Status: ✅ Concluído · 🔄 Em andamento · 🟡 Parcial · ⏳ Pendente · 💡 Decisão tomada · 🎨 Pendente design · 🔍 Pendente verificação
- Estado Auditado: `—` (não auditado) · `✓` (aprovado) · `⚠` (com ressalvas)

| ID | Grupo | Descrição Técnica | Prioridade | Pré-requisito | Dificuldade | Status | Estado Auditado |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| AR-01 | Architecture | Camada Domain (Mid) — 20 interfaces + 19 modelos | Alta | — | Alta | ✅ Concluído | ✓ |
| AR-02 | Architecture | Camada Infrastructure (End) — 26 providers + 3 exporters | Alta | AR-01 | Alta | ✅ Concluído | ⚠ |
| AR-03 | Architecture | Inicialização progressiva assíncrona (sequenciador QML + thread-safety) | Alta | AR-02 | Alta | ✅ Concluído | ⚠ |
| AR-04 | Architecture | Separar use cases (`update_manager.h`, `diagnostic_manager.h`) de `domain/` para nova camada `application/` | Baixa | AR-01 | Média | ⏳ Pendente | — |
| AR-05 | Architecture | Auditoria de signals/slots Qt (disconnect-on-destroy, `Qt::QueuedConnection` cross-thread, múltiplos `connect` redundantes) | Alta | AR-03 | Média | ⏳ Pendente | — |
| AR-06 | Architecture | Auditoria de parent ownership `QObject` (objetos sem parent = leak; `QPointer` para observação) | Alta | — | Média | ⏳ Pendente | — |
| AR-07 | Architecture | Mover I/O dos construtores para método `initialize()` (CLAUDE.md §4) em `CpuViewModel`, `HardwareViewModel`, `PowerViewModel`, `GpuViewModel` | Média | AR-03 | Média | ⏳ Pendente | — |
| AR-08 | Architecture | Tornar `refresh()` assíncrono (`QtConcurrent::run`) em `NetworkViewModel`, `StorageViewModel`, `MemoryViewModel`, `HardwareViewModel` — hoje rodam síncronos na UI thread | Média | AR-03 | Média | ⏳ Pendente | — |
| AR-09 | Architecture | Substituir `Timer(300ms)` do sequenciador QML por cadeia de signals `readyChanged`/`dataLoaded` — elimina "mágica" de 300ms e paraleliza onde possível | Baixa | AR-03 | Média | ⏳ Pendente | — |
| IN-01 | Infra | Executor de shell genérico (`QProcessShellExecutor`) | Alta | — | Baixa | ✅ Concluído | ⚠ |
| IN-01a | Infra | Capturar `stderr` em `QProcessShellExecutor::execute` (hoje só captura stdout; erros do subprocesso ficam silenciosos) | Média | IN-01 | Baixa | ⏳ Pendente | — |
| IN-01b | Infra | Controlar `QProcessEnvironment` explícito (hoje herda env do pai, risco baixo mas vale anotar) | Baixa | IN-01 | Baixa | ⏳ Pendente | — |
| IN-02 | Infra | Provedor DNF (`DnfUpdateProvider`) | Alta | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-02a | Infra | `DnfUpdateProvider`: `current_version` nunca preenchido — consultar `rpm -q <pkg>` para enriquecer cada `UpdatePackage` | Média | IN-02 | Média | ⏳ Pendente | — |
| IN-02b | Infra | `DnfUpdateProvider`: `description` recebe nome do repositório (`updates`/`fedora`), não descrição real — renomear campo para `source_repo` ou buscar descrição via `dnf info` | Média | IN-02 | Baixa | ⏳ Pendente | — |
| IN-02c | Infra | `DnfUpdateProvider`: ampliar testes (DNF indisponível, output vazio, `is_available()`, heurística `is_critical`) | Média | IN-02 | Baixa | ⏳ Pendente | — |
| IN-03 | Infra | Provedor Flatpak | Média | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-03a | Infra | `FlatpakUpdateProvider`: `current_version` vazio — enriquecer com `flatpak list --app --columns=application,version` | Média | IN-03 | Média | ⏳ Pendente | — |
| IN-03b | Infra | `FlatpakUpdateProvider`: `description` recebe `application_id` (`org.gnome.Platform`), não descrição — renomear campo ou buscar via `flatpak info` | Média | IN-03 | Baixa | ⏳ Pendente | — |
| IN-03c | Infra | `FlatpakUpdateProvider`: ampliar testes (Flatpak indisponível, output vazio, `is_available()`, linha malformada) | Média | IN-03 | Baixa | ⏳ Pendente | — |
| IN-04 | Infra | Provedor NPM | Média | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-04a | Infra | `NpmUpdateProvider`: ampliar testes (npm indisponível, JSON malformado, objeto vazio, `is_available()`) | Baixa | IN-04 | Baixa | ⏳ Pendente | — |
| IN-05 | Infra | Provedor APT (Debian/Ubuntu) | Média | IN-01 | Média | ✅ Concluído | ✓ |
| IN-06 | Infra | Provedor Pacman (Arch) | Média | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-06a | Infra | `PacmanUpdateProvider`: ampliar testes (pacman indisponível, output vazio, `is_available()`) | Baixa | IN-06 | Baixa | ⏳ Pendente | — |
| IN-07 | Infra | Provedor Snap | Baixa | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-07a | Infra | `SnapUpdateProvider`: `current_version` vazio — consultar `snap list` cruzado para enriquecer | Média | IN-07 | Média | ⏳ Pendente | — |
| IN-07b | Infra | `SnapUpdateProvider`: ampliar testes (snap indisponível, output sem cabeçalho, `is_available()`) | Baixa | IN-07 | Baixa | ⏳ Pendente | — |
| IN-08 | Infra | Provedor Cargo (Rust) | Baixa | IN-01 | Média | ✅ Concluído | ⚠ |
| IN-08a | Infra | `CargoUpdateProvider`: falha silenciosa quando crate `cargo-update` não instalada — detectar ausência e expor mensagem clara ao usuário | Baixa | IN-08 | Baixa | ⏳ Pendente | — |
| IN-08b | Infra | `CargoUpdateProvider`: ampliar testes (crate ausente, output vazio, `is_available()`) | Baixa | IN-08 | Baixa | ⏳ Pendente | — |
| PR-01 | Presentation | `CpuViewModel` (load total + por core + temperatura) | Alta | AR-01 | Média | ✅ Concluído | ⚠ |
| PR-02 | Presentation | `MemoryViewModel` (uso, cache, swap) | Alta | AR-01 | Média | ✅ Concluído | ⚠ |
| PR-03 | Presentation | `FirmwareViewModel` via DBus (`DBusFirmwareProvider`) | Média | AR-01 | Alta | ✅ Concluído | ✓ |
| PR-04 | Presentation | Fix `UpdateViewModelTest.ShouldRefreshUpdatesFromManager` — mock `check_all_updates()` nunca chamado e `isBusy()` fica travado em `true` | Alta | PR-01 | Média | 🔄 Em andamento | ⚠ |
| PR-05 | Presentation | Fix `SystemInfoViewModelTest.ShouldLoadDataInConstructor` — dados não carregam no ctor | Alta | PR-01 | Média | 🔄 Em andamento | ⚠ |
| PR-06 | Presentation | Fix `ProcessViewModelTest.ShouldFilterAndSort` — ordenação/filtragem regrediu mesmo após correção de SIGSEGV (commit 1279cdf) | Alta | PR-01 | Alta | 🔄 Em andamento | ⚠ |
| AU-01 | Audio | `QtAudioProvider` — waveform + RMS/peak em thread segura | Média | AR-02 | Alta | ✅ Concluído | ⚠ |
| AU-01a | Audio | Encapsular `on_data_ready` como setter (`set_data_callback`) em `IAudioProvider` — hoje é `std::function` público mutável, qualquer consumidor pode sobrescrever o callback silenciosamente | Média | AU-01 | Baixa | ⏳ Pendente | — |
| AU-01b | Audio | Tornar `m_capturing` `std::atomic<bool>` em `QtAudioProvider` — hoje é `bool` simples consultado por `is_capturing()` enquanto pode ser escrito do callback de áudio | Média | AU-01 | Baixa | ⏳ Pendente | — |
| AU-01c | Audio | Marshalar `on_data_ready` para a thread do consumidor via `QMetaObject::invokeMethod(..., Qt::QueuedConnection)` — hoje callback dispara direto da audio thread, acoplamento frágil com `AudioViewModel` | Alta | AU-01 | Média | ⏳ Pendente | — |
| SC-01 | Security | Cofre AES-256-GCM via `OpenSslVault` (`ISecurityVault`) | Alta | — | Alta | 🔍 Pendente verificação | — |
| EX-01 | Export | Relatório JSON em `JsonExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-02 | Export | Relatório Markdown em `MarkdownExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-03 | Export | Relatório HTML em `HtmlExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-04 | Export | Pasta padrão (Documentos) e histórico no `ExportViewModel` | Média | EX-01 | Baixa | 🔍 Pendente verificação | — |
| DB-01 | Dashboard | Card resumo de CPU (Load %) ligado a `CpuViewModel` | Alta | PR-01 | Baixa | 🔍 Pendente verificação | — |
| DB-02 | Dashboard | Card resumo de RAM (Livre/Total) ligado a `MemoryViewModel` | Alta | PR-02 | Baixa | 🔍 Pendente verificação | — |
| DB-03 | Dashboard | Card resumo de Storage (Uso %) ligado a `StorageViewModel` | Alta | AR-02 | Baixa | 🔍 Pendente verificação | — |
| DB-04 | Dashboard | Card resumo de Network (Up/Down) ligado a `NetworkViewModel` | Alta | AR-02 | Baixa | 🔍 Pendente verificação | — |
| DB-05 | Dashboard | Auto-refresh progressivo CPU/MEM/Disk/Net via timers + mutex | Alta | DB-01, DB-02, DB-03, DB-04 | Média | 🔍 Pendente verificação | — |
| DB-06 | Dashboard | Interatividade — clique em card navega/abre detalhe | Média | DB-01 | Baixa | 🔍 Pendente verificação | — |
| DB-07 | Dashboard | Áudio real-time: waveform + spectrum em `AudioCanvas` | Alta | AU-01 | Média | 🔍 Pendente verificação | — |
| UP-01 | Updates | Contador de pacotes desatualizados agregando provedores | Alta | IN-02 | Baixa | 🔍 Pendente verificação | — |
| UP-02 | Updates | Lista de pacotes com Nome/Versão via `UpdatePackageModel` | Alta | IN-02 | Média | 🔍 Pendente verificação | — |
| UP-03 | Updates | Botão "Atualizar Tudo" com `IInstaller` + log interativo | Alta | IN-01, UP-02 | Média | 🔍 Pendente verificação | — |
| JR-01 | Journal | Filtro Hardware/Firmware no `DiagnosticViewModel` | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| JR-02 | Journal | Filtro Kernel/Mensagens | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| JR-03 | Journal | Filtro Ambiente (KDE/Wayland) | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| JR-04 | Journal | Double-Click abre busca online no navegador padrão | Média | JR-01 | Baixa | 🔍 Pendente verificação | — |
| HW-01 | Hardware | SMBIOS/DMI via `LinuxHardwareProvider` | Média | AR-02 | Baixa | 🔍 Pendente verificação | — |
| HW-02 | Hardware | Topologia USB/Bluetooth | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| HW-03 | Hardware | Detecção GPU (NVIDIA + Sysfs) | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| SV-01 | Services | Listagem de unidades systemd (`SystemdServiceProvider`) | Alta | AR-02 | Média | 🔍 Pendente verificação | — |
| SV-02 | Services | Controle start/stop/restart via D-Bus | Alta | SV-01 | Alta | 🔍 Pendente verificação | — |
| SV-03 | Services | Criação de daemons user (UI + unit file) | Alta | SV-02 | Alta | 🔍 Pendente verificação | — |
| DT-01 | DevTools | Detecção de compiladores/runtimes/containers | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| UX-01 | UI/UX | Barras de rolagem + ordenação estável em listas longas | Alta | — | Baixa | 🔍 Pendente verificação | — |
| UX-02 | UI/UX | Seletor de pasta nativo KDE (StandardPaths + FolderDialog) | Alta | — | Média | 🔍 Pendente verificação | — |
| UX-03 | UI/UX | Auditoria de layouts QML responsivos (remover tamanhos hardcoded, usar `Layout.fillWidth/Height`, anchors corretos) | Média | — | Média | ⏳ Pendente | — |
| UX-04 | UI/UX | Feedback visual (loading spinners, error banners, success toasts) em ações assíncronas | Alta | — | Média | ⏳ Pendente | — |
| UX-05 | UI/UX | Auditoria de operações pesadas na UI thread (garantir `QtConcurrent`/`QThread` em I/O e parsing) | Alta | AR-03 | Alta | ⏳ Pendente | — |
| QA-01 | QA | T1 — Suite completa de unitários GTest (39 arquivos) | Alta | AR-03 | Alta | ⏳ Pendente | — |
| QA-02 | QA | T2 — Clang-Tidy + CppCheck sem `bugprone-*`/`security-*`/`cert-*` | Alta | — | Baixa | ⏳ Pendente | — |
| QA-03 | QA | T3 — Suite ATDD (19 arquivos) | Alta | QA-01 | Alta | ⏳ Pendente | — |
| QA-04 | QA | T4 — Valgrind/ASan zero *definitely lost* e zero ERROR SUMMARY | Alta | QA-01 | Alta | ⏳ Pendente | — |
| QA-05 | QA | A3 — WCAG AA (contraste ≥4,5:1, Tab/Enter, tooltips) | Média | DB-06, UX-01 | Média | ⏳ Pendente | — |
| QA-06 | QA | A6 — Cobertura LCOV ≥ 70% em `src/domain/` e serviços | Média | QA-01, QA-03 | Alta | ⏳ Pendente | — |
| QA-07 | QA | A9 — Análise SOLID/SRP (classes >300 linhas, ciclos de include) | Alta | AR-01, AR-02 | Alta | ⏳ Pendente | — |
| QA-08 | QA | A10 — Relatório final de auditoria consolidado | Alta | QA-01, QA-02, QA-03, QA-04, QA-05, QA-06, QA-07 | Baixa | ⏳ Pendente | — |
| QA-09 | QA | Null-pointer checks antes de dereference (`QPointer`, guards `if (!ptr) return`, raw pointer policy) | Alta | QA-02 | Média | ⏳ Pendente | — |
| QA-10 | QA | Const-correctness e uso obrigatório de `override`/`final` em hierarquias virtuais | Média | QA-02 | Baixa | ⏳ Pendente | — |
| QA-11 | QA | Higiene de headers (include-what-you-use, remoção de includes desnecessários, forward declarations) | Média | — | Média | ⏳ Pendente | — |
| QA-12 | QA | Consistência de tipos de string (`QString` nas fronteiras Qt, `std::string` interno, evitar `const char*` soltos) | Baixa | — | Baixa | ⏳ Pendente | — |
| QA-13 | QA | Exception safety (strong/basic guarantee em setters, `noexcept` em destrutores e move constructors) | Média | — | Média | ⏳ Pendente | — |
| QA-14 | QA | Cobertura de error handling em fronteiras externas (D-Bus, shell, filesystem) com logs e propagação explícita | Alta | — | Média | ⏳ Pendente | — |
| QA-15 | QA | ThreadSanitizer (`-fsanitize=thread`) sobre `unit_tests` para detectar race conditions residuais | Alta | QA-01 | Alta | ⏳ Pendente | — |
| QA-16 | QA | Diagnóstico final categorizado (CRÍTICO / IMPORTANTE / COSMÉTICO) com estimativas de esforço por item | Alta | QA-08 | Baixa | ⏳ Pendente | — |
| QA-17 | QA | Migrar literal `_qs` deprecado para `_s` do namespace `Qt::StringLiterals` em `src/main.cpp:151` | Baixa | — | Baixa | ⏳ Pendente | — |
| QA-18 | QA | Remover `using namespace my_comp::domain::interfaces;` dos headers (dnf, flatpak, npm update providers) — poluição de escopo em consumidores | Baixa | AR-02 | Baixa | ⏳ Pendente | — |
| BT-01 | Beta | FFT real no áudio — Cooley-Tukey iterativo já implementado (Hann window, `std::bit_floor`, bit-reversal); avaliar migração opcional para Kiss FFT/FFTW por performance e spectrum detalhado (bins log-espaçados, smoothing) | Média | DB-07, AU-01 | Média | 🟡 Parcial | ⚠ |
| BT-02 | Beta | Fuzzing AFL++/libFuzzer em parsers (journal, SMBIOS, D-Bus) | Média | QA-02 | Alta | ⏳ Pendente | — |
| BT-03 | Beta | Script `scripts/ci.sh` (build + ctest + clang-tidy + cppcheck + valgrind) | Média | QA-02, QA-04 | Média | ⏳ Pendente | — |
| BT-04 | Beta | Release Beta v2.0 (CHANGELOG + tag + push Codeberg) | Alta | QA-08, QA-16, BT-01, BT-02, BT-03 | Baixa | ⏳ Pendente | — |

## Roadmap

1. **Fundação** — AR-01 → AR-02 → AR-03 (AR-04 é refinamento opcional, pode ficar por último).
2. **Infraestrutura** — IN-01 (shell) → IN-02…IN-08 (provedores de update).
3. **Presentation core** — PR-01 → PR-02 → PR-03.
4. **Áudio & segurança** — AU-01, SC-01.
5. **Exporters** — EX-01 → EX-04.
6. **Features visuais (periferia)** — DB-*, UP-*, JR-*, HW-*, SV-*, DT-*, UX-* (smoke manual em Wayland).
7. **Gates QA** — QA-01 → QA-08 (testes rigorosos por último, quando o código já está estável) → QA-09 → QA-15 (auditorias complementares: null checks, const/override, headers, strings, exception safety, error handling, TSan) → QA-16 (diagnóstico categorizado).
8. **Beta** — BT-01 → BT-04 (FFT, fuzzing, CI, release).

## Notas de verificação por ID

Esta seção é preenchida pela Fase 2. Cada entrada registra o comando usado, o resultado (pass/fail) e o arquivo/teste envolvido, ex.:

```
- DB-01 (2026-04-19): ctest -R cpu_viewmodel_test → PASS; smoke manual: Main.qml abre card com valor > 0.
- IN-02 (2026-04-19): unit_tests --gtest_filter=DnfUpdateProviderTest.* → FAIL em ParsesFlatpakList (hipótese: regex não cobre prefix "info:"). Nova linha: IN-02a.
```

- **AR-01 (2026-04-19)**: `ls src/domain/interfaces | wc -l` → 20; `ls src/domain/models | wc -l` → 19. `grep` por includes proibidos (QtGui/Quick/Widgets/DBus/Network/Multimedia/Concurrent, QProcess, OpenSSL, dbus, sys/) → **0 matches**. Todas 20 interfaces em `IPascalCase` com métodos virtuais puros. `cmake --build build` → 223/223 targets, zero erros, 1 warning em `src/main.cpp:151` (`_qs` deprecado → QA-17). `ctest` → 59/62 PASS; as 3 falhas (UpdateViewModel, SystemInfoViewModel, ProcessViewModel) são de Presentation e não afetam AR-01 → PR-04, PR-05, PR-06.
- **AR-02 (2026-04-19)**: 26 arquivos em `collectors/` + 3 em `exporters/` + 1 em `audio/` + 1 em `security/`. 30 classes de infra herdam de interfaces do domain (via `domain::interfaces::I*` ou `using namespace`). `grep` por `#include.*infrastructure/` em `src/domain` → **0 matches** (dependência inversa respeitada). `unit_tests --gtest_filter='*UpdateProvider*:*ExportProvider*:*Vault*:*ShellExecutor*:…'` → **35/35 PASS** em 25 test suites. Ressalva: 3 headers (`dnf`, `flatpak`, `npm`) usam `using namespace my_comp::domain::interfaces;` → QA-18.
- **AR-03 (2026-04-19)**: sequenciador QML presente em `Main.qml:42-73` (Timer 300ms, switch com 12 steps). 5 ViewModels (diagnostic, dev_stack, process, service, firmware) usam `QtConcurrent::run` + `QMetaObject::invokeMethod` com `Qt::QueuedConnection` — thread-safety OK. `LinuxProcessProvider` protege acesso com `std::lock_guard<std::mutex>`; `LinuxUserMetadataProvider` usa `getpwuid_r`/`getgrgid_r` + mutex global (CLAUDE.md §5). Ressalvas: (a) 4 VMs fazem I/O no ctor (Cpu, Hardware, Power, Gpu) violando §4 → AR-07; (b) 4 refresh() síncronos na UI thread (Network, Storage, Memory, Hardware) → AR-08; (c) sequenciador por Timer é arbitrário → AR-09.
- **IN-01 (2026-04-19)**: `IShellExecutor` com 1 método virtual puro + `QProcessShellExecutor` usando `QProcess` stack-allocated, timeout 5s com `kill()`, `start(command, args)` (sem shell interpretation → sem injection). 50 consumidores no código + testes. `ShellExecutor` filter passou em AR-02. Ressalvas: (a) não captura stderr → IN-01a; (b) herda env do pai → IN-01b.
- **IN-02 (2026-04-19)**: `DnfUpdateProvider` header-only, DI via `IShellExecutor`. Usa exit code 100 (convenção DNF). `unit_tests --gtest_filter='DnfUpdateProviderTest.*'` → 1/1 PASS. Revisão manual do parsing expôs bugs de semântica: `current_version` nunca preenchido → IN-02a; `description` recebe nome do repositório (`updates`/`fedora`), não descrição → IN-02b; cobertura de teste precisa ampliar → IN-02c.
- **IN-03 (2026-04-19)**: `FlatpakUpdateProvider` header-only, parse por tab (`\t`) — mais robusto que whitespace do DNF. `unit_tests --gtest_filter='FlatpakUpdateProviderTest.*'` → 1/1 PASS. Bugs paralelos ao DNF: `current_version` vazio → IN-03a; `description` recebe `application_id` em vez de descrição → IN-03b; cobertura happy-path apenas → IN-03c.
- **IN-04 (2026-04-19)**: `NpmUpdateProvider` usa `QJsonDocument` (parsing robusto, sem regex). `current_version` preenchido corretamente (sem bug do IN-02a/IN-03a). `unit_tests --gtest_filter='NpmUpdateProviderTest.*'` → 1/1 PASS. Ressalva: `description` é placeholder fixo ("Global node package"), aceitável como MVP. Cobertura a ampliar → IN-04a.
- **IN-05 (2026-04-19)**: `AptUpdateProvider` com `.h`+`.cpp` separados, regex específica (3 capture groups: nome, nova_versão, versão_atual). `current_version` preenchido corretamente. `unit_tests --gtest_filter='AptUpdateProviderTest.*'` → **3/3 PASS** (happy path, falha, availability). Primeiro provider auditado ✓ sem ressalvas estruturais — `description`/`is_critical` ficam com defaults por limitação da fonte APT (não bug).
- **IN-06/07/08 (2026-04-19)**: Pacman/Snap/Cargo todos `.h`+`.cpp` separados, sem `using namespace` em header. `unit_tests --gtest_filter='PacmanUpdateProviderTest.*:SnapUpdateProviderTest.*:CargoUpdateProviderTest.*'` → **3/3 PASS**. Pacman: `parts[2]=="->"` valida semântica, OK (⚠ só cobertura IN-06a). Snap: `current_version` vazio (bug igual IN-02a/IN-03a) → IN-07a; cobertura IN-07b. Cargo: falha silenciosa se crate `cargo-update` ausente → IN-08a; cobertura IN-08b.
- **PR-01/02/03 (2026-04-19)**: `CpuViewModel` (11 Q_PROPERTY, separação dinâmicos vs CONSTANT de arquitetura), `MemoryViewModel` (8 Q_PROPERTY com helper `format_kb`), `FirmwareViewModel` (3 Q_PROPERTY com async correto `QtConcurrent`+`invokeMethod`). `unit_tests --gtest_filter='CpuViewModelTest.*:MemoryViewModelTest.*:FirmwareViewModelTest.*'` → **3/3 PASS**. Ressalvas já registradas: PR-01 → AR-07 (ctor I/O); PR-02 → AR-08 (refresh síncrono). PR-03 aprovado ✓ sem ressalvas (padrão async correto).
- **AU-01 (2026-04-19)**: `IAudioProvider` com 3 métodos (`start_capture`/`stop_capture`/`is_capturing`) + `std::function<void(AudioData)> on_data_ready` membro público. `QtAudioProvider` implementa via `QAudioSource` + `AudioSink : public QIODevice` (callback-based). Constructor: 44100Hz, mono, Float; enumera `QMediaDevices::audioInputs()` e usa `defaultAudioInput()`. `process_samples` calcula RMS + peak e chama `calculate_fft` (Cooley-Tukey iterativo, janela Hann, `std::bit_floor` C++20, permutação bit-reversa, butterfly iterativo — n/2 bins de magnitude). RAII correto (destrutor chama `stop_capture`). `unit_tests --gtest_filter='*Audio*'` → **3/3 PASS** (AudioFuzzTest.StressWithRandomData 467ms, HandleExtremeValues 0ms, acceptance). Ressalvas: (a) `on_data_ready` como membro público mutável → AU-01a; (b) `m_capturing` não atômico → AU-01b; (c) callback dispara da audio thread sem marshalling cross-thread → AU-01c. Consequência para BT-01: FFT **já implementado**, BT-01 reclassificado para `🟡 Parcial` (migração para Kiss FFT/FFTW vira refinamento opcional).
