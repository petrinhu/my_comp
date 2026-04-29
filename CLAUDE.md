# CLAUDE.md — Contrato de Engenharia: my_comp v2.0

Fonte única de verdade para o desenvolvimento do **my_comp** (evolução do script legado em `./legacy/` para aplicação nativa C++23/Qt6).

## 0. Documentos normativos

Hierarquia de autoridade técnica. Em conflito, a ordem abaixo prevalece:

1. **Este `CLAUDE.md`** — regras específicas do projeto + decisões de arquitetura.
2. **[`docs/CONTRACT.md`](docs/CONTRACT.md)** — princípios universais de OOP/SOLID/Clean Code/segurança em RFC 2119 (MUST/SHOULD/MAY). Genérico para C++/Qt6/MySQL; aplica-se ao my_comp na parte C++23 + Qt6.
3. **[`docs/TESTES.md`](docs/TESTES.md)** — procedimentos T1–T12 (testes) e A1–A10 (auditoria) com comandos exatos. Define os gates de qualidade.
4. **[`docs/AUDITORIAS.md`](docs/AUDITORIAS.md)** — checklists operacionais detalhados por bloco (segurança, arquitetura, C++23, Qt6, performance, devops, qualidade). Use como referência ao auditar.

## 1. Stack & ambiente

- **SO alvo:** Fedora 43 + KDE Plasma 6 + Wayland nativo (design agnóstico a distros; multi-PM detectado em runtime).
- **Linguagem:** C++23 estrito (flags `-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -Wnull-dereference -fstack-protector-strong -D_FORTIFY_SOURCE=3` globais; `-Werror` em adoção incremental — hoje 491 warnings rastreados como dívida técnica).
- **Framework:** Qt6 — `Core`, `Gui`, `Quick`, `QuickControls2`, `WaylandClient`, `Multimedia`, `DBus`, `Network`, `Concurrent`, `Test`.
- **Build:** CMake ≥ 3.16 + Ninja.
- **Testes:** GoogleTest + CTest.
- **Cripto:** OpenSSL (AES-256-GCM).
- **Repositório:** `ssh://git@codeberg.org/petrinhu/my_comp.git`.

## 2. Arquitetura — 4 camadas

```
Presentation     src/presentation/    ViewModels (Q_OBJECT) + QML/Kirigami
        │ depende ↓
Application      src/application/     Use cases / services (orquestração)
        │ depende ↓
Domain           src/domain/          Entidades + interfaces IPascalCase (ports)
        ↑ implementa
Infrastructure   src/infrastructure/  Collectors/Adapters Linux, DBus, OpenSSL, QtAudio, Exporters
```

Subnamespaces de feature ficam dentro da camada (ex.: `src/domain/vram/`, `src/application/vram/`).

Regras obrigatórias:
- **Dependência em sentido único**: Presentation → Application → Domain ← Infrastructure. Domain não importa Qt GUI, nem OpenSSL, nem processo externo.
- **Interface por fronteira**: toda comunicação cruzando camada passa por interface abstrata em `src/domain/interfaces/` (ou `src/domain/<feature>/interfaces/`).
- **DI manual** em `src/main.cpp` (sem service locator). Providers concretos só aparecem na camada `infrastructure`.
- **Violações proibidas**: `QProcess`/`QNetworkReply`/SQL/`QFile` dentro de viewmodel ou domínio; lógica de negócio dentro de provider; acesso a hardware dentro de QML.

## 3. Estilo e nomenclatura

| Tipo | Convenção | Exemplo |
| :--- | :--- | :--- |
| Classe/Struct | PascalCase | `MemoryMonitor` |
| Função/Variável | snake_case | `read_cpu_stats` |
| Interface (abstrata) | `IPascalCase` | `ICpuProvider` |
| Constante | `SCREAMING_SNAKE_CASE` | `MAX_RETRY_COUNT` |
| Membro privado | `m_` + snake_case | `m_cache` |
| Override virtual | `override` obrigatório | — |

Arquivos com mais de 300 linhas ou classes com >1 responsabilidade → refatorar (SRP).

## 4. Memory safety & RAII

- **Obrigatório**: `std::unique_ptr`/`std::shared_ptr` para ownership. `QPointer` para observação de QObject.
- **Proibido**: `new`/`delete` manual em código de aplicação, raw owning pointer, `NULL` (usar `nullptr`), `(Type*)` cast (usar `static_cast`).
- **RAII sempre**: arquivos, mutex, sockets, handles — nunca `close()` manual.
- Construtor **não** faz I/O/network/DB; usar método `initialize()` quando precisar.

## 5. Qt & QML

- `Q_OBJECT` em toda classe que usa signals/slots.
- `connect()` com sintaxe tipada (ponteiro-para-membro), nunca string-based.
- **Threads**: trabalho pesado em `QThread`/`QtConcurrent`; nunca bloquear thread de UI. Cross-thread → `Qt::QueuedConnection`.
- **libc não-reentrante** (`getpwuid`, `getgrgid`, etc.) → usar variantes `_r` com mutex.
- **Parent/child ownership** para widgets; recursos QML em `.qrc`.
- **Tema**: sem `setStyleSheet` inline com cores fixas — usar paleta central (Breeze Dark/Light).

## 6. Fluxo TDD/ATDD

1. **Red** — teste falhando primeiro (ATDD no feature, unit por componente).
2. **Green** — implementação mínima.
3. **Refactor** — conformidade com §3/§4/§5.
4. **Audit** — `clang-tidy` + `cppcheck` + `valgrind` sem regressão.

Comandos-padrão:
```bash
cmake -B build -G Ninja && cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
valgrind --leak-check=full ./build/unit_tests
```

## 7. Gates de qualidade — MUST PASS para Beta

| Gate | Ferramenta | Critério |
| :--- | :--- | :--- |
| Build | cmake/ninja | Zero erros, zero novos warnings |
| T1 Unit | GTest | 100% dos testes passando |
| T2 Estática | clang-tidy + cppcheck | Zero `bugprone-*`/`security-*`/`cert-*` |
| T4 Dinâmica | Valgrind / ASan / UBSan | Zero *definitely lost*, zero ERROR SUMMARY |
| T5/T12 CVEs | trivy ou grype | Zero CRITICAL em dependências |
| T8 Secrets | gitleaks | Zero detectados no histórico completo |
| A2 Camadas | grep + revisão | Zero violação (Qt GUI em domínio, SQL em widget, etc.) |
| A6 Cobertura | lcov (`--ignore-errors mismatch,source,gcov`) | ≥ 70% em `src/domain/` e serviços |
| A3 UI/UX | contraste + Tab/Enter | WCAG AA (≥4,5:1 normal / ≥3:1 grande) |

## 8. Fluxo mandatório após cada feature

1. `./deploy.sh "<conventional commit>"` (build + commit + push).
2. Atualizar `TODO.md` (mover ID do item entre colunas Status/Estado Auditado).
3. Perguntar ao usuário: **"Como deseja prosseguir?"**.

Sem SESSION.md — o histórico vive no git log + `TODO.md`.

## 9. Conventional Commits

| Tipo | Quando |
| :--- | :--- |
| `feat` | Nova funcionalidade |
| `fix` | Correção de bug |
| `refactor` | Mudança sem feature/fix |
| `docs` | Documentação |
| `test` | Testes |
| `chore` | Build/CI/tooling |
| `perf` | Performance |
| `style` | Formatação |
| `revert` | Reversão |

Escopo entre parênteses referencia módulo (`feat(audio): …`, `fix(process): …`). Mensagens curtas, imperativas, focadas em *por quê*.

## 10. Segurança Zero-Cloud

- **100% offline** — nenhuma telemetria/upload.
- Persistência sensível via `ISecurityVault` (AES-256-GCM + IV + auth tag).
- Proibido: segredos hardcoded, string concat em SQL, `eval`/`exec` com input externo.
- Validar toda entrada externa (QProcess output, DBus payload, arquivos de `/sys`, `/proc`).

## 11. Repositório

- Remoto: `ssh://git@codeberg.org/petrinhu/my_comp.git`
- `master` é a branch principal — nunca `force push`; trabalho em branch `feat/…` ou `fix/…` quando a mudança for grande.

## 12. Pendências

A tabela de pendências e planejamento do projeto está em `TODO.md` na raiz.

---

*Legado (script Bash v0.3.6) fica em `./legacy/` como referência histórica. Não modificar.*
