# my_comp v2.0 — Planejamento

**Última auditoria registrada:** 2026-04-19 (reabertura). Os 43 itens funcionais herdados da TABELA.md anterior entram como `🔍 Pendente verificação` + Estado Auditado `—` e só migram para `✅ Concluído` + `✓` depois que a Fase 2 (rebuild + ctest + clang-tidy + valgrind + smoke manual) confirmar cada um. Qualquer falha vira nova linha no grupo correspondente com `Pré-requisito` apontando para o ID original.

**Legenda rápida:**
- Status: ✅ Concluído · 🔄 Em andamento · 🟡 Parcial · ⏳ Pendente · 💡 Decisão tomada · 🎨 Pendente design · 🔍 Pendente verificação
- Estado Auditado: `—` (não auditado) · `✓` (aprovado) · `⚠` (com ressalvas)

| ID | Grupo | Descrição Técnica | Prioridade | Pré-requisito | Dificuldade | Status | Estado Auditado |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| DB-01 | Dashboard | Card resumo de CPU (Load %) ligado a `CpuViewModel` | Alta | — | Baixa | 🔍 Pendente verificação | — |
| DB-02 | Dashboard | Card resumo de RAM (Livre/Total) ligado a `MemoryViewModel` | Alta | — | Baixa | 🔍 Pendente verificação | — |
| DB-03 | Dashboard | Card resumo de Storage (Uso %) ligado a `StorageViewModel` | Alta | — | Baixa | 🔍 Pendente verificação | — |
| DB-04 | Dashboard | Card resumo de Network (Up/Down) ligado a `NetworkViewModel` | Alta | — | Baixa | 🔍 Pendente verificação | — |
| DB-05 | Dashboard | Auto-refresh progressivo CPU/MEM/Disk/Net via timers + mutex | Alta | DB-01, DB-02, DB-03, DB-04 | Média | 🔍 Pendente verificação | — |
| DB-06 | Dashboard | Interatividade — clique em card navega/abre detalhe | Média | DB-01 | Baixa | 🔍 Pendente verificação | — |
| DB-07 | Dashboard | Áudio real-time: waveform + spectrum exibidos em `AudioCanvas` | Alta | AU-01 | Média | 🔍 Pendente verificação | — |
| UP-01 | Updates | Contador de pacotes desatualizados agregando provedores | Alta | IN-02 | Baixa | 🔍 Pendente verificação | — |
| UP-02 | Updates | Lista de pacotes com Nome/Versão via `UpdatePackageModel` | Alta | IN-02 | Média | 🔍 Pendente verificação | — |
| UP-03 | Updates | Botão "Atualizar Tudo" com `IInstaller` + log interativo | Alta | IN-01, UP-02 | Média | 🔍 Pendente verificação | — |
| JR-01 | Journal | Filtro Hardware/Firmware no `DiagnosticViewModel` | Média | — | Média | 🔍 Pendente verificação | — |
| JR-02 | Journal | Filtro Kernel/Mensagens | Média | — | Média | 🔍 Pendente verificação | — |
| JR-03 | Journal | Filtro Ambiente (KDE/Wayland) | Média | — | Média | 🔍 Pendente verificação | — |
| JR-04 | Journal | Double-Click abre busca online no navegador padrão | Média | JR-01 | Baixa | 🔍 Pendente verificação | — |
| HW-01 | Hardware | SMBIOS/DMI via `LinuxHardwareProvider` | Média | — | Baixa | 🔍 Pendente verificação | — |
| HW-02 | Hardware | Topologia USB/Bluetooth | Média | — | Média | 🔍 Pendente verificação | — |
| HW-03 | Hardware | Detecção GPU (NVIDIA + Sysfs) | Média | — | Média | 🔍 Pendente verificação | — |
| SC-01 | Security | Cofre AES-256-GCM via `OpenSslVault` (`ISecurityVault`) | Alta | — | Alta | 🔍 Pendente verificação | — |
| EX-01 | Export | Relatório JSON em `JsonExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-02 | Export | Relatório Markdown em `MarkdownExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-03 | Export | Relatório HTML em `HtmlExportProvider` | Baixa | AR-01 | Baixa | 🔍 Pendente verificação | — |
| EX-04 | Export | Pasta padrão (Documentos) e histórico no `ExportViewModel` | Média | EX-01 | Baixa | 🔍 Pendente verificação | — |
| IN-01 | Infra | Executor de shell genérico (`QProcessShellExecutor`) | Alta | — | Baixa | 🔍 Pendente verificação | — |
| IN-02 | Infra | Provedor DNF (`DnfUpdateProvider`) | Alta | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-03 | Infra | Provedor Flatpak | Média | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-04 | Infra | Provedor NPM | Média | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-05 | Infra | Provedor APT (Debian/Ubuntu) | Média | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-06 | Infra | Provedor Pacman (Arch) | Média | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-07 | Infra | Provedor Snap | Baixa | IN-01 | Média | 🔍 Pendente verificação | — |
| IN-08 | Infra | Provedor Cargo (Rust) | Baixa | IN-01 | Média | 🔍 Pendente verificação | — |
| PR-01 | Presentation | `CpuViewModel` (load total + por core + temperatura) | Alta | AR-01 | Média | 🔍 Pendente verificação | — |
| PR-02 | Presentation | `MemoryViewModel` (uso, cache, swap) | Alta | AR-01 | Média | 🔍 Pendente verificação | — |
| PR-03 | Presentation | `FirmwareViewModel` via DBus (`DBusFirmwareProvider`) | Média | AR-01 | Alta | 🔍 Pendente verificação | — |
| AU-01 | Audio | `QtAudioProvider` coletando waveform + RMS/peak em thread segura | Média | AR-02 | Alta | 🔍 Pendente verificação | — |
| AR-01 | Architecture | Camada Domain (Mid) com 20 interfaces + 19 modelos | Alta | — | Alta | 🔍 Pendente verificação | — |
| AR-02 | Architecture | Camada Infrastructure (End) com 26 providers + 3 exporters | Alta | AR-01 | Alta | 🔍 Pendente verificação | — |
| AR-03 | Architecture | Inicialização progressiva assíncrona (sequenciador QML + thread-safety) | Alta | AR-02 | Alta | 🔍 Pendente verificação | — |
| SV-01 | Services | Listagem de unidades systemd em `SystemdServiceProvider` | Alta | AR-02 | Média | 🔍 Pendente verificação | — |
| SV-02 | Services | Controle start/stop/restart via D-Bus | Alta | SV-01 | Alta | 🔍 Pendente verificação | — |
| SV-03 | Services | Criação de daemons user (UI + unit file) | Alta | SV-02 | Alta | 🔍 Pendente verificação | — |
| DT-01 | DevTools | Detecção de compiladores/runtimes/containers (`LinuxDevStackProvider`) | Média | AR-02 | Média | 🔍 Pendente verificação | — |
| UX-01 | UI/UX | Barras de rolagem + ordenação estável em listas longas | Alta | — | Baixa | 🔍 Pendente verificação | — |
| UX-02 | UI/UX | Seletor de pasta nativo KDE (StandardPaths + FolderDialog) | Alta | — | Média | 🔍 Pendente verificação | — |
| QA-01 | QA | T1 — Suite completa de unitários GTest passando (39 arquivos) | Alta | AR-03 | Alta | ⏳ Pendente | — |
| QA-02 | QA | T2 — Clang-Tidy + CppCheck sem `bugprone-*`/`security-*`/`cert-*` | Alta | — | Baixa | ⏳ Pendente | — |
| QA-03 | QA | T3 — Suite ATDD (19 arquivos) passando (`acceptance_tests`) | Alta | QA-01 | Alta | ⏳ Pendente | — |
| QA-04 | QA | T4 — Valgrind/ASan zero `definitely lost` e zero `ERROR SUMMARY` | Alta | QA-01 | Alta | ⏳ Pendente | — |
| QA-05 | QA | A3 — WCAG AA (contraste ≥4,5:1, Tab/Enter, tooltips) | Média | DB-06, UX-01 | Média | ⏳ Pendente | — |
| QA-06 | QA | A6 — Cobertura LCOV ≥ 70% em `src/domain/` e serviços | Média | QA-01, QA-03 | Alta | ⏳ Pendente | — |
| QA-07 | QA | A9 — Análise SOLID/SRP (classes >300 linhas, ciclos de include) | Alta | AR-01, AR-02 | Alta | ⏳ Pendente | — |
| QA-08 | QA | A10 — Relatório final de auditoria consolidado | Alta | QA-01, QA-02, QA-03, QA-04, QA-05, QA-06, QA-07 | Baixa | ⏳ Pendente | — |
| BT-01 | Beta | FFT real no áudio (Kiss FFT ou FFTW) + spectrum detalhado | Alta | DB-07, AU-01 | Alta | ⏳ Pendente | — |
| BT-02 | Beta | Fuzzing AFL++/libFuzzer em parsers (journal, SMBIOS, D-Bus) | Média | QA-02 | Alta | ⏳ Pendente | — |
| BT-03 | Beta | Script `scripts/ci.sh` (build + ctest + clang-tidy + cppcheck + valgrind) | Média | QA-02, QA-04 | Média | ⏳ Pendente | — |
| BT-04 | Beta | Release Beta v2.0 (CHANGELOG + tag + push Codeberg) | Alta | QA-08, BT-01, BT-02, BT-03 | Baixa | ⏳ Pendente | — |

## Roadmap

1. **Fase de reverificação (imediata)** — executar rebuild limpo, `ctest --output-on-failure`, `valgrind`, `clang-tidy`, `cppcheck` e percorrer `DB-*`, `UP-*`, `JR-*`, `HW-*`, `SC-*`, `EX-*`, `IN-*`, `PR-*`, `AU-*`, `AR-*`, `SV-*`, `DT-*`, `UX-*` um a um. Cada item vai para ✅/✓ ou gera sub-item corretivo.
2. **Fase de gates (QA-01 a QA-08)** — fechar os 8 gates na ordem listada.
3. **Fase Beta (BT-01 a BT-04)** — FFT real, fuzzing, CI e release.

## Notas de verificação por ID

Esta seção é preenchida pela Fase 2. Cada entrada registra o comando usado, o resultado (pass/fail) e o arquivo/teste envolvido, ex.:

```
- DB-01 (2026-04-19): ctest -R cpu_viewmodel_test → PASS; smoke manual: Main.qml abre card com valor > 0.
- IN-02 (2026-04-19): unit_tests --gtest_filter=DnfUpdateProviderTest.* → FAIL em ParsesFlatpakList (hipótese: regex não cobre prefix "info:"). Nova linha: IN-02a.
```
