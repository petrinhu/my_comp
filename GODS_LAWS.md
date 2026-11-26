# GODS_LAWS.md: my_comp

> Ordens do líder para este projeto. Cada lei tem um gatilho, conferido no momento da ação.

## Protocolo de uso

1. Antes de agir, confira o índice de gatilhos do `CLAUDE.md` deste projeto; se um gatilho casa, leia a lei inteira antes do primeiro comando.
2. Siga o protocolo de uso do `~/.claude/GODS_LAWS.md`. No que este arquivo silencia, vale a lei global; `L-NN` sem prefixo é lei global e `MC-NN` é lei daqui.

---

## MC-01

**Data:** regra permanente.

Em conflito técnico, siga esta ordem de autoridade:

1. este `GODS_LAWS.md` e o `CLAUDE.md`: regras específicas do projeto e decisões de arquitetura;
2. [`docs/CONTRACT.md`](docs/CONTRACT.md): princípios universais de OOP/SOLID/Clean Code/segurança em RFC 2119 (MUST/SHOULD/MAY), genérico para C++/Qt6/MySQL, aplicado ao my_comp na parte C++23 + Qt6;
3. [`docs/TESTES.md`](docs/TESTES.md): procedimentos T1–T12 (testes) e A1–A10 (auditoria) com comandos exatos; define os gates de qualidade;
4. [`docs/AUDITORIAS.md`](docs/AUDITORIAS.md): checklists operacionais detalhados por bloco (segurança, arquitetura, C++23, Qt6, performance, devops, qualidade); use como referência ao auditar.

Abra esses docs nos checkpoints certos, porque não basta eles existirem. Antes de marcar uma tarefa do TODO como ✅ concluída ou de propor merge, **consulte e cite** as seções aplicáveis aos arquivos tocados:

| Gatilho | Consultar |
| :--- | :--- |
| Mexeu em **camada cruzada** (Presentation ↔ Application ↔ Domain ↔ Infrastructure) | CONTRACT §5 (Architecture Layers) + AUDITORIAS §I2 (4 Camadas) — reverificar que dependência é unidirecional, interfaces respeitadas |
| Adicionou ou mudou **memória dinâmica / RAII / smart pointers / threads** | CONTRACT §8 (Security) + AUDITORIAS §I3 (Memory Safety) + §I4 (C++23 Moderno) |
| Adicionou ou mudou **UI / QML / ViewModel** | CONTRACT §13.1 (Qt6) + AUDITORIAS §I5 (Qt6 Específico) — checar `tr()`, `connect` tipado, lambdas com contexto, parent ownership |
| Adicionou ou mudou **escrita/leitura em arquivo, processo externo, parsing de input não-confiável** | CONTRACT §8 + AUDITORIAS §I3 (validação de input, secrets, LGPD) + TESTES §T3 (fuzzing se for parser) |
| Adicionou ou mudou **testes** | TESTES §T1 (unit) + §T2 (estática) + §A6 (cobertura) — confirmar que o teste falha sem a implementação (red), não só passa por acidente |
| Antes de **fechar uma feature ou abrir merge** | Rodar `scripts/ci.sh` (que cobre T1+T2+T4 localmente) e validar contra os gates da MC-07 |

Cite a cláusula na mensagem de commit ou no comentário do PR (ex.: `aplica AUDITORIAS §I3.2: std::unique_ptr no lugar de new manual em XYZ`), porque isso torna a verificação explícita em vez de implícita.

## MC-02

**Data:** regra permanente.

Mantenha as 4 camadas:

```
Presentation     src/presentation/    ViewModels (Q_OBJECT) + QML/Kirigami
        │ depende ↓
Application      src/application/     Use cases / services (orquestração)
        │ depende ↓
Domain           src/domain/          Entidades + interfaces IPascalCase (ports)
        ↑ implementa
Infrastructure   src/infrastructure/  Collectors/Adapters Linux, DBus, OpenSSL, QtAudio, Exporters
```

Ponha os subnamespaces de feature dentro da camada (ex.: `src/domain/vram/`, `src/application/vram/`).

- **Dependência em sentido único:** Presentation → Application → Domain ← Infrastructure. O Domain não importa Qt GUI, nem OpenSSL, nem processo externo.
- **Interface por fronteira:** faça toda comunicação que cruza camada passar por interface abstrata em `src/domain/interfaces/` (ou `src/domain/<feature>/interfaces/`).
- **DI manual** em `src/main.cpp`, sem service locator. Providers concretos só aparecem na camada `infrastructure`.
- **Violações proibidas:** `QProcess`/`QNetworkReply`/SQL/`QFile` dentro de viewmodel ou domínio; lógica de negócio dentro de provider; acesso a hardware dentro de QML.

## MC-03

**Data:** regra permanente.

Nomeie assim:

| Tipo | Convenção | Exemplo |
| :--- | :--- | :--- |
| Classe/Struct | PascalCase | `MemoryMonitor` |
| Função/Variável | snake_case | `read_cpu_stats` |
| Interface (abstrata) | `IPascalCase` | `ICpuProvider` |
| Constante | `SCREAMING_SNAKE_CASE` | `MAX_RETRY_COUNT` |
| Membro privado | `m_` + snake_case | `m_cache` |
| Override virtual | `override` obrigatório | — |

**Exceção — API exposta ao QML:** `Q_INVOKABLE`, getters/setters de `Q_PROPERTY` e signals usam camelCase (`startInstallation`, `isBusyChanged`), porque é a convenção do lado QML. A exceção vale só para essa superfície; o resto do C++ segue a tabela. O `.clang-tidy` não distingue o que é exposto ao QML, então o uso fora dela se fiscaliza em revisão. O código VRAM em camelCase pt-br (`criarBackendCompositoDoSistema`) é dívida a renomear em ondas (ver `TODO.md`).

Refatore toda classe com mais de uma responsabilidade (SRP). Trate as 300 linhas por arquivo como orientação, que pede exame e não obriga a dividir (L-34).

## MC-04

**Data:** regra permanente.

- **Obrigatório:** `std::unique_ptr`/`std::shared_ptr` para ownership; `QPointer` para observação de QObject.
- **Proibido:** `new`/`delete` manual em código de aplicação, raw owning pointer, `NULL` (use `nullptr`) e cast `(Type*)` (use `static_cast`).
- **RAII sempre:** arquivos, mutex, sockets e handles, nunca `close()` manual.
- Não faça I/O, network nem DB no construtor; use um método `initialize()` quando precisar.

## MC-05

**Data:** regra permanente.

- Ponha `Q_OBJECT` em toda classe que usa signals/slots.
- Use `connect()` com sintaxe tipada (ponteiro-para-membro), nunca string-based.
- **Threads:** rode trabalho pesado em `QThread`/`QtConcurrent` e nunca bloqueie a thread de UI. Cross-thread → `Qt::QueuedConnection`.
- **libc não-reentrante** (`getpwuid`, `getgrgid` etc.): use as variantes `_r` com mutex.
- Use **parent/child ownership** para widgets e ponha os recursos QML em `.qrc`.
- **Tema:** sem `setStyleSheet` inline com cores fixas; use a paleta central (Breeze Dark/Light).

## MC-06

**Data:** regra permanente.

Siga o fluxo TDD/ATDD:

1. **Red:** teste falhando primeiro (ATDD na feature, unit por componente).
2. **Green:** implementação mínima.
3. **Refactor:** conformidade com MC-03, MC-04 e MC-05.
4. **Audit:** `clang-tidy` + `cppcheck` + `valgrind` sem regressão, dentro do pré-CI seletivo (`scripts/ci.sh`, L-35).

## MC-07

**Data:** regra permanente.

Passe nestes gates de qualidade, **MUST PASS para Beta**:

| Gate | Ferramenta | Critério |
| :--- | :--- | :--- |
| Build | cmake/ninja | Zero erros, zero novos warnings |
| T1 Unit | GTest | 100% dos testes passando |
| T2 Estática | clang-tidy + cppcheck | Zero `bugprone-*`/`security-*`/`cert-*` |
| T4 Dinâmica | Valgrind / ASan / UBSan | Zero *definitely lost*, zero ERROR SUMMARY |
| T5/T12 CVEs | trivy ou grype | Zero CRITICAL em dependências |
| T8 Secrets | gitleaks | Zero detectados no histórico completo |
| A2 Camadas | grep + revisão | Zero violação (Qt GUI em domínio, SQL em widget, etc.) |
| A6 Cobertura | lcov (`--ignore-errors mismatch,source,gcov`) | Medida, sem meta numérica (L-35) |
| A3 UI/UX | contraste + Tab/Enter | WCAG AA (≥4,5:1 normal / ≥3:1 grande) |

## MC-08

**Data:** regra permanente.

Depois de cada feature:

1. Com o portão verde e a autorização explícita do líder no contexto (L-24), rode `./deploy.sh "<conventional commit>"`: build + commit (opcional) + **release semântica automática** (bump SemVer + atualiza `CHANGELOG.md` + cria tag anotada via `git-cliff`) + `git push --follow-tags`. O `git push` direto é bloqueado pelo hook `scripts/hooks/pre-push`.
2. Atualize o `TODO.md` (mova o ID do item entre as colunas Status/Estado Auditado).
3. Pergunte ao usuário: **"Como deseja prosseguir?"**.

Não crie SESSION.md: o histórico vive no git log + `TODO.md` + `CHANGELOG.md`.

## MC-09

**Data:** regra permanente.

Ponha entre parênteses o escopo, que referencia o módulo (`feat(audio): …`, `fix(process): …`), e escreva mensagens curtas, imperativas, focadas em *por quê*.

## MC-10

**Data:** regra permanente.

Segurança Zero-Cloud:

- **100% offline:** nenhuma telemetria nem upload.
- Faça a persistência sensível via `ISecurityVault` (AES-256-GCM + IV + auth tag).
- **Proibido:** segredos hardcoded, string concat em SQL, `eval`/`exec` com input externo.
- Valide toda entrada externa (QProcess output, DBus payload, arquivos de `/sys`, `/proc`).

## MC-11

**Data:** regra permanente.

- Use como remoto o GitHub, `git@github.com:petrinhu/my_comp.git` (L-29).
- `main` é a branch principal: nunca faça force push nela. Trabalhe em branch `feat/…` ou `fix/…` e leve o trabalho ao `main` só com a onda fechada e o OK do líder (L-24).

## MC-12

**Data:** regra permanente.

- Versione por [SemVer 2.0](https://semver.org/spec/v2.0.0.html) com pré-release. Baseline: `v0.1.0-alpha.0`. Pré-1.0 = APIs ainda instáveis.
- O próximo release é calculado automaticamente pelo `git-cliff` (`cliff.toml`) a partir dos commits Conventional desde o último tag (regras conferidas empiricamente com git-cliff 2.14):
  - **Em pré-release** (`-alpha.N`, `-beta.N`, `-rc.N`): qualquer commit releasável (`feat:`, `fix:`, `refactor:`, `perf:`, `docs:`) só incrementa o contador (`v0.1.0-alpha.4` → `v0.1.0-alpha.5`). Sair do pré-release ou mudar de estágio é decisão consciente, não automática.
  - **Em `0.x` estável:** `feat:` e `fix:` sobem PATCH (`features_always_bump_minor = false`); `BREAKING CHANGE:` sobe MAJOR (`breaking_always_bump_major = true`, zera pré-release).
  - **A partir de `1.0.0`:** `feat:` → MINOR, `fix:`/`refactor:`/`perf:`/`docs:` → PATCH, `BREAKING CHANGE:` → MAJOR.
- O marco `BT-04 Release Beta v2.0` cortará `v1.0.0-beta.0` (decisão consciente, não automática).
- O **codename "v2.0"** no nome do projeto (README, CLAUDE.md) é label histórica (segunda geração pós-bash legacy v0.3.6), independente do SemVer real.
- O arquivo `VERSION` na raiz reflete a versão **atual** (do último release feito).
- O `CHANGELOG.md` segue Keep a Changelog, gerado e atualizado pelo `git-cliff` a cada release. Não o edite manualmente acima da linha de marcador.

## MC-13

**Data:** regra permanente.

Não modifique o legado (script Bash v0.3.6) em `./legacy/`: ele fica lá como referência histórica.
