# Integração `vram_optimizer` → `my_comp` — Briefing Operacional

> **Para o Claude da próxima sessão rodando em `~/IDrive/Documentos/projetos_dev/my_comp/`:**
> Este arquivo é o ponto de entrada para absorver a feature `vram_optimizer` no my_comp.
> **Leia inteiro antes de tocar em código, CMakeLists ou TODO.**

Gerado em **2026-04-29** ao copiar a pasta `vram_optimizer/` (limpa) para dentro do my_comp.

---

## 1. Resumo em uma página

`vram_optimizer` é um auditor de uso de **VRAM/GPU em Linux**. Detecta processos consumindo VRAM, classifica candidatos a "rodar só em CPU" (ex.: navegadores com aceleração de hardware) e **localiza a configuração de origem** que fez cada app subir para a GPU (flag de aceleração, env var como `__NV_PRIME_RENDER_OFFLOAD`/`DRI_PRIME`, `.desktop` com wrapper `prime-run`/`optirun`, config de compositor, VAAPI/VDPAU, etc.).

Estado atual: **DES-03** — specs e Domain/Application parciais. Sem Infrastructure, sem Presentation, sem CLI.

Razão da incorporação: o my_comp já é um diagnóstico nativo de sistema Linux (CPU, memória, armazenamento, áudio, firmware, journal, serviços). VRAM/GPU é a próxima fronteira natural — e o `vram_optimizer` já trouxe o **modelo de domínio**, **regras de classificação em cascata** e **infraestrutura de localização de configs** (4 sublocators: Chromium, Firefox, env-var, desktop-wrapper) prontos. O que falta é Infrastructure (NVML/Intel sysfs) e Presentation (QML).

---

## 2. Decisões arquiteturais do usuário no momento desta integração

O usuário decidiu, **ao incorporar esta feature**, alinhar o my_comp ao seu padrão global de stack:

### 2.1 Stack — subir para C++23

| Aspecto | my_comp antes | my_comp agora (decisão) |
|---|---|---|
| Padrão C++ | C++20 | **C++23** |
| Toolchain | GCC/Clang atuais | GCC 13+ ou Clang 16+ |
| Razão | — | habilita `std::expected`, `std::print`, deducing `this`, `if consteval` — já usados/planejados no `vram_optimizer` |

**Ação**: ajustar `CMakeLists.txt` raiz do my_comp:
```cmake
set(CMAKE_CXX_STANDARD 23)   # antes: 20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

### 2.2 Arquitetura — 4 camadas com vocabulário `front / mid / back / foundation`

O my_comp hoje é **Hexagonal 3 camadas** (`Presentation / Domain / Infrastructure`). O usuário decidiu migrar para **4 camadas** com o vocabulário do `~/.claude/CLAUDE.md` global:

```
Layer 1 — Front          UI: QML/Kirigami (my_comp), Qt Widgets (vram), CLI argv
Layer 2 — Mid            Orquestração: ViewModels-controllers, services, casos de uso, glue
Layer 3 — Back           Domínio puro: entidades, value objects, interfaces (= 0)
Layer 4 — Foundation     Infra: providers Linux, D-Bus, NVML, sysfs, OpenSSL, persistência, build, CI
```

**Direção de dependência (única autorizada):**
```
Front  →  Mid  →  Back
                   ↑
        Foundation ┘  (implementa interfaces do Back)
```

**Diferença chave para o my_comp atual:** hoje os ViewModels (em `src/presentation/`) mesclam UI + orquestração. A migração separa: **QML/Kirigami fica em Front**, **ViewModels-controllers e services orquestradores vão para Mid**.

### 2.3 Tabela de mapeamento — vocabulários antigos × novo

| Vocabulário do `vram_optimizer` | Vocabulário do `my_comp` antigo | **Vocabulário oficial novo** | Diretório novo sugerido |
|---|---|---|---|
| `presentation/` | `presentation/` (QML + ViewModels misturados) | **`front/`** | `src/front/` (QML + views) |
| `application/` | (não existia separado) | **`mid/`** | `src/mid/` (services, orquestradores, ViewModels-controllers extraídos) |
| `domain/` | `domain/` | **`back/`** | `src/back/` (entidades, interfaces, valueobjects) |
| `infrastructure/` | `infrastructure/` | **`foundation/`** | `src/foundation/` (collectors, providers, persistência, cripto) |

> **Observação importante:** o `vram_optimizer` interno ainda usa "Presentation/Application/Domain/Infrastructure" no código e nos docs. **Não traduzir nomes de arquivos/classes** — eles são neutros de camada. O renomeio é só de **diretórios** e **vocabulário em CLAUDE.md/TODO/CONTRACT**.

---

## 3. O que está nesta pasta (`features/vram_optimizer/`)

Após a limpeza pré-cópia, restou:

```
features/vram_optimizer/
├── CLAUDE.md                  ← regras específicas DA FEATURE (não substitui o CLAUDE.md raiz do my_comp)
├── README.md                  ← visão geral da feature
├── TODO.md                    ← 46 itens próprios (DES-* / BOOT-* / TEMA-* / DOM-* / APP-* / INF-* / PRE-* / CLI-* / TEST-* / DOC-*)
├── INTEGRACAO_MY_COMP.md      ← este arquivo
├── CMakeLists.txt             ← do projeto standalone — precisa ser desfeito/incorporado
├── cmake/                     ← módulos CMake auxiliares (CompilerFlags, DependenciasTeste)
├── .clang-format              ← compatível com C++23
├── .clang-tidy                ← compatível com C++23
├── .gitignore                 ← do projeto standalone — descartar
├── docs/
│   ├── CONTRACT.md            ← 1.700+ linhas, autoridade RFC 2119 (multi-linguagem)
│   └── TESTES.md              ← T1–T12 (testes) e A1–A10 (auditoria)
├── src/                       ← Domain + Application parciais (ver §4)
└── tests/                     ← GTest+GMock cobrindo Domain + Application
```

**Já apagado da pasta original (não veio na cópia):**
- `build/`, `build_rev/`, `.cache/`, `compile_commands.json` (artefatos)
- `.git/` (histórico salvo em `/tmp/vram_optimizer_git_log_backup.txt` na máquina do usuário, 46 commits)
- `docs/cpp_qt6_*.md` (9 docs genéricos C++23/Qt6/MySQL — duplicariam com material do my_comp)
- `docs/superpowers/` (artefatos internos de skill)
- `CMakePresets.json` (incompatível com presets do my_comp)

---

## 4. Estado real da implementação no `src/` e `tests/`

### 4.1 Back (atualmente em `src/domain/`) — pronto

| Tipo | Itens |
|---|---|
| **Entidades** | `Processo`, `GpuInfo`, `OrigemConfig`, `EntradaCatalogo`, `EntradaFeedback` |
| **Value Objects** | `PID`, `BytesDeMemoria`, `NomeProcesso`, `ChaveProcesso` |
| **Structs/auxiliares** | `EntradaDesktopExec`, `AmostraJanela`, `LimiaresComportamento`, `Categoria`, `Justificativa`, `ResultadoClassificacao` |
| **Interfaces (puras, `= 0`)** | `IClassificadorProcessos`, `IConfigLocator`, `IRegraClassificacao`, `ICatalogoClassificacao`, `IRepositorioFeedback`, `ILeitorDesktopFile`, `ILeitorMapsProcesso`, `IAmostradorComportamento`, `ILeitorEnviron`, `ILeitorArquivoConfig`, `ILeitorDesktopExec` |
| **Pendente em Back** | `IGpuBackend` (DOM-05) |

### 4.2 Mid (atualmente em `src/application/`) — parcial

**Configuração (DES-02, completo):**
- `LocalizadorComposite` (orquestrador) + 4 sublocators:
  - `LocalizadorBrowserChromium` (Chrome/Brave/Edge — Local State + Preferences)
  - `LocalizadorFirefox` (`prefs.js` / `user.js`)
  - `LocalizadorEnvVar` (`__NV_PRIME_RENDER_OFFLOAD`, `DRI_PRIME`, `LIBVA_DRIVER_NAME`, etc.)
  - `LocalizadorDesktopWrapper` (`.desktop` com `Exec=prime-run …` ou `optirun`)
- `ServicoLocalizarConfig` (fachada) — testes de integração com sublocators reais

**Classificação (DES-01, completo):**
- `ClassificadorEmCascata` orquestra 5 regras:
  - `RegraFeedback` (memória do que o usuário já decidiu)
  - `RegraCatalogo` (whitelist conhecida)
  - `RegraCategoriaDesktop` (heurística por categoria do `.desktop`)
  - `RegraBibliotecaCompute` (detecta CUDA/OpenCL/Vulkan compute em `/proc/<pid>/maps`)
  - `RegraComportamento` (amostragem de janela de uso real de VRAM)
- `ServicoClassificarProcesso` (fachada)
- `ServicoSugerirAcao` (mapeia `OrigemConfig` → instrução humana)

**Pendente em Mid:**
- `ServicoAuditarVramAtual` (orquestra `IGpuBackend`) — APP-01

### 4.3 Foundation (pendente — toda)

Implementações concretas das interfaces, **NADA codificado ainda**:
- `NvmlGpuBackend` (NVML / `libnvidia-ml`) — INF-01
- `IntelI915GpuBackend` (sysfs `/sys/class/drm/card*/`, Level Zero) — INF-02
- Factory `IGpuBackend` com detecção runtime — INF-03
- `ChromiumConfigParser`, `FirefoxConfigParser`, `DesktopFileParser`, `EnvironParser` — INF-04..07
- Persistência opcional (`SqliteRepositorio` ou nada se ficar no my_comp atual) — INF-08

### 4.4 Front (pendente — toda)

- GUI Qt original era Widgets + tema Breeze. **Aqui muda**: no my_comp é **QML/Kirigami**. Reescrever Presentation em QML respeitando as ViewModels que serão criadas em Mid.
- CLI: `vram_optimizer_cli` planejada como target separado. Repensar se vira subcomando do my_comp ou módulo independente.

### 4.5 Tests

44 arquivos de teste em `tests/` cobrindo Domain (Back) e Application (Mid). **GTest + GMock** — compatível com a stack do my_comp (que já usa GoogleTest + CTest).

Mocks prontos em `tests/application/mocks/` para todas as 11 interfaces do Domain.

---

## 5. Plano sugerido de adaptação (4 fases)

> **Não execute sem alinhar com o usuário.** Use isto como base para um plano formal via `superpowers:writing-plans`.

### Fase A — Reconciliação estrutural (sem código novo)

1. Mover diretórios: `src/domain/` → `src/back/`, `src/application/` → `src/mid/`. Manter sub-estrutura.
2. Atualizar todos os `#include` afetados (sed/IDE refactor).
3. Atualizar `CMakeLists.txt` da feature: targets `vram_back` e `vram_mid` (libs internas).
4. Integrar ao `CMakeLists.txt` raiz do my_comp via `add_subdirectory(features/vram_optimizer)`.
5. Verificar build verde com C++23 e `-Werror`.
6. Rodar suíte de testes — todos devem passar sem alteração de lógica.

### Fase B — Subir my_comp inteiro para C++23 + 4 camadas

1. `set(CMAKE_CXX_STANDARD 23)` no `CMakeLists.txt` raiz.
2. Renomear `src/presentation/` → `src/front/`, `src/domain/` → `src/back/`, `src/infrastructure/` → `src/foundation/`.
3. **Extrair Mid** do que hoje está em `src/presentation/` (ViewModels que orquestram providers) ou `src/domain/` (services-fachada): mover para `src/mid/`.
4. Atualizar `CLAUDE.md` raiz do my_comp para refletir 4 camadas + C++23.
5. Atualizar `README.md` (seção "Arquitetura").

### Fase C — Implementar Foundation da feature

1. INF-01: `NvmlGpuBackend` em `src/foundation/gpu/nvml_gpu_backend.cpp` — implementa `IGpuBackend`. TDD com mock em teste de integração que pula se NVML não disponível.
2. INF-02: `IntelI915GpuBackend` — sysfs. TDD com fixture de filesystem fake.
3. INF-03: Factory de GPU com detecção runtime (presença de `libnvidia-ml.so.1` e/ou `/sys/class/drm/card*/device/vendor == 0x8086`).
4. INF-04..07: parsers de config (Chromium/Firefox/Desktop/Environ). TDD com fixtures.

### Fase D — Front em QML/Kirigami

1. Definir telas: lista de processos com VRAM por GPU, classificação por cor, painel de "configs encontradas e como mudar".
2. ViewModels em `src/mid/viewmodels/` (Q_OBJECT, propriedades NOTIFY) consumidos pelo QML.
3. Integrar ao dashboard do my_comp (tab/aba "VRAM").
4. CLI: avaliar se vira subcomando `my_comp vram` ou se permanece binário separado `vram_optimizer_cli`.

---

## 6. Reconciliação de contratos e docs

### 6.1 CLAUDE.md

- **`my_comp/CLAUDE.md`** (raiz) **continua sendo a autoridade do projeto inteiro**. Atualizá-lo para refletir C++23 + 4 camadas (front/mid/back/foundation).
- **`my_comp/features/vram_optimizer/CLAUDE.md`** vira **CLAUDE.md de subfeature** — é hierárquico, o Claude carrega ambos. Editar para remover referências a "Presentation/Application/Domain/Infrastructure" e usar "Front/Mid/Back/Foundation". Manter o que é específico (heurísticas de classificação, anti-patterns de detecção GPU).

### 6.2 docs/CONTRACT.md e docs/TESTES.md

O my_comp não tem `docs/`. Duas opções (decidir com o usuário):

**Opção (a)**: criar `my_comp/docs/CONTRACT.md` e `my_comp/docs/TESTES.md` na raiz do my_comp, **ascendendo** os arquivos do vram_optimizer. Adaptar referências:
- Em `CONTRACT.md`: atualizar §5 "Architecture Layers" para usar **Front/Mid/Back/Foundation** em vez de Presentation/Application/Domain/Infrastructure.
- Em `TESTES.md`: ajustar caminhos de exemplo (`src/back/`, `src/mid/`, etc.).
- **Vantagem**: my_comp passa a ter contrato formal RFC 2119 (hoje só tem CLAUDE.md condensado).

**Opção (b)**: manter `CONTRACT.md` e `TESTES.md` confinados em `features/vram_optimizer/docs/`. **Não recomendado** — fragmenta autoridade.

→ Recomendação: **(a)**, com renomeação interna de camadas em ambos os arquivos.

### 6.3 TODO.md

- O `my_comp/TODO.md` raiz tem 93 KB e 43+ itens. Adicionar 46 da feature lá faria explodir.
- Recomendação: **manter `features/vram_optimizer/TODO.md` como tabela local da feature** (exceção controlada à regra "único TODO na raiz"). Adicionar **um único item agregador** no `my_comp/TODO.md` raiz:
  ```
  | VRAM-FEATURE | FEATURE | Integrar feature vram_optimizer (ver features/vram_optimizer/TODO.md) | Alta | — | Alta | 🟡 Em integração | — |
  ```
- Quando a feature estiver beta-pronta, condensar em ~5 itens no TODO raiz e arquivar o local.

### 6.4 .clang-format e .clang-tidy

A feature traz seus arquivos. Comparar com os do my_comp:
- Se coincidirem: apagar os da feature.
- Se divergirem: reconciliar para um único par na raiz, alinhado a C++23 + Qt6 + snake_case de arquivos.

### 6.5 cmake/CompilerFlags.cmake e cmake/DependenciasTeste.cmake

- Feature traz módulos auxiliares. my_comp tem suas flags inline no `CMakeLists.txt`.
- Recomendação: extrair as flags do my_comp para `cmake/CompilerFlags.cmake` (DRY!) e usar o mesmo módulo para a feature. Idem `DependenciasTeste.cmake`.

---

## 7. Snapshot das memórias persistentes do `vram_optimizer`

> Estas 9 memórias estavam em `~/.claude/projects/-home-petrus-...-vram-optimizer/memory/`.
> **Não vêm com a cópia da pasta** (memória do Claude vive fora do projeto).
> Migrar para `~/.claude/projects/-home-petrus-...-my-comp/memory/` ou consolidar nas memórias existentes do my_comp.

### 7.1 `project_escopo_vram.md` (tipo: project)

Auditor de VRAM/GPU em Linux. Detecta processos com VRAM, classifica candidatos a CPU-only, localiza origem da config (flag, env, .desktop, compositor). Stack: C++23, Qt 6.x, MySQL condicional (atrás de DIP), CMake ≥ 3.21. GPUs MVP: NVIDIA (NVML) + Intel (sysfs i915 / Level Zero); AMD via extensão `IGpuBackend`. Targets `vram_optimizer_gui` + `vram_optimizer_cli` com Application/Domain compartilhados. **Adaptação ao my_comp**: GUI vira QML/Kirigami integrada; CLI a decidir.

### 7.2 `project_contrato_resumo_vram.md` (tipo: project)

Síntese MUST/MUST NOT do `docs/CONTRACT.md`: encapsulamento privado, DIP via `shared_ptr<IInterface>`, herança ≤ 3, função ≤ 40 linhas / ≤ 4 params / ≤ 3 níveis aninhamento, smart pointers obrigatórios, nada de `new`/`delete` raw, `nullptr` em vez de `NULL`, `static_cast` em vez de C-cast, `connect` tipado, `prepare()` + `bindValue()` em 100% das queries SQL, `tr()` em toda string visível, QSS centralizado (Breeze), zero PII em logs, Conventional Commits, build com zero warnings.

### 7.3 `project_arquitetura_4camadas_vram.md` (tipo: project) — **OBSOLETA, atualizar**

Era: "Presentation / Application / Domain / Infrastructure". **Substituir por**: **"Front / Mid / Back / Foundation"** (decisão 2026-04-29 pelo usuário). Direção de dependência: `Front → Mid → Back ← Foundation`. Back puro (zero deps externas além de `QString`/`QVector`/`QDateTime`).

### 7.4 `project_padroes_pt_br.md` (tipo: project)

`QLocale::setDefault(QLocale::Portuguese, QLocale::Brazil)`. Decimal vírgula, milhar ponto, data `dd/MM/yyyy`, hora 24h. Toda string visível em `tr()`. Charset `utf8mb4` em DB e conexão Qt. Comentários e logs em pt-br com acentuação completa. Identificadores: inglês para tipos do domínio técnico, pt-br para conceitos de UI/usuário.

### 7.5 `project_qualidade_metricas.md` (tipo: project)

Função ≤ 40 linhas, ≤ 4 params, ≤ 3 níveis. Classe ≤ 300 linhas, herança ≤ 3. CC ≤ 15 (lizard bloqueia). Cobertura: Back ≥ 90 %, Mid ≥ 80 %, Foundation ≥ 70 %, gate global ≥ 60 %. Build estrito: `-std=c++23 -Wall -Wextra -Wpedantic -Werror -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -D_FORTIFY_SOURCE=3 -fstack-protector-strong -fPIE` + linker `-pie -Wl,-z,relro -Wl,-z,now`. ASan+UBSan obrigatórios em Debug/CI.

### 7.6 `project_ferramentas_qa.md` (tipo: project)

CMake ≥ 3.21 + Ninja + ccache + GCC 13+/Clang 16+. Estática: clang-tidy, cppcheck, flawfinder, iwyu, lizard, CodeQL. Dinâmica: ASan, UBSan, TSan, LSan, Valgrind, Heaptrack, perf. Testes: GoogleTest + GoogleMock + QtTest + ctest + lcov/gcovr. Segurança: gitleaks, trufflehog, trivy, grype, OSV.dev. Específicas VRAM: NVML, `nvidia-smi --query-compute-apps`, sysfs DRM, Level Zero, `intel_gpu_top`, `/proc/<pid>/maps`, `lsof /dev/dri/*`.

### 7.7 `feedback_tema_breeze.md` (tipo: feedback) — **CRUZADO COM my_comp**

Toda UI usa **tema Breeze/KDE como default, embarcado em `.qrc`**, sem dependência de KDE Plasma / `kf5` / `kf6` / pacotes do sistema. Em máquina sem KDE, app deve abrir já com aparência Breeze. Aplicar paleta + QSS antes de criar widgets. Nunca `setStyleSheet` com cores hardcoded em widget individual. **No my_comp (QML/Kirigami)**: equivalente é Breeze embarcado via `qt_add_resources` carregado em QML pelo `Theme` global. Vale para todo app pt-br do usuário.

### 7.8 `project_dry_principio.md` (tipo: project)

Regra dos 3: 1ª escreve, 2ª tolera, 3ª extrai. Distinguir duplicação real (mesma razão de mudar) de coincidência. Ao extrair, **nomear a razão de mudança** explicitamente — se não souber nomear, abstração é prematura. Anti-patterns: extrair na 2ª, esconder semânticas distintas atrás de flag, helper com `if (modo == "a")` (duas funções separadas seriam mais claras).

### 7.9 `reference_docs_vram.md` (tipo: reference) — **PARCIALMENTE OBSOLETA**

Era: índice dos 11 docs em `docs/` (CONTRACT, TESTES + 9 cpp_qt6_*). **Após limpeza**: só `CONTRACT.md` e `TESTES.md` vieram na cópia. Os 9 `cpp_qt6_*.md` foram apagados (genéricos, conflitavam com C++20 do my_comp e duplicariam com material do projeto).

---

## 8. Restrições não-negociáveis (do `~/.claude/CLAUDE.md` global do usuário)

Estas valem para **todo código produzido neste projeto**, sem exceção:

- **Idioma pt-br** em UI, logs, comentários, commit messages — acentuação completa sempre, nunca substituir `não`/`função`/`índice` pelo equivalente ASCII.
- **Conventional Commits**: `feat:`, `fix:`, `refactor:`, `chore:`, `docs:`, `test:`, `perf:`, `style:`, `revert:`. Mensagens em pt-br.
- **`TODO.md` único na raiz do projeto**: nunca `PENDENCIAS.md`/`TAREFAS.md`. (Exceção controlada: `features/vram_optimizer/TODO.md` durante a integração, conforme §6.3.)
- **TDD obrigatório** em Back (Domain) e Mid (Application): Red → Green → Refactor. Bug fix começa por teste vermelho que reproduz.
- **Nomenclatura de arquivos: `snake_case`** (ex.: `gerenciador_vram.cpp`, não `GerenciadorVram.cpp`).
- **Tema Breeze embarcado** (§7.7).
- **Stack default: C++23 + Qt 6.x**.
- **Sem `--no-verify`** sem pedido explícito. Sem `git push --force` em main. Sem `--amend` em commits publicados.
- **Sem criar arquivos `.md` de planejamento/análise intermediária** sem pedido explícito.
- **Sem emojis** em código ou docs sem pedido explícito.

---

## 9. Primeiros comandos sugeridos para o Claude da próxima sessão

```bash
# 1. Localizar-se
cd /home/petrus/IDrive/Documentos/projetos_dev/my_comp
ls -la
git status
git log --oneline -10

# 2. Confirmar que a feature foi colada
ls -la features/vram_optimizer/
cat features/vram_optimizer/INTEGRACAO_MY_COMP.md  # este arquivo

# 3. Ler autoridade do my_comp
cat CLAUDE.md
head -80 TODO.md  # ver itens ativos

# 4. Ler autoridade da feature
cat features/vram_optimizer/CLAUDE.md
cat features/vram_optimizer/docs/CONTRACT.md | head -100
cat features/vram_optimizer/docs/TESTES.md | head -50

# 5. Inspecionar o código já feito
find features/vram_optimizer/src -type f -name "*.h" | head -20
find features/vram_optimizer/tests -type f | wc -l   # ~44 arquivos de teste

# 6. Conferir build atual do my_comp (sanity check antes de mexer)
cmake -B build -G Ninja
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure && cd ..
```

---

## 10. Ordem de trabalho recomendada (resumo executivo)

1. **Ler este arquivo, `my_comp/CLAUDE.md` e `features/vram_optimizer/CLAUDE.md`.** Confirmar entendimento com o usuário antes de tocar em código.
2. **Confirmar com o usuário** três decisões abertas:
   - (a) `docs/CONTRACT.md` e `docs/TESTES.md` sobem para `my_comp/docs/` (Opção 6.2.a)?
   - (b) `TODO.md` da feature fica local + entrada agregadora no raiz (Opção §6.3)?
   - (c) CLI vira `my_comp vram` ou permanece `vram_optimizer_cli` separado?
3. **Plano formal** via `superpowers:writing-plans` cobrindo Fases A→D.
4. **Fase A** (reconciliação estrutural sem código novo) — pode iniciar autonomamente após confirmação dos pontos 2(a–c).
5. **Cada fase termina com:** build C++23 verde + zero warnings + todos os testes passando + commit Conventional + atualização do TODO da feature.

---

## Apêndice — Inventário do TODO.md da feature (resumo por grupo)

| Grupo | Total | Concluídos | Pendentes |
|---|---|---|---|
| DESIGN (DES-*) | 5 | 2 | 3 |
| BOOTSTRAP (BOOT-*) | 8 | 4 | 4 (precisam refazer no contexto my_comp) |
| TEMA (TEMA-*) | 5 | 0 | 5 (Breeze embarcado — possivelmente já feito no my_comp) |
| BACK / DOMAIN (DOM-*) | 10 | 9 | 1 (`IGpuBackend`) |
| MID / APPLICATION (APP-*) | 7 | 6 | 1 (`ServicoAuditarVramAtual`) |
| FOUNDATION / INFRA (INF-*) | 8 | 0 | 8 (NvmlGpuBackend, Intel sysfs, parsers, factory, persistência) |
| FRONT / PRESENTATION (PRE-*) | 2 | 0 | 2 (precisa reescrever em QML) |
| CLI | n/a | 0 | a definir (subcomando do my_comp ou binário separado) |
| TESTES | n/a | 44 arquivos | crescer com Foundation |

Total estimado de trabalho restante após Fase A: **~6–8 sprints** dependendo de cobertura de GPUs e decisão sobre persistência.

---

**Fim do briefing.** Em caso de dúvida sobre prioridade, fazer apenas o que está em §9 e parar para alinhar com o usuário.
