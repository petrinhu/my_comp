# vram_optimizer — Planejamento

Tabela única de pendências e planejamento. Formato da skill `tab_pendencias`. Toda decisão pendente, tarefa em andamento ou desvio explícito do `docs/CONTRACT.md` (grupo `AUD`) é rastreada aqui.

| ID | Grupo | Descrição Técnica | Prioridade | Pré-requisito | Dificuldade | Status | Estado Auditado |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| DES-01 | DESIGN | Brainstorming: heurística de classificação CPU-only (whitelist por nome / categoria / aprendizado por feedback) | Alta | — | Média | ✅ Concluído | ✓ |
| DES-02 | DESIGN | Brainstorming: mapeamento processo→config-fonte (Chromium, Firefox, .desktop, PRIME-env, VAAPI) | Alta | — | Alta | ✅ Concluído | ✓ |
| DES-03 | DESIGN | Brainstorming: modo de operação (snapshot único / polling / daemon+tray / on-demand) e privilégios | Alta | — | Média | 🎨 Pendente design | — |
| DES-04 | DESIGN | Decidir entrada de MySQL no MVP ou fase 2 (alternativa: SQLite ou arquivo) | Alta | DES-01, DES-03 | Baixa | ⏳ Pendente | — |
| DES-05 | DESIGN | Plano formal de implementação por fases via `superpowers:writing-plans` | Alta | DES-01, DES-02, DES-03, DES-04 | Média | 🟡 Parcial | — |
| BOOT-01 | BOOTSTRAP | `git init` + `.gitignore` + commit `chore: bootstrap inicial do projeto` (inclui docs/, CLAUDE.md, README.md, TODO.md) | Alta | — | Baixa | ✅ Concluído | ✓ |
| BOOT-02 | BOOTSTRAP | Estrutura de diretórios em 4 camadas (`src/presentation/`, `application/`, `domain/`, `infrastructure/`) | Alta | DES-05 | Baixa | 🟡 Parcial | — |
| BOOT-03 | BOOTSTRAP | `CMakeLists.txt` raiz com C++23, `qt_add_executable`, `qt_standard_project_setup()`, presets dev/release/ci | Alta | BOOT-02 | Média | ✅ Concluído | ✓ |
| BOOT-04 | BOOTSTRAP | `cmake/CompilerFlags.cmake` com flags estritas + sanitizers via `option(ENABLE_SANITIZERS)` | Alta | BOOT-03 | Baixa | ✅ Concluído | ✓ |
| BOOT-05 | BOOTSTRAP | Targets `vram_optimizer_gui` e `vram_optimizer_cli` compartilhando libs `vram_application` e `vram_domain` | Alta | BOOT-03 | Média | 🟡 Parcial | — |
| BOOT-06 | BOOTSTRAP | `.clang-tidy` + `.clang-format` alinhados a `docs/cpp_qt6_auditoria_seguranca.md` §6 | Média | BOOT-03 | Baixa | ✅ Concluído | ✓ |
| BOOT-07 | BOOTSTRAP | `CHANGELOG.md` inicial bilíngue (pt-br/en) seguindo Keep a Changelog | Média | BOOT-01 | Baixa | ⏳ Pendente | — |
| BOOT-08 | BOOTSTRAP | `CONTRIBUTING.md` inicial bilíngue (fluxo de PR, Conventional Commits, gates) | Média | BOOT-01 | Baixa | ⏳ Pendente | — |
| TEMA-01 | TEMA | Extrair/converter assets oficiais Breeze (paleta, QSS, ícones SVG) para uso embarcado | Alta | DES-05 | Média | ⏳ Pendente | — |
| TEMA-02 | TEMA | `assets/temas/breeze/` com `breeze.qss`, `paleta.qss` (ou C++), `icones/*.svg` e `breeze.qrc` registrado no CMake | Alta | TEMA-01, BOOT-03 | Média | ⏳ Pendente | — |
| TEMA-03 | TEMA | Classe `TemaBreeze` com `carregarPaleta()` e `aplicarTema(QApplication&)` | Alta | TEMA-02 | Baixa | ⏳ Pendente | — |
| TEMA-04 | TEMA | Integrar aplicação do tema em `main.cpp` da GUI antes de criar qualquer widget | Alta | TEMA-03 | Baixa | ⏳ Pendente | — |
| TEMA-05 | TEMA | Smoke test em máquina sem KDE (Fedora GNOME / Ubuntu) — aparência Breeze idêntica | Alta | TEMA-04 | Baixa | 🔍 Pendente verificação | — |
| DOM-01 | DOMAIN | Entidade `Processo` (PID, nome, bytes_vram, gpu_id) — TDD com GTest | Alta | DES-05 | Baixa | ✅ Concluído | ✓ |
| DOM-02 | DOMAIN | Entidade `GpuInfo` (id, fabricante, modelo, bytes_vram_total) — TDD | Alta | DES-05 | Baixa | ✅ Concluído | ✓ |
| DOM-03 | DOMAIN | Entidade `OrigemConfig` (categoria, tipo, caminho, chave, valor_atual, valor_sugerido, instrucao_humana, confianca, requer_reinicio_app, aplicavel_automatico) + `EntradaDesktopExec` — TDD | Alta | DES-02 | Média | ✅ Concluído | ✓ |
| DOM-04 | DOMAIN | Value Objects: `PID`, `BytesDeMemoria`, `NomeProcesso` com invariantes — TDD | Alta | DES-05 | Baixa | ✅ Concluído | ✓ |
| DOM-05 | DOMAIN | Interface `IGpuBackend` (listarProcessos, vramTotal, fabricante) — TDD com mock | Alta | DOM-01, DOM-02 | Baixa | ⏳ Pendente | — |
| DOM-06 | DOMAIN | Interface `IConfigLocator` (localizar(processo) → QVector&lt;OrigemConfig&gt;) — TDD com mock | Alta | DOM-03 | Média | ✅ Concluído | ✓ |
| DOM-10 | DOMAIN | Interfaces complementares de DES-02: `ILeitorEnviron`, `ILeitorArquivoConfig`, `ILeitorDesktopExec` + enums `CategoriaOrigem`/`Confianca` — TDD via static_assert is_abstract_v | Alta | DOM-03 | Baixa | ✅ Concluído | ✓ |
| DOM-07 | DOMAIN | Interface `IClassificadorProcessos` (classificar(processo) → Categoria) — TDD com mock | Alta | DOM-01, DES-01 | Média | ✅ Concluído | ✓ |
| DOM-08 | DOMAIN | `IRegraClassificacao` + `ICatalogoClassificacao`/`IRepositorioFeedback`/`ILeitorDesktopFile`/`ILeitorMapsProcesso`/`IAmostradorComportamento` — 6 interfaces complementares para a cascata DES-01 (TDD via static_assert is_abstract_v) | Alta | DOM-07 | Baixa | ✅ Concluído | ✓ |
| DOM-09 | DOMAIN | Value Objects + structs DES-01: `ChaveProcesso`, `EntradaCatalogo`, `EntradaFeedback`, `AmostraJanela`, `LimiaresComportamento`, `Categoria`, `Justificativa`, `ResultadoClassificacao` — TDD | Alta | DOM-04 | Baixa | ✅ Concluído | ✓ |
| APP-01 | APPLICATION | Service `AuditarVramAtual` orquestrando `IGpuBackend` — TDD com GMock | Alta | DOM-05 | Média | ⏳ Pendente | — |
| APP-02 | APPLICATION | Service `ServicoLocalizarConfig` orquestrando `IConfigLocator` (composite) — TDD com GMock | Alta | DOM-06 | Média | ✅ Concluído | ✓ |
| APP-07 | APPLICATION | 4 sublocators DES-02 (`LocalizadorBrowserChromium`, `LocalizadorFirefox`, `LocalizadorEnvVar`, `LocalizadorDesktopWrapper`) + `LocalizadorComposite` + teste de integração com sublocators reais — TDD com GMock | Alta | DOM-10 | Alta | ✅ Concluído | ✓ |
| APP-03 | APPLICATION | Service `ClassificarProcesso` orquestrando `IClassificadorProcessos` — TDD com GMock (`ClassificadorEmCascata` + `ServicoClassificarProcesso`) | Alta | DOM-07 | Média | ✅ Concluído | ✓ |
| APP-04 | APPLICATION | Service `SugerirAcaoMitigatoria` baseado em `OrigemConfig` — TDD (`ServicoSugerirAcao` mapeamento básico pronto; templates concretos por app dependem de Infrastructure) | Média | APP-02 | Alta | 🟡 Parcial | — |
| APP-05 | APPLICATION | 5 regras concretas da cascata DES-01: `RegraFeedback`, `RegraCatalogo`, `RegraCategoriaDesktop` + `MapaCategoriasDesktop`, `RegraBibliotecaCompute` + `ListaLibsCompute`, `RegraComportamento` — TDD com GMock | Alta | DOM-08 | Alta | ✅ Concluído | ✓ |
| INF-01 | INFRASTRUCTURE | `NvmlGpuBackend` wrapper de `libnvidia-ml` (enumeração de processos + VRAM) + teste integração | Alta | DOM-05 | Alta | ⏳ Pendente | — |
| INF-02 | INFRASTRUCTURE | `IntelI915GpuBackend` (sysfs `/sys/class/drm/card*/` + Level Zero quando disponível) + teste | Alta | DOM-05 | Alta | ⏳ Pendente | — |
| INF-03 | INFRASTRUCTURE | Factory de `IGpuBackend` com detecção em runtime das GPUs presentes | Alta | INF-01, INF-02 | Média | ⏳ Pendente | — |
| INF-04 | INFRASTRUCTURE | `ChromiumConfigParser` (Local State + Preferences) | Alta | DOM-06 | Média | ⏳ Pendente | — |
| INF-05 | INFRASTRUCTURE | `FirefoxConfigParser` (`prefs.js` / `user.js`) | Alta | DOM-06 | Média | ⏳ Pendente | — |
| INF-06 | INFRASTRUCTURE | `DesktopFileParser` (casamento `cmdline` ↔ `.desktop` instalados) | Alta | DOM-06 | Média | ⏳ Pendente | — |
| INF-07 | INFRASTRUCTURE | `EnvironParser` (`/proc/<pid>/environ`) com fallback gracioso para falta de privilégio | Alta | DOM-06 | Média | ⏳ Pendente | — |
| INF-08 | INFRASTRUCTURE | Persistência opcional: `MySqlPerfilRepositorio` ou `SqliteRepositorio` (atrás de interface no Domain) | Média | DES-04 | Alta | 💡 Decisão tomada | — |
| PRE-01 | PRESENTATION | GUI Qt: `MainWindow` com layouts responsivos + tema Breeze aplicado | Alta | APP-01, TEMA-04 | Média | ⏳ Pendente | — |
| PRE-02 | PRESENTATION | GUI: `QAbstractTableModel` para lista de processos com VRAM e classificação | Alta | PRE-01 | Média | ⏳ Pendente | — |
| PRE-03 | PRESENTATION | GUI: exibição de origem de config detectada + ações sugeridas | Alta | PRE-02, APP-02 | Média | ⏳ Pendente | — |
| PRE-04 | PRESENTATION | CLI: argv parsing (snapshot, watch, --json, --theme) + saída tabular/JSON | Alta | APP-01 | Média | ⏳ Pendente | — |
| PRE-05 | PRESENTATION | i18n: `QLocale` pt-BR, timezone `America/Sao_Paulo`, fluxo `lupdate`→`.ts`→`lrelease`→`.qm` | Alta | PRE-01 | Média | ⏳ Pendente | — |
| PRE-06 | PRESENTATION | Acessibilidade: `setAccessibleName`, tab order, contraste WCAG AA, atalhos de teclado | Média | PRE-01 | Média | ⏳ Pendente | — |
| CI-01 | CI | GitHub Actions: matrix Ubuntu × {GCC, Clang}, preset `ci`, build + ctest | Alta | BOOT-05 | Média | ⏳ Pendente | — |
| CI-02 | CI | CI: cobertura via `lcov` + gate ≥ 60% global (Domain ≥ 90%, Application ≥ 80%) | Alta | CI-01 | Média | ⏳ Pendente | — |
| CI-03 | CI | CI: análise estática `clang-tidy` + `cppcheck` + `flawfinder` com gates de segurança | Alta | CI-01 | Média | ⏳ Pendente | — |
| CI-04 | CI | CI: build com sanitizers (`-fsanitize=address,undefined,leak`) executando testes | Alta | CI-01 | Média | ⏳ Pendente | — |
| CI-05 | CI | CI: `gitleaks` + `trufflehog` para varredura de secrets em PR | Alta | CI-01 | Baixa | ⏳ Pendente | — |
| CI-06 | CI | CI: `trivy` + `grype` para CVEs em dependências | Média | CI-01 | Baixa | ⏳ Pendente | — |
| CI-07 | CI | CI: `lizard` com `-C 15 --error-exit-code-threshold 1` (CC ≤ 15) | Média | CI-01 | Baixa | ⏳ Pendente | — |
| AUD-01 | CONTRATO | Suite completa T1–T12 do `docs/TESTES.md` antes do primeiro release | Alta | CI-04, CI-05, CI-06 | Alta | ⏳ Pendente | — |
| AUD-02 | CONTRATO | Auditoria A1–A10 do `docs/TESTES.md` antes do primeiro release | Alta | AUD-01 | Alta | ⏳ Pendente | — |
| AUD-03 | CONTRATO | Empacotamento CPack DEB + AppImage testado em máquina limpa (sem KDE, sem Qt instalado) | Alta | AUD-02, TEMA-05 | Alta | ⏳ Pendente | — |
| AUD-04 | CONTRATO | Limpar warnings clang-tidy acumulados (readability-redundant-member-init em campos Qt; bugprone-reserved-identifier do GMock `using ::testing::_`; modernize-use-designated-initializers em testes; readability-convert-member-functions-to-static em ServicoSugerirAcao). Decidir entre NOLINT, suprimir checks no `.clang-tidy` ou refatorar antes de habilitar `CXX_CLANG_TIDY` no CMake | Média | DOM-09, APP-05 | Média | ⏳ Pendente | — |
| META-01 | META | `TODO.md` criado via skill `tab_pendencias` na raiz do projeto | — | — | Baixa | ✅ Concluído | ✓ |

## Convenções

- **Status**: ✅ Concluído | 🔄 Em andamento | 🟡 Parcial | ⏳ Pendente | 💡 Decisão tomada | 🎨 Pendente design | 🔍 Pendente verificação
- **Estado Auditado**: `—` não auditado | `✓` aprovado | `⚠` aprovado com ressalvas
- **Grupos**: `DESIGN` (brainstorming/decisões) · `BOOTSTRAP` (git/CMake/docs) · `TEMA` (Breeze) · `DOMAIN` · `APPLICATION` · `INFRASTRUCTURE` · `PRESENTATION` · `CI` · `CONTRATO` (auditoria) · `META` (housekeeping da própria tabela)
- Desvios explícitos do `docs/CONTRACT.md` MUST entrar como item `AUD-XX` com justificativa.
- Conformidade: tabela única, sem `PENDENCIAS.md`/`TAREFAS.md` paralelos.
