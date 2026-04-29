# AI Coder Contract — Best Practices, Architecture & Standards

> **Audience:** AI coding agents (Claude, GPT, Gemini, Copilot, etc.)
> **Purpose:** Mandatory reference before writing, modifying, or reviewing any code.
> **Authority:** Rules use RFC 2119 keywords — MUST, MUST NOT, SHOULD, SHOULD NOT, MAY.
> **Testing & Audit:** All testing, quality and audit procedures are defined in [TESTES.md](TESTES.md).

---

## Table of Contents

1. [How to Use This Document](#1-how-to-use-this-document)
2. [OOP Fundamentals](#2-oop-fundamentals)
3. [SOLID Principles](#3-solid-principles)
4. [Design Patterns — Complete Reference](#4-design-patterns--complete-reference)
5. [Architecture Layers](#5-architecture-layers)
6. [Clean Code Rules](#6-clean-code-rules)
7. [UI/UX Guidelines](#7-uiux-guidelines)
8. [Security](#8-security)
9. [Performance](#9-performance)
10. [Git Process for AI Coders](#10-git-process-for-ai-coders)
11. [Testing & Audit Mandate](#11-testing--audit-mandate)
12. [Language-Specific Rules](#12-language-specific-rules)
13. [Framework-Specific Rules](#13-framework-specific-rules)

---

## 1. How to Use This Document

**Before writing any code, the AI coder MUST:**

1. Read this document fully for the first task in a project.
2. Identify the target language and framework — apply sections 12 and 13 accordingly.
3. Identify the architecture layer being modified — apply section 5 rules.
4. After completing any task: run the checklist in section 11.

**Decision flow:**

```
New task received
      │
      ▼
Read existing code before modifying ──► Understand context fully
      │
      ▼
Identify layer (Frontend / Middleware / Backend)
      │
      ▼
Apply SOLID + Design Pattern rules
      │
      ▼
Write code → Build → No errors/warnings?
      │
      ▼
Run applicable tests from TESTES.md
      │
      ▼
Commit with Conventional Commits format
      │
      ▼
Done
```

**What the AI coder MUST NOT do:**
- Write code without reading existing files first.
- Add features beyond what was requested.
- Introduce hardcoded secrets, credentials, or API keys.
- Skip the build step before committing.
- Use deprecated APIs, unsafe functions, or patterns marked ANTI-PATTERN below.
- Ignore compiler warnings.

---

## 2. OOP Fundamentals

### 2.1 The Four Pillars

**Encapsulation**
- MUST hide internal state. Expose only what callers need.
- MUST use private/protected for data members, public only for interface.

```cpp
// CORRECT
class GerenciadorCache {
public:
    bool salvar(const QString& chave, const QByteArray& dados);
    std::optional<QByteArray> recuperar(const QString& chave) const;
private:
    QHash<QString, QByteArray> m_cache;  // hidden
    int m_tamanho_max{1000};             // hidden
};

// INCORRECT — exposes internals
class GerenciadorCache {
public:
    QHash<QString, QByteArray> cache;   // direct access = violation
};
```

**Abstraction**
- MUST define interfaces (pure abstract classes) for every cross-layer boundary.
- MUST NOT let callers depend on implementation details.

**Inheritance**
- SHOULD prefer composition over inheritance.
- MUST NOT use inheritance for code reuse alone — only for true IS-A relationships.
- Inheritance depth MUST NOT exceed 3 levels.

**Polymorphism**
- MUST use virtual dispatch via interfaces, not type-checking (no `dynamic_cast` chains).
- MUST mark overrides with `override` keyword (C++) or `@Override` (Java/Kotlin).

### 2.2 Class Design Rules

- One class = one clearly stated responsibility.
- Class size: SHOULD NOT exceed 300 lines. If it does, decompose.
- Constructor MUST NOT perform heavy work (I/O, network, database). Use an `initialize()` method.
- MUST implement the Rule of Five/Zero (C++) or equivalent resource management.

---

## 3. SOLID & DRY Principles

### S — Single Responsibility Principle
> A class should have only one reason to change.

- Each class owns one concept: parsing, rendering, fetching, caching, validating — never all at once.
- Test: if you describe the class and use the word "and", split it.

```
CORRECT:  RepositorioCartas  →  only fetches cards
CORRECT:  CacheCartas        →  only caches cards
INCORRECT: RepositorioCartas →  fetches AND caches AND validates
```

### O — Open/Closed Principle
> Open for extension, closed for modification.

- Add behavior via new classes, not by editing existing ones.
- Use Strategy, Decorator, or plugin patterns to extend without modifying.

```cpp
// CORRECT — add new API source without touching existing code
class IRepositorioCartas {
public:
    virtual QVector<CartaPokemon> buscar(const QString& nome) = 0;
    virtual ~IRepositorioCartas() = default;
};
class RepositorioTcgdex  : public IRepositorioCartas { ... };
class RepositorioScrydex : public IRepositorioCartas { ... };  // extension, not modification
```

### L — Liskov Substitution Principle
> Subtypes must be substitutable for their base types without breaking behavior.

- An override MUST NOT weaken preconditions or strengthen postconditions.
- An override MUST NOT throw exceptions the base does not declare.
- MUST NOT override to do nothing or throw `NotImplementedException`.

```cpp
// INCORRECT — LSP violation: override does nothing
class RepositorioNulo : public IRepositorioCartas {
    QVector<CartaPokemon> buscar(const QString&) override { return {}; } // silent failure
};

// CORRECT — explicit null object that documents intent
class RepositorioNulo : public IRepositorioCartas {
    QVector<CartaPokemon> buscar(const QString&) override {
        qWarning() << "RepositorioNulo: operação não suportada";
        return {};
    }
};
```

### I — Interface Segregation Principle
> Clients should not depend on interfaces they do not use.

- Split fat interfaces into role-specific ones.
- Each interface SHOULD have ≤ 5 methods.

```cpp
// INCORRECT — one fat interface
class IRepositorio {
    virtual void salvar(CartaPokemon) = 0;
    virtual CartaPokemon buscar(QString) = 0;
    virtual void deletar(QString) = 0;
    virtual QVector<CartaPokemon> listar() = 0;
    virtual void exportarCSV() = 0;   // unrelated to repo
    virtual void enviarEmail() = 0;   // completely unrelated
};

// CORRECT — segregated
class IRepositorioLeitura  { virtual CartaPokemon buscar(QString) = 0; ... };
class IRepositorioEscrita  { virtual void salvar(CartaPokemon) = 0; ... };
class IExportador          { virtual void exportarCSV() = 0; ... };
```

### D — Dependency Inversion Principle
> Depend on abstractions, not concretions.

- High-level modules MUST NOT import low-level modules directly.
- Both MUST depend on interfaces.
- Inject dependencies via constructor (preferred), setter, or factory.

```cpp
// INCORRECT — high-level depends on concrete low-level
class ServicoCartas {
    RepositorioTcgdex m_repo;  // concrete dependency
};

// CORRECT — depends on abstraction, injected via constructor
class ServicoCartas {
    std::shared_ptr<IRepositorioCartas> m_repo;
public:
    explicit ServicoCartas(std::shared_ptr<IRepositorioCartas> repo)
        : m_repo(std::move(repo)) {}
};
```

### 3.6 DRY — Don't Repeat Yourself

> Every piece of knowledge MUST have a single, unambiguous, authoritative representation in the system.

DRY is a parallel principle to SOLID — same priority, applied independently.

**Rule of three (mandatory):**

- 1st occurrence: write it.
- 2nd occurrence: tolerate the duplication. Do NOT extract yet.
- 3rd occurrence: extract. The pattern is now confirmed.

Premature abstraction is worse debt than duplication. WET (Write Everything Twice) is acceptable; building the wrong abstraction is not.

**Distinguish real duplication from coincidence:**

- **Real duplication**: two pieces of code share the same *reason to change*. Unify them — they will evolve together.
- **Coincidence**: two pieces of code happen to look similar today, but answer different questions. Leave them apart — they will diverge tomorrow, and a forced abstraction will fit neither.

When extracting, name the **reason for change** explicitly. If you cannot name it, the abstraction is premature.

**DRY by layer (apply where it pays):**

| Layer | Common duplication | Extraction |
|-------|--------------------|------------|
| Presentation | Same `QMessageBox::critical()` style in 20 slots | Helper `mostrarErro(QWidget*, const QString&)` |
| Presentation | `QTableView` configuration repeated | Helper `configurarTabela(QTableView*)` |
| Application | Email validation duplicated across services | `EmailValidator` class or `Email` value object |
| Application | Pagination logic repeated | Template `Paginator<T>` |
| Infrastructure | Row → struct mapping repeated | `hydrate()` method on a base repository |
| Infrastructure | SQL connection setup repeated | Centralized `DatabaseConnection` |
| Domain | Same invariants in multiple entities | Base class or C++20 concept |

**DRY in build (CMake):**

| Anti-pattern | Solution |
|--------------|----------|
| Compiler flags repeated in each `target_compile_options` | Function `apply_common_flags(target)` |
| Qt deps repeated in each `target_link_libraries` | Interface library `Qt6CommonDeps` |
| Test boilerplate repeated | Macro `add_unit_test(name sources)` |

**Forbidden:**

- Extracting on the second occurrence "to be safe."
- Hiding distinct semantics behind a shared abstraction with conditional flags (`bool isForX`, `enum Variant { ... }` selecting behavior).
- Renaming a variable in five places via search-and-replace instead of extracting the concept it represents.
- Helper that takes N parameters and contains `if (modo == "a") ... else if (modo == "b")` — two separate functions are clearer.

```cpp
// INCORRECT — extracted on the 2nd occurrence, no confirmed pattern
template<typename T>
auto fazAlgoGenerico(T&& x) { /* ... */ }

// CORRECT — duplicate on the first two occurrences; extract only on the 3rd.
// The name describes the shared *reason*, not the similar form.
QString formatarBytesPorSegundo(qint64 bytes);   // reason: display GPU transfer rates
```

---

## 4. Design Patterns — Complete Reference

Apply patterns when they solve a real problem. MUST NOT apply patterns speculatively.

### 4.1 Creational Patterns

| Pattern | Use When | Key Rule |
|---------|----------|----------|
| **Singleton** | Exactly one instance needed (logger, config) | MUST be thread-safe. AVOID in testable code — prefer DI. |
| **Factory Method** | Subclasses decide which object to create | Define abstract `criar()`, override in subclasses. |
| **Abstract Factory** | Families of related objects (theme, platform) | One factory interface, multiple concrete factories. |
| **Builder** | Complex object construction with many optional params | Separate construction from representation. Fluent API. |
| **Prototype** | Clone expensive objects | Implement deep copy. Avoid shared mutable state. |

```cpp
// Builder example
class QtaqueBuilder {
    Ataque m_ataque;
public:
    QtaqueBuilder& nome(const QString& n)   { m_ataque.nome = n; return *this; }
    QtaqueBuilder& dano(const QString& d)   { m_ataque.dano = d; return *this; }
    QtaqueBuilder& custo(QStringList c)     { m_ataque.custo = std::move(c); return *this; }
    Ataque build() { return std::move(m_ataque); }
};
// Usage:
auto ataque = QtaqueBuilder{}.nome("Tackle").dano("10").custo({"Colorless"}).build();
```

### 4.2 Structural Patterns

| Pattern | Use When | Key Rule |
|---------|----------|----------|
| **Adapter** | Incompatible interfaces must work together | Wrap external API to match internal interface. |
| **Bridge** | Separate abstraction from implementation | Decouple so both can vary independently. |
| **Composite** | Tree structures (UI hierarchy, file system) | Leaf and composite share same interface. |
| **Decorator** | Add behavior dynamically without subclassing | Wrap object, delegate, then extend. |
| **Facade** | Simplify complex subsystem access | One simple interface over many complex classes. |
| **Flyweight** | Many objects sharing common state (icons, fonts) | Separate intrinsic (shared) from extrinsic (unique) state. |
| **Proxy** | Control access, add lazy init, logging, caching | Same interface as real object. |

```cpp
// Composite: repository that tries primary then fallback
class RepositorioComposto : public IRepositorioCartas {
    std::unique_ptr<IRepositorioCartas> m_primario;
    std::unique_ptr<IRepositorioCartas> m_fallback;
public:
    CartaPokemon buscar(const QString& id) override {
        auto resultado = m_primario->buscar(id);
        if (resultado.id.isEmpty()) resultado = m_fallback->buscar(id);
        return resultado;
    }
};
```

### 4.3 Behavioral Patterns

| Pattern | Use When | Key Rule |
|---------|----------|----------|
| **Chain of Responsibility** | Multiple handlers may process a request | Each handler decides to handle or pass forward. |
| **Command** | Encapsulate action as object (undo/redo, queue) | Separate invoker from receiver. |
| **Iterator** | Traverse collection without exposing internals | Use standard iteration protocol. |
| **Mediator** | Reduce coupling between many objects | Central hub coordinates communication. |
| **Memento** | Save/restore object state | Snapshot without violating encapsulation. |
| **Observer** | Notify dependents of state changes | Qt signals/slots ARE the Observer pattern — use them. |
| **State** | Object changes behavior based on internal state | Replace conditionals with state objects. |
| **Strategy** | Switch algorithm at runtime | Extract algorithm family into interchangeable objects. |
| **Template Method** | Define skeleton, let subclasses fill steps | Base class controls flow, subclasses override steps. |
| **Visitor** | Add operations to object structure without modifying it | Separate algorithm from object structure. |
| **Interpreter** | Parse and evaluate a language/grammar | Define grammar as class hierarchy. |

### 4.4 Modern / Architectural Patterns

| Pattern | Use When |
|---------|----------|
| **Repository** | Abstract data source from business logic. |
| **Unit of Work** | Group related database operations into a transaction. |
| **CQRS** | Separate read (Query) from write (Command) operations. |
| **Event Sourcing** | Store state as sequence of events, not current state. |
| **Dependency Injection** | Provide dependencies from outside the class. |
| **Service Locator** | AVOID — hidden dependency, hard to test. Use DI instead. |
| **MVC** | Separate Model (data), View (UI), Controller (logic). |
| **MVP** | Like MVC but Presenter holds all UI logic, View is passive. |
| **MVVM** | ViewModel exposes state; View binds reactively. Qt supports this. |
| **Null Object** | Avoid null checks — provide do-nothing default implementation. |
| **Specification** | Encapsulate business rules as composable predicates. |

---

## 5. Architecture Layers

### 5.1 Layer Rules

```
┌─────────────────────────────────────────┐
│  FRONTEND / PRESENTATION                │  UI components, widgets, views
│  CAN: render, handle user events        │
│  CANNOT: call APIs, access DB, business │
├─────────────────────────────────────────┤
│  MIDDLEWARE / APPLICATION / SERVICE     │  Use cases, orchestration
│  CAN: call domain & infrastructure      │
│  CANNOT: render UI, know HTTP/SQL       │
├─────────────────────────────────────────┤
│  BACKEND / DOMAIN                       │  Entities, interfaces, rules
│  CAN: pure business logic only          │
│  CANNOT: import UI, network, DB libs    │
├─────────────────────────────────────────┤
│  INFRASTRUCTURE / DATA                  │  HTTP, SQL, file system, APIs
│  CAN: implement domain interfaces       │
│  CANNOT: contain business logic         │
└─────────────────────────────────────────┘
```

**Dependency direction:** Frontend → Middleware → Backend ← Infrastructure

**Violations the AI MUST detect and refuse to introduce:**
- HTTP client import in a widget/view file.
- SQL query inside a domain entity.
- Business rule calculation inside a repository.
- UI framework import inside a service or domain class.

### 5.2 Layer Checklist Before Committing

```
[ ] Class belongs to exactly one layer?
[ ] No upward dependency (lower layer importing upper layer)?
[ ] Cross-layer calls go through interfaces only?
[ ] No domain entity imports Qt network/SQL/widget modules?
```

---

## 6. Clean Code Rules

### 6.1 Naming

- Names MUST reveal intent. No abbreviations unless universally known (`id`, `url`, `http`).
- Functions: verb + noun (`buscarCarta`, `salvarCache`, `renderizarGrade`).
- Booleans: `is`, `has`, `can`, `should` prefix (`isValido`, `hasCached`, `canRetry`).
- Constants: ALL_CAPS with underscores (`MAX_TENTATIVAS`, `URL_BASE`).
- Private members: `m_` prefix (`m_cliente_http`, `m_cache`).
- MUST NOT use single-letter names except loop counters (`i`, `j`) and lambda args.

### 6.2 Functions

- MUST do one thing. If you can extract a sub-function with a meaningful name, do it.
- MUST NOT exceed 40 lines. If longer, decompose.
- MUST NOT take more than 4 parameters. Wrap in struct if needed.
- Return early to avoid deep nesting. MUST NOT exceed 3 levels of nesting.

```cpp
// INCORRECT — deep nesting
void processar(Carta c) {
    if (c.valida()) {
        if (!cache.tem(c.id)) {
            if (api.disponivel()) {
                // actual logic buried here
            }
        }
    }
}

// CORRECT — early returns (guard clauses)
void processar(const Carta& c) {
    if (!c.valida()) return;
    if (cache.tem(c.id)) return;
    if (!api.disponivel()) { reportarErro("API indisponível"); return; }
    // actual logic at top level
}
```

### 6.3 Comments

- MUST NOT comment what the code does. Comment **why** it does it.
- MUST comment every workaround, hack, or non-obvious decision.
- MUST update comments when changing the code they describe.

```cpp
// INCORRECT
i++;  // increment i

// CORRECT
// TCGdex returns 1-based page numbers, our internal API uses 0-based
pagina++;
```

### 6.4 Error Handling

- MUST handle all error cases. MUST NOT silently swallow exceptions.
- MUST log errors with context (file, function, relevant data).
- MUST propagate errors to the caller — do not hide failures.
- MUST NOT use exceptions for control flow.
- Use `std::optional`, `std::expected`, or result types for expected failures.

### 6.5 Constants vs Magic Numbers

```cpp
// INCORRECT
if (tentativas > 3) retry();
QPixmap px = img.scaled(260, 360);

// CORRECT
constexpr int MAX_TENTATIVAS = 3;
constexpr QSize TAMANHO_IMAGEM_CARTA{260, 360};
if (tentativas > MAX_TENTATIVAS) retry();
QPixmap px = img.scaled(TAMANHO_IMAGEM_CARTA);
```

### 6.6 RAII and Resource Management

- MUST use RAII for all resources (memory, file handles, DB connections, mutexes).
- MUST prefer smart pointers (`unique_ptr`, `shared_ptr`) over raw `new`/`delete`.
- MUST NOT call `delete` manually in application code.
- MUST NOT store raw owning pointers.

---

## 7. UI/UX Guidelines

### 7.1 Responsiveness

- MUST NEVER block the UI thread with I/O, network, or heavy computation.
- MUST use async patterns (callbacks, signals/slots, async/await, workers).
- All operations > 100ms MUST show a loading indicator.
- All operations > 3s MUST be cancellable.

### 7.2 Feedback

- Every user action MUST produce visible feedback within 200ms.
- Error messages MUST be human-readable, not stack traces or error codes.
- Success and failure states MUST be visually distinct (color + icon, not color alone).
- Empty states MUST show a helpful message, not a blank screen.

### 7.3 Accessibility (WCAG AA minimum)

- Text/background contrast ratio MUST be ≥ 4.5:1 for normal text.
- Text/background contrast ratio MUST be ≥ 3:1 for large text (≥ 18pt).
- All interactive elements MUST be reachable via keyboard (Tab + Enter/Space).
- All images MUST have descriptive alt text or `setAccessibleName()`.
- MUST NOT convey information using color alone (add icon or text).

```
Contrast check reference:
  Normal text on dark (#EFF0F1 on #31363B) = 7.2:1  ✅
  Accent on dark     (#3DAEE9 on #31363B) = 3.8:1  ✅ (large text)
  Red error on white (#DA4453 on #EFF0F1) = 4.6:1  ✅
```

### 7.4 Consistency

- MUST use the project's design token system for all colors, spacing, and typography.
- MUST NOT hardcode color hex values in widget code — use theme tokens.
- Font sizes: MUST follow the type scale (body 13px, subtitle 12px, title 16-18px).
- Spacing: MUST use consistent margins (8px grid).
- Interactive elements (buttons, links) MUST have hover AND pressed states.

### 7.5 Forms and Inputs

- MUST validate inputs at the point of submission, not silently on blur.
- MUST show validation errors adjacent to the offending field.
- MUST NOT clear the form on error — preserve user input.
- Submit buttons MUST be disabled during async operations.

### 7.6 Theme & Visual Identity

The application MUST use the **Breeze/KDE** visual theme as default — palette, typography, spacing, and icons.

**Theme assets MUST be embedded in the project**:
- Qt: QSS files, palette definitions, and SVG icons MUST live inside `.qrc` resources (e.g. `:/temas/breeze/breeze.qss`, `:/temas/breeze/icones/`).
- Web: CSS and icon assets MUST be served from project static files.
- The application MUST NOT require KDE Plasma, `kf5`/`kf6` runtime, OS-installed icon themes, or any external theme library at runtime. The app MUST render with full Breeze appearance on a clean machine without KDE installed.

**Apply via central theme system (Qt example):**

```cpp
// main.cpp — apply Breeze before creating any widget
QPalette breeze = TemaBreeze::carregarPaleta();           // from embedded resource
qApp->setPalette(breeze);

QFile qss(QStringLiteral(":/temas/breeze/breeze.qss"));
qss.open(QIODevice::ReadOnly);
qApp->setStyleSheet(QString::fromUtf8(qss.readAll()));

// Icons — fromTheme with embedded fallback (always present in .qrc)
QIcon::setThemeName(QStringLiteral("breeze"));
QIcon::setFallbackThemeName(QStringLiteral("breeze-embarcado"));
QIcon::setFallbackSearchPaths({QStringLiteral(":/temas/breeze/icones")});
```

Typical resource layout in `assets/temas/breeze/` (registered in `.qrc`):

```
assets/temas/breeze/
├── breeze.qss
├── paleta.qss          (or palette defined in C++ from constants)
├── icones/
│   └── *.svg
└── breeze.qrc
```

**User override:** the user MAY switch theme via flag/setting (`--theme=system`), but Breeze is the invariant default.

**Forbidden:**
- `setStyleSheet()` with hardcoded colors in individual widgets (covered in §13.1).
- Loading themes from `~/.local/share/plasma/`, `/usr/share/themes/`, or any system path at runtime.
- Linking against `KF6::*` or `kdelibs` solely to obtain the theme.
- Releasing without verifying the binary renders correctly on a non-KDE Linux box (e.g. GNOME live USB) and on Windows/macOS if those targets are in scope.

---

## 8. Security

> Full procedures in [TESTES.md](TESTES.md) — sections T8, T10, T12.

### 8.1 OWASP Top 10 — AI Coder Checklist

| # | Risk | Rule |
|---|------|------|
| A01 | Broken Access Control | MUST validate authorization server-side on every request. |
| A02 | Cryptographic Failures | MUST use AES-256+ for data at rest. MUST use TLS 1.2+ for data in transit. NEVER MD5/SHA1 for security. |
| A03 | Injection | MUST use prepared statements for ALL SQL. MUST sanitize all inputs before use in commands/queries. |
| A04 | Insecure Design | MUST threat-model new features before implementing. |
| A05 | Security Misconfiguration | MUST NOT ship with debug flags, default passwords, or verbose error messages in production. |
| A06 | Vulnerable Components | MUST check CVEs before adding any dependency (see T12 in TESTES.md). |
| A07 | Auth Failures | MUST use established auth libraries. NEVER roll your own crypto. |
| A08 | Software Integrity | MUST verify integrity of downloaded dependencies (checksums, signatures). |
| A09 | Logging Failures | MUST log security events. MUST NOT log passwords, tokens, or PII. |
| A10 | SSRF | MUST validate and allowlist all URLs before fetching. |

### 8.2 Hardcoded Secrets — Zero Tolerance

```cpp
// INCORRECT — NEVER commit this
const QString API_KEY = "sk-live-abc123xyz789";
QString url = "https://api.com?token=mypassword";

// CORRECT — load from secure storage or environment
const QString apiKey = gerenciadorChaves.recuperar("limitless_api_key");
```

### 8.3 Input Validation

- MUST validate all data arriving from: user input, API responses, file reads, environment variables.
- MUST NOT trust data from any external source.
- MUST constrain string lengths, numeric ranges, and allowed characters at entry points.

---

## 9. Performance

### 9.1 General Rules

- MUST profile before optimizing. No premature optimization.
- MUST cache results of expensive operations (network, disk, computation).
- MUST NOT make redundant API calls — check cache first.
- MUST use lazy loading for data not immediately needed.
- MUST NOT copy large objects unnecessarily — use references and move semantics.

### 9.2 Network

- MUST debounce user-triggered searches (wait ≥ 300ms after last keystroke).
- MUST implement request cancellation for outdated requests.
- MUST set reasonable timeouts (connect: 10s, read: 30s).
- MUST retry with exponential backoff on transient failures (max 3 attempts).

### 9.3 Memory

- MUST release resources when they go out of scope (RAII).
- MUST NOT hold large objects in memory indefinitely — use LRU cache with size limit.
- MUST NOT leak Qt objects — ensure parent ownership or explicit deletion.

### 9.4 Rendering

- MUST NOT do layout calculations on the UI thread unnecessarily.
- Avoid creating/destroying widgets in tight loops — reuse or use virtual scrolling.
- Heavy image operations MUST be done off-thread, result posted to UI thread.

---

## 10. Git Process for AI Coders

### 10.1 Before Writing Any Code

```bash
# MUST: read current state of files to be modified
# MUST: understand existing patterns before introducing new ones
# MUST: check if a build passes before starting
cmake --build build -j$(nproc) 2>&1 | grep -E "error:|warning:"
```

### 10.2 Conventional Commits (MANDATORY)

Format: `<type>(<scope>): <description>`

| Type | When to use |
|------|------------|
| `feat` | New feature added |
| `fix` | Bug fix |
| `refactor` | Code change without feature or fix |
| `docs` | Documentation only |
| `test` | Adding or fixing tests |
| `chore` | Build, CI, dependencies, tooling |
| `perf` | Performance improvement |
| `style` | Formatting, no logic change |
| `revert` | Reverting a previous commit |

```bash
# CORRECT examples
git commit -m "feat(filtros): add real-time chip filter applied to loaded grid"
git commit -m "fix(tema): type icon not rendering in QLabel RichText via data URI"
git commit -m "docs: add TESTES.md with complete audit and quality guide"
git commit -m "chore: convert type images webp→png, remove webp files"

# INCORRECT
git commit -m "fix stuff"
git commit -m "WIP"
git commit -m "changes"
```

### 10.3 Branch Naming

```
feat/nome-da-feature
fix/descricao-do-bug
refactor/modulo-afetado
docs/nome-do-documento
chore/ferramenta-ou-dep
test/modulo-testado
```

### 10.4 Commit Checklist (MUST complete before every commit)

```
[ ] Build passes with zero errors
[ ] Zero new compiler warnings introduced
[ ] No hardcoded secrets, tokens, or credentials
[ ] .gitignore excludes build artifacts, IDE files, .env files
[ ] Commit message follows Conventional Commits format
[ ] Files staged are only those related to the current task
[ ] No unrelated changes mixed in (separate commits for separate concerns)
```

### 10.5 What the AI MUST NEVER Do in Git

- MUST NOT force-push to `main` or `master`.
- MUST NOT commit files containing secrets (`.env`, credentials, private keys).
- MUST NOT amend published commits (use a new commit instead).
- MUST NOT use `--no-verify` to skip hooks unless explicitly instructed.
- MUST NOT batch unrelated changes into one commit.
- MUST NOT commit generated files (build artifacts, `moc_*.cpp`, `qrc_*.cpp`).

### 10.6 Pull Request Description Template

```markdown
## What
[One sentence: what this PR does]

## Why
[One sentence: why it was needed]

## How
- [Key technical decision 1]
- [Key technical decision 2]

## Checklist
- [ ] Build passes
- [ ] Tests pass (reference TESTES.md sections run)
- [ ] No new warnings
- [ ] No secrets committed
- [ ] CHANGELOG.md updated (if user-facing change)
```

---

## 11. Testing & Audit Mandate

> Full procedures, commands, and tools: **[TESTES.md](TESTES.md)**

### When to Run Tests

| Event | Required tests |
|-------|---------------|
| Every commit | Build passes, zero warnings |
| Feature complete | T1 (unit), T2 (static analysis), T4 (ASan/UBSan) |
| Before any release/deploy | T1–T12 full suite + A1–A10 full audit |
| New dependency added | T5 (dependency scan) + T12 (CVE check) |
| Auth/crypto code changed | T8 (secrets), T10 (SQL injection), A5 (dynamic analysis) |
| UI changed | A3 (UX/accessibility) |
| Architecture changed | A2 (layer audit), A7 (coupling), A9 (SOLID) |

### Minimum Quality Gates (MUST pass before release)

```
[ ] T1  Unit tests: 0 failures
[ ] T2  Static analysis: 0 security/bugprone errors
[ ] T4  ASan: 0 ERROR SUMMARY
[ ] T8  Secrets scan: 0 detected
[ ] T12 CVE scan: 0 CRITICAL unpatched
[ ] A2  Architecture: 0 layer violations
[ ] A10 Audit report generated and reviewed
```

---

## 12. Language-Specific Rules

### 12.1 C++ / Qt *(Mandatory)*

**Version:** C++20 minimum. Qt 6.x.

**Memory:**
```cpp
// MUST use smart pointers
auto obj = std::make_unique<MeuObjeto>();
auto shared = std::make_shared<Servico>(dep);

// MUST use QPointer for optional Qt object observation
QPointer<QLabel> lbl_seguro = new QLabel(parent);
if (lbl_seguro) lbl_seguro->setText("ok");  // safe even if deleted

// MUST NOT
MeuObjeto* raw = new MeuObjeto();  // who owns this?
delete raw;                         // manual delete = leak risk
```

**Qt Specifics:**
- MUST use Qt parent-child ownership for widgets (parent deletes children).
- MUST use signals/slots for cross-layer communication (Observer pattern built-in).
- MUST NOT call network, file I/O, or database from the UI thread.
- MUST use `QThread` + `moveToThread()` or `QtConcurrent` for background work.
- MUST use `Qt::QueuedConnection` when emitting across threads.
- MUST use `QSqlQuery` with `bindValue()` — NEVER string concatenation for SQL.
- MUST check `QNetworkReply::error()` before reading response data.
- MUST handle all `QFile::open()` failures.
- Theme/style: MUST use QSS via central theme system, NEVER `setStyleSheet()` in individual widgets with hardcoded colors. Default visual theme is **Breeze/KDE embedded** in the project's `.qrc` (see §7.6). MUST NOT depend on KDE Plasma, `kf5`/`kf6`, or system-installed theme libraries at runtime.

**Modern C++20 MUST-use features:**
```cpp
std::optional<Carta>     // instead of nullptr checks
std::variant<Ok, Erro>   // instead of error codes
[[nodiscard]]            // on functions whose return value must be checked
const auto&              // prefer const references
if (auto val = buscar(); val.has_value())  // init-statement in if
```

**MUST NOT use:**
```cpp
NULL           // use nullptr
(Type*)ptr     // use static_cast<Type*>(ptr)
printf/scanf   // use qDebug() / QTextStream
gets()         // buffer overflow risk
strcpy/strcat  // use QString or std::string
```

### 12.2 Python

**Version:** 3.10+ minimum. Type hints MANDATORY.

```python
# MUST — type hints, dataclasses, pathlib
from pathlib import Path
from dataclasses import dataclass

@dataclass
class Carta:
    id: str
    nome: str
    hp: int | None = None

def buscar_carta(nome: str) -> Carta | None: ...

# MUST use context managers for resources
with open(path, "r", encoding="utf-8") as f:
    data = f.read()

# MUST NOT
def buscar(nome):         # no type hints
    f = open(nome)        # no context manager
    return eval(f.read()) # eval = security risk
```

- MUST use `venv` or `poetry` for isolation.
- MUST NOT use `*` imports.
- MUST use `logging` module, not `print()` for application logs.
- MUST use `httpx` or `requests` with explicit timeouts.
- MUST use `pydantic` for external data validation.

### 12.3 TypeScript / JavaScript

**Version:** TypeScript 5+ strict mode. ES2022+.

```typescript
// MUST — strict typing, no any
interface Carta { id: string; nome: string; hp?: number; }
async function buscarCarta(nome: string): Promise<Carta | null> { ... }

// MUST use optional chaining and nullish coalescing
const hp = carta?.hp ?? 0;

// MUST NOT
const carta: any = {};      // any defeats TypeScript
function buscar(nome) {}    // missing types
var x = 1;                  // use const/let
```

- MUST enable `strict: true` in `tsconfig.json`.
- MUST NOT use `==` — use `===` always.
- MUST handle Promise rejections (`.catch()` or `try/catch`).
- MUST NOT access `localStorage`/`sessionStorage` without existence checks.
- MUST sanitize all DOM insertions — use `textContent`, not `innerHTML` with untrusted data.

### 12.4 Java

**Version:** Java 17+ LTS minimum.

```java
// MUST use records for immutable data
public record Carta(String id, String nome, Optional<Integer> hp) {}

// MUST use Optional instead of null
Optional<Carta> buscar(String nome) { ... }

// MUST use try-with-resources
try (var conn = dataSource.getConnection()) { ... }

// MUST NOT
public Carta buscar(String nome) { return null; }  // return null
catch (Exception e) { e.printStackTrace(); }        // swallow exception
```

- MUST use `final` for variables that don't change.
- MUST use `var` (Java 10+) for local variables when type is obvious.
- MUST use `PreparedStatement` for all SQL.
- MUST NOT use raw types for generics.

### 12.5 C# / .NET

**Version:** C# 12 / .NET 8+ minimum.

```csharp
// MUST use nullable reference types
public record Carta(string Id, string Nome, int? Hp = null);

// MUST use async/await
public async Task<Carta?> BuscarAsync(string nome, CancellationToken ct = default) { ... }

// MUST use pattern matching
if (resultado is { Hp: > 0 } carta) { ... }

// MUST NOT
public Carta Buscar(string nome) { return null; }   // return null
Thread.Sleep(1000);                                  // blocks UI thread
```

- MUST enable `<Nullable>enable</Nullable>` in `.csproj`.
- MUST use `IDisposable`/`using` for resource management.
- MUST use `ILogger<T>` for logging.
- MUST NOT catch `Exception` without re-throwing or logging.

### 12.6 Go

**Version:** Go 1.22+ minimum.

```go
// MUST always handle errors
carta, err := repositorio.Buscar(nome)
if err != nil {
    return fmt.Errorf("buscarCarta: %w", err)  // wrap with context
}

// MUST use contexts for cancellation
func Buscar(ctx context.Context, nome string) (*Carta, error) { ... }

// MUST NOT
carta, _ := repositorio.Buscar(nome)  // ignore error
```

- MUST use `errors.Is` / `errors.As` for error checking.
- MUST use `defer` for cleanup (files, mutex unlock).
- MUST NOT use goroutines without thinking about their lifecycle.
- MUST use `sync.WaitGroup` or channels to coordinate goroutines.

### 12.7 Rust

**Version:** Rust 2021 edition minimum.

```rust
// MUST use Result and Option — never panic in library code
fn buscar_carta(nome: &str) -> Result<Carta, BuscaErro> { ... }

// MUST handle all variants
match resultado {
    Ok(carta) => renderizar(carta),
    Err(BuscaErro::NaoEncontrado) => mostrar_vazio(),
    Err(e) => reportar_erro(e),
}

// MUST NOT
fn buscar(nome: &str) -> Carta { ... }  // must return Result
unwrap()  // panics — use ? operator or proper match
```

- MUST use `clippy` with warnings as errors in CI.
- MUST use `thiserror` or `anyhow` for error types.
- MUST NOT use `unsafe` without detailed comment explaining why.

### 12.8 Kotlin

**Version:** Kotlin 1.9+ minimum.

```kotlin
// MUST use data classes for DTOs
data class Carta(val id: String, val nome: String, val hp: Int? = null)

// MUST use coroutines for async
suspend fun buscarCarta(nome: String): Carta? = withContext(Dispatchers.IO) { ... }

// MUST use sealed classes for results
sealed class Resultado<out T> {
    data class Sucesso<T>(val dados: T) : Resultado<T>()
    data class Erro(val mensagem: String) : Resultado<Nothing>()
}

// MUST NOT
fun buscar(nome: String): Carta? = null  // nullable without reason
Thread.sleep(1000)                       // blocks thread
```

- MUST use `val` by default; `var` only when mutation is truly needed.
- MUST use scope functions (`let`, `apply`, `run`, `also`) appropriately.
- MUST NOT use `!!` operator — use `?: return` or `?: throw` instead.

### 12.9 Swift

**Version:** Swift 5.9+ minimum.

```swift
// MUST use async/await
func buscarCarta(nome: String) async throws -> Carta { ... }

// MUST use Result type
func buscar(nome: String) -> Result<Carta, BuscaErro> { ... }

// MUST use guard for early exit
guard let carta = resultado else { return }

// MUST NOT
var carta: Carta!  // implicitly unwrapped optional = crash risk
try! buscarCarta() // force try = crash on error
```

- MUST mark UI updates with `@MainActor`.
- MUST use `Codable` for JSON serialization.
- MUST NOT force-unwrap optionals (`!`) in production code.

### 12.10 PHP

**Version:** PHP 8.2+ minimum.

```php
// MUST use strict types
declare(strict_types=1);

// MUST use type declarations
function buscarCarta(string $nome): ?Carta { ... }

// MUST use prepared statements
$stmt = $pdo->prepare("SELECT * FROM cartas WHERE nome = :nome");
$stmt->execute([':nome' => $nome]);

// MUST NOT
$resultado = mysqli_query($conn, "SELECT * FROM cartas WHERE nome = '$nome'");  // SQL injection
```

- MUST use Composer for dependency management.
- MUST enable `error_reporting(E_ALL)` in development.
- MUST NOT use `eval()`, `exec()`, `system()` with user input.
- MUST use `password_hash()` / `password_verify()` for passwords.

---

## 13. Framework-Specific Rules

### 13.1 Qt (C++) *(Mandatory)*

- MUST use Qt's parent-child memory model — pass `this` as parent to child widgets.
- MUST use `Q_OBJECT` macro in every class using signals/slots.
- MUST use `connect()` with typed signal/slot syntax (not string-based).
- UI definition: prefer code over `.ui` files for complex dynamic layouts.
- MUST use `QThread::create()` or `QtConcurrent::run()` for background tasks, never `QThread::sleep()` on UI thread.
- Resources MUST be declared in `.qrc` files and accessed via `:/prefix/alias`.
- MUST use `QSettings` for user preferences, not hardcoded paths.
- Styles MUST come from a central QSS system applied via `qApp->setStyleSheet()`.

### 13.2 React / Next.js

- MUST use functional components with hooks. No class components.
- MUST NOT mutate state directly — always use `setState` or dispatchers.
- MUST use `useCallback` and `useMemo` for expensive computations and stable references.
- MUST use `React.lazy` + `Suspense` for code splitting.
- MUST sanitize all user-generated content before rendering (`DOMPurify`).
- Next.js: MUST use Server Components for data fetching. Client Components only for interactivity.

### 13.3 Django / FastAPI (Python)

- Django: MUST use ORM — NEVER raw SQL strings with user input.
- Django: MUST use `@login_required` and permission decorators.
- FastAPI: MUST use `pydantic` models for request/response validation.
- FastAPI: MUST use dependency injection for DB sessions and auth.
- MUST configure `ALLOWED_HOSTS` and `DEBUG=False` in production.
- MUST use `python-decouple` or `pydantic-settings` for environment config.

### 13.4 Spring Boot (Java)

- MUST use constructor injection, not field injection (`@Autowired` on field).
- MUST use `@Transactional` at the service layer, not repository.
- MUST validate input DTOs with `@Valid` + `javax.validation` annotations.
- MUST use `ResponseEntity<T>` for explicit HTTP response control.
- MUST externalize configuration via `application.yml` + environment variables.

### 13.5 ASP.NET Core (C#)

- MUST use `IOptions<T>` for configuration binding.
- MUST use `ILogger<T>` for structured logging.
- MUST use `async`/`await` all the way through the stack.
- MUST use `ModelState.IsValid` before processing any input.
- MUST use built-in DI container — no static service locators.

### 13.6 Vue.js / Nuxt.js

- MUST use Composition API with `<script setup>`.
- MUST use `defineProps` and `defineEmits` with types.
- MUST use Pinia for state management (not Vuex).
- MUST NOT use `v-html` with untrusted data.
- Nuxt: MUST use `useFetch`/`useAsyncData` for data fetching.

### 13.7 Angular

- MUST use standalone components (Angular 17+).
- MUST use `inject()` function instead of constructor injection.
- MUST use `OnPush` change detection for performance.
- MUST use reactive forms, not template-driven for complex forms.
- MUST use `HttpClient` with interceptors for auth headers.

### 13.8 Laravel (PHP)

- MUST use Eloquent ORM with query builder — never raw SQL with user input.
- MUST use Form Request classes for validation.
- MUST use policies and gates for authorization.
- MUST use `.env` for all secrets — NEVER hardcode.
- MUST use queues for any operation > 2 seconds.

### 13.9 Flutter (Dart)

- MUST use `const` constructors wherever possible.
- MUST use `BLoC` or `Riverpod` for state management — no `setState` in complex widgets.
- MUST dispose controllers, streams, and subscriptions in `dispose()`.
- MUST use `FutureBuilder`/`StreamBuilder` for async UI, not `setState` after await.
- MUST NOT block the main isolate — use `compute()` for heavy work.

### 13.10 Express.js / NestJS (Node.js)

- MUST use `helmet` middleware for HTTP security headers.
- MUST use `express-rate-limit` for all public endpoints.
- MUST validate request bodies with `joi`, `zod`, or `class-validator`.
- NestJS: MUST use Guards for auth, Interceptors for logging, Pipes for validation.
- MUST use `async`/`await` — NEVER mix callbacks with promises.
- MUST handle `unhandledRejection` and `uncaughtException` at process level.

---

*This contract is the authoritative reference for all code written in this project.*
*Last updated: 2026-03-29 — sync with TESTES.md for testing procedures.*
