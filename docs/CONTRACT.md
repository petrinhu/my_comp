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
14. [Universal Engineering Principles](#14-universal-engineering-principles)
15. [API Design — REST](#15-api-design--rest)
16. [Logging & Observability](#16-logging--observability)
17. [LGPD Compliance Baseline](#17-lgpd-compliance-baseline)

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

## 3. SOLID Principles

### S — Single Responsibility Principle
> A class should have only one reason to change.

- Each class owns one concept: parsing, rendering, fetching, caching, validating — never all at once.
- Test: if you describe the class and use the word "and", split it.

```
CORRECT:  RepositorioProdutos  →  only fetches items
CORRECT:  CacheProdutos        →  only caches items
INCORRECT: RepositorioProdutos →  fetches AND caches AND validates
```

### O — Open/Closed Principle
> Open for extension, closed for modification.

- Add behavior via new classes, not by editing existing ones.
- Use Strategy, Decorator, or plugin patterns to extend without modifying.

```cpp
// CORRECT — add new API source without touching existing code
class IRepositorioProdutos {
public:
    virtual QVector<Produto> buscar(const QString& nome) = 0;
    virtual ~IRepositorioProdutos() = default;
};
class RepositorioApiA  : public IRepositorioProdutos { ... };
class RepositorioApiB : public IRepositorioProdutos { ... };  // extension, not modification
```

### L — Liskov Substitution Principle
> Subtypes must be substitutable for their base types without breaking behavior.

- An override MUST NOT weaken preconditions or strengthen postconditions.
- An override MUST NOT throw exceptions the base does not declare.
- MUST NOT override to do nothing or throw `NotImplementedException`.

```cpp
// INCORRECT — LSP violation: override does nothing
class RepositorioNulo : public IRepositorioProdutos {
    QVector<Produto> buscar(const QString&) override { return {}; } // silent failure
};

// CORRECT — explicit null object that documents intent
class RepositorioNulo : public IRepositorioProdutos {
    QVector<Produto> buscar(const QString&) override {
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
    virtual void salvar(Produto) = 0;
    virtual Produto buscar(QString) = 0;
    virtual void deletar(QString) = 0;
    virtual QVector<Produto> listar() = 0;
    virtual void exportarCSV() = 0;   // unrelated to repo
    virtual void enviarEmail() = 0;   // completely unrelated
};

// CORRECT — segregated
class IRepositorioLeitura  { virtual Produto buscar(QString) = 0; ... };
class IRepositorioEscrita  { virtual void salvar(Produto) = 0; ... };
class IExportador          { virtual void exportarCSV() = 0; ... };
```

### D — Dependency Inversion Principle
> Depend on abstractions, not concretions.

- High-level modules MUST NOT import low-level modules directly.
- Both MUST depend on interfaces.
- Inject dependencies via constructor (preferred), setter, or factory.

```cpp
// INCORRECT — high-level depends on concrete low-level
class ServicoProdutos {
    RepositorioApiA m_repo;  // concrete dependency
};

// CORRECT — depends on abstraction, injected via constructor
class ServicoProdutos {
    std::shared_ptr<IRepositorioProdutos> m_repo;
public:
    explicit ServicoProdutos(std::shared_ptr<IRepositorioProdutos> repo)
        : m_repo(std::move(repo)) {}
};
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
class RepositorioComposto : public IRepositorioProdutos {
    std::unique_ptr<IRepositorioProdutos> m_primario;
    std::unique_ptr<IRepositorioProdutos> m_fallback;
public:
    Produto buscar(const QString& id) override {
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
- Functions: verb + noun (`buscarItem`, `salvarCache`, `renderizarLista`).
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
void processar(Item c) {
    if (c.valida()) {
        if (!cache.tem(c.id)) {
            if (api.disponivel()) {
                // actual logic buried here
            }
        }
    }
}

// CORRECT — early returns (guard clauses)
void processar(const Item& c) {
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
// API externa usa páginas base-1; nossa API interna usa base-0
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
QPixmap px = img.scaled(120, 160);

// CORRECT
constexpr int MAX_TENTATIVAS = 3;
constexpr QSize TAMANHO_MINIATURA{120, 160};
if (tentativas > MAX_TENTATIVAS) retry();
QPixmap px = img.scaled(TAMANHO_MINIATURA);
```

### 6.6 RAII and Resource Management

- MUST use RAII for all resources (memory, file handles, DB connections, mutexes).
- MUST prefer smart pointers (`unique_ptr`, `shared_ptr`) over raw `new`/`delete`.
- MUST NOT call `delete` manually in application code.
- MUST NOT store raw owning pointers.

### 6.7 DRY — Don't Repeat Yourself

**Regra de Três:** Na **primeira** ocorrência, escreva. Na **segunda**, registre a repetição. Na **terceira**, extraia.

```cpp
// 1ª e 2ª ocorrências: duplicação aceitável (WET — Write Everything Twice)
QString formatarPreco(double v)  { return QString("R$ %1").arg(v, 0, 'f', 2); }
QString formatarSaldo(double v)  { return QString("R$ %1").arg(v, 0, 'f', 2); }

// 3ª ocorrência: EXTRAIA — nomeie a razão comum de mudança
// Razão: "formatação de valores monetários em BRL"
QString formatarBRL(double valor, int casas = 2) {
    return QString("R$ %1").arg(valor, 0, 'f', casas);
}
```

**Duplicação real vs. coincidência:**

| Tipo | Definição | Regra |
|------|-----------|-------|
| **Duplicação real** | Mesmo conceito, mesma razão de mudar | MUST extrair |
| **Coincidência** | Parece similar hoje; divergirá amanhã | MUST NOT unificar |

```cpp
// COINCIDÊNCIA — NÃO unificar mesmo tendo lógica idêntica hoje:
// Regras de negócio distintas; mudarão de forma independente
bool validarIdade(int anos)        { return anos >= 0 && anos <= 120; }
bool validarAnoFabricacao(int ano) { return ano  >= 0 && ano  <= 120; }

// DUPLICAÇÃO REAL — MUST extrair:
// Mesma razão de mudar: "validar quantidade positiva com teto de estoque"
bool validarQuantidade(int qtd)  { return qtd > 0 && qtd <= 9999; }
bool validarEstoque(int estoq)   { return estoq > 0 && estoq <= 9999; }
// → bool validarQuantidadePositiva(int val) { return val > 0 && val <= 9999; }
```

**Rules (RFC 2119):**

- MUST name the *common reason to change* when extracting — similarity in code alone is insufficient justification.
- MUST NOT unify logic with distinct business meanings even if syntactically identical.
- SHOULD prefer WET (Write Everything Twice) over a premature abstraction that fits neither caller.
- MUST NOT create generic helpers to avoid two similar lines; three real occurrences are required.
- MAY tolerate duplication in tests when each test independently documents a distinct behavior.

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
- Theme/style: MUST use QSS via central theme system, NEVER `setStyleSheet()` in individual widgets with hardcoded colors.

**Modern C++20 MUST-use features:**
```cpp
std::optional<Item>     // instead of nullptr checks
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
class Item:
    id: str
    nome: str
    preco: float | None = None

def buscar_item(nome: str) -> Item | None: ...

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
interface Item { id: string; nome: string; preco?: number; }
async function buscarItem(nome: string): Promise<Item | null> { ... }

// MUST use optional chaining and nullish coalescing
const preco = item?.preco ?? 0;

// MUST NOT
const item: any = {};      // any defeats TypeScript
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
public record Item(String id, String nome, Optional<Double> preco) {}

// MUST use Optional instead of null
Optional<Item> buscar(String nome) { ... }

// MUST use try-with-resources
try (var conn = dataSource.getConnection()) { ... }

// MUST NOT
public Item buscar(String nome) { return null; }  // return null
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
public record Item(string Id, string Nome, double? Preco = null);

// MUST use async/await
public async Task<Item?> BuscarAsync(string nome, CancellationToken ct = default) { ... }

// MUST use pattern matching
if (resultado is { Preco: > 0 } item) { ... }

// MUST NOT
public Item Buscar(string nome) { return null; }   // return null
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
item, err := repositorio.Buscar(nome)
if err != nil {
    return fmt.Errorf("buscarItem: %w", err)  // wrap with context
}

// MUST use contexts for cancellation
func Buscar(ctx context.Context, nome string) (*Item, error) { ... }

// MUST NOT
item, _ := repositorio.Buscar(nome)  // ignore error
```

- MUST use `errors.Is` / `errors.As` for error checking.
- MUST use `defer` for cleanup (files, mutex unlock).
- MUST NOT use goroutines without thinking about their lifecycle.
- MUST use `sync.WaitGroup` or channels to coordinate goroutines.

### 12.7 Rust

**Version:** Rust 2021 edition minimum.

```rust
// MUST use Result and Option — never panic in library code
fn buscar_item(nome: &str) -> Result<Item, BuscaErro> { ... }

// MUST handle all variants
match resultado {
    Ok(item) => renderizar(item),
    Err(BuscaErro::NaoEncontrado) => mostrar_vazio(),
    Err(e) => reportar_erro(e),
}

// MUST NOT
fn buscar(nome: &str) -> Item { ... }  // must return Result
unwrap()  // panics — use ? operator or proper match
```

- MUST use `clippy` with warnings as errors in CI.
- MUST use `thiserror` or `anyhow` for error types.
- MUST NOT use `unsafe` without detailed comment explaining why.

### 12.8 Kotlin

**Version:** Kotlin 1.9+ minimum.

```kotlin
// MUST use data classes for DTOs
data class Item(val id: String, val nome: String, val preco: Double? = null)

// MUST use coroutines for async
suspend fun buscarItem(nome: String): Item? = withContext(Dispatchers.IO) { ... }

// MUST use sealed classes for results
sealed class Resultado<out T> {
    data class Sucesso<T>(val dados: T) : Resultado<T>()
    data class Erro(val mensagem: String) : Resultado<Nothing>()
}

// MUST NOT
fun buscar(nome: String): Item? = null  // nullable without reason
Thread.sleep(1000)                       // blocks thread
```

- MUST use `val` by default; `var` only when mutation is truly needed.
- MUST use scope functions (`let`, `apply`, `run`, `also`) appropriately.
- MUST NOT use `!!` operator — use `?: return` or `?: throw` instead.

### 12.9 Swift

**Version:** Swift 5.9+ minimum.

```swift
// MUST use async/await
func buscarItem(nome: String) async throws -> Item { ... }

// MUST use Result type
func buscar(nome: String) -> Result<Item, BuscaErro> { ... }

// MUST use guard for early exit
guard let item = resultado else { return }

// MUST NOT
var item: Item!  // implicitly unwrapped optional = crash risk
try! buscarItem() // force try = crash on error
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
function buscarItem(string $nome): ?Item { ... }

// MUST use prepared statements
$stmt = $pdo->prepare("SELECT * FROM itens WHERE nome = :nome");
$stmt->execute([':nome' => $nome]);

// MUST NOT
$resultado = mysqli_query($conn, "SELECT * FROM itens WHERE nome = '$nome'");  // SQL injection
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

## 14. Universal Engineering Principles

> Complement to SOLID and DRY. Apply across all languages and stacks to produce robust, professional, and high-performance code.

### 14.1 KISS — Keep It Simple, Stupid

The simplest solution that correctly solves the problem is the right solution. Complexity is debt.

**Rules:**
- MUST NOT add layers of abstraction without a concrete, present reason.
- MUST NOT use a design pattern just because it fits — only when it removes real pain.
- When two solutions work, MUST choose the one a new team member understands in 30 seconds.

```python
# INCORRECT — unnecessary abstraction for a trivial operation
class SomadorDeInteiros:
    def somar(self, a: int, b: int) -> int:
        return a + b
resultado = SomadorDeInteiros().somar(2, 3)

# CORRECT
resultado = 2 + 3
```

```c
/* INCORRECT — premature indirection when higher-order is not yet needed */
typedef int (*operacao_fn)(int, int);
int aplicar(operacao_fn fn, int a, int b) { return fn(a, b); }

/* CORRECT */
int r = a + b;
```

### 14.2 YAGNI — You Aren't Gonna Need It

Build what is required **now**. Future requirements arrive with their own context.

**Rules:**
- MUST NOT implement features, flags, or extension points for hypothetical future use.
- MUST NOT add configuration options that no current caller uses.
- MUST NOT generalize a function until the third real use case exists (see DRY § 6.7).

```typescript
// INCORRECT — plugin system nobody asked for
interface Processador<T, R> {
  processar(input: T, opcoes?: ProcessadorOpcoes): Promise<R>;
  cancelar?(): void;
  progresso?(): number;
}

// CORRECT — solve the actual problem
async function processarPedido(pedido: Pedido): Promise<Resultado> { ... }
```

```rust
// INCORRECT — enum arms that don't exist yet
enum Formato { Json, Csv, Xml, Parquet, Avro }  // Parquet/Avro: YAGNI

// CORRECT — add arms when requirements arrive
enum Formato { Json, Csv }
```

### 14.3 Fail Fast

Detect violations of preconditions as early as possible — at the system boundary, not buried in domain logic.

**Rules:**
- MUST validate all external input at the entry point (API, CLI, file, IPC, queue).
- MUST NOT silently coerce invalid input into a valid-looking value.
- MUST crash or return error immediately when an invariant is violated — never defer.
- MUST include the violated condition and the actual value in the error message.
- MUST NOT use default values to mask missing required input.

```python
# INCORRECT — silent coercion hides bad input
def processar_quantidade(qtd):
    qtd = max(0, qtd or 0)

# CORRECT — fail at the boundary
def processar_quantidade(qtd: int) -> None:
    if qtd is None:
        raise ValueError("quantidade é obrigatória")
    if qtd <= 0:
        raise ValueError(f"quantidade deve ser positiva, recebido: {qtd}")
```

```c
/* CORRECT — assert for programming-error invariants */
#include <assert.h>
void processar_pacote(const uint8_t *dados, size_t n) {
    assert(dados != NULL);
    assert(n > 0 && n <= MAX_PACOTE);
}
```

```rust
// CORRECT — Result at boundaries, panic only for programming errors
fn processar_quantidade(qtd: i32) -> Result<(), String> {
    if qtd <= 0 {
        return Err(format!("quantidade deve ser positiva, recebido: {qtd}"));
    }
    Ok(())
}
```

```javascript
// CORRECT — Node.js: validate at route handler boundary
function processarPedido(req, res) {
    const { quantidade } = req.body;
    if (quantidade == null)
        return res.status(400).json({ erro: 'quantidade obrigatória' });
    if (!Number.isInteger(quantidade) || quantidade <= 0)
        return res.status(400).json({ erro: `quantidade inválida: ${quantidade}` });
}
```

### 14.4 Law of Demeter — Principle of Least Knowledge

A unit MUST only talk to its immediate collaborators. No reaching through the object graph.

**The "one dot" rule:** `a.fazAlgo()` is fine. `a.getB().getC().fazAlgo()` is a violation.

**Rules:**
- MUST NOT chain more than one method/property access on a foreign object.
- MUST NOT reach through an object to access its internals — ask the object to act.
- SHOULD expose behavior, not structure (Tell, Don't Ask).

```python
# INCORRECT
preco_total = pedido.cliente.endereco.cidade.imposto * pedido.valor

# CORRECT
preco_total = pedido.calcular_total_com_imposto()
```

```cpp
// INCORRECT
double aliquota = pedido->getCliente()->getEndereco()->getCidade()->getAliquota();

// CORRECT
double imposto = pedido->calcularImposto();
```

```typescript
// INCORRECT
const desconto = carrinho.usuario.fidelidade.nivel.desconto;

// CORRECT
const desconto = carrinho.calcularDesconto();
```

### 14.5 CQS — Command-Query Separation

A function either **changes state** (command) or **returns data** (query). Never both.

**Rules:**
- Commands MUST return `void` (or `Result`/`Error` for success/failure only — no business data).
- Queries MUST be pure: same input → same output, no side effects.
- MUST NOT have a function that returns meaningful data AND produces a side effect.
- Exception: language-idiomatic patterns like `pop()` (stack), `next()` (iterator) are accepted.

```python
# INCORRECT — query with side effect
def buscar_proximo(fila):
    pedido = fila[0]
    fila.remove(pedido)  # side effect + return value mixed
    return pedido

# CORRECT
def peek_proximo(fila) -> Pedido:   return fila[0]     # query
def remover_proximo(fila) -> None:  fila.pop(0)        # command
```

```rust
// Rust makes this natural via &self vs &mut self
impl Fila {
    fn proximo(&self) -> Option<&Pedido> { ... }       // query
    fn remover(&mut self) -> Option<Pedido> { ... }    // command
}
```

```typescript
// INCORRECT — command returning business data
function salvarUsuario(u: Usuario): Usuario {
    db.insert(u);
    return { ...u, id: db.lastInsertId() };
}

// CORRECT
function salvarUsuario(u: Usuario): void { db.insert(u); }
function buscarUsuario(id: number): Usuario { return db.find(id); }
```

### 14.6 Composition over Inheritance

Prefer assembling behavior from small collaborators over deep class hierarchies.

**Rules:**
- MUST NOT create inheritance hierarchies deeper than 2 levels (base + one concrete).
- MUST NOT use inheritance to share implementation — use composition or free functions.
- SHOULD use interfaces/traits/protocols to define behavior contracts.

```python
# INCORRECT — fragile 3-level hierarchy
class Exportador:          def exportar(self): ...
class ExportadorCSV(Exportador): def exportar(self): ...
class ExportadorCSVComHeader(ExportadorCSV): def exportar(self): super().exportar(); ...

# CORRECT — composition
class ExportadorCSV:
    def __init__(self, formatador: Formatador, escritor: Escritor): ...
    def exportar(self, dados): self._escritor.escrever(self._formatador.formatar(dados))
```

```rust
// CORRECT — traits for behavior, structs for composition
trait Serializavel { fn serializar(&self) -> String; }
trait Persistivel  { fn persistir(&self) -> Result<(), DbErro>; }

struct Repositorio<T: Serializavel + Persistivel> { item: T }
```

### 14.7 Immutability by Default

Treat data as immutable unless there is a concrete reason to mutate it.

**Rules:**
- MUST declare variables as immutable (`const`, `val`, `let`, `final`, `const&`) by default.
- MUST NOT mutate function parameters.
- SHOULD return new values instead of modifying existing ones in domain logic.
- Shared mutable state MUST be protected by a synchronization primitive (mutex, atomic, lock).

```c
/* CORRECT — const parameters prevent accidental mutation */
double calcular_area(const double largura, const double altura) {
    return largura * altura;
}
```

```python
# INCORRECT — mutates caller's data
def normalizar(lista):
    lista.sort()
    return lista

# CORRECT — returns new value
def normalizar(lista: list) -> list:
    return sorted(lista)
```

```rust
let preco = 100.0;      // immutable by default
let mut estoque = 50;   // explicit mutation opt-in
```

```typescript
// CORRECT — readonly prevents accidental mutation
function calcularTotal(itens: readonly Item[]): number {
    return itens.reduce((acc, i) => acc + i.preco, 0);
}
```

### 14.8 Explicit over Implicit

Behavior MUST be visible at the call site. Magic and hidden side effects are failure modes.

**Rules:**
- MUST NOT rely on global mutable state or thread-local singletons silently affecting behavior.
- MUST pass dependencies explicitly (constructor/function parameters), not via globals.
- Configuration that affects runtime behavior MUST be explicit, not inferred from environment magic.
- MUST NOT use hidden default arguments that change behavior without callers knowing.

```python
# INCORRECT — invisible dependency
_config = {}
def buscar_item(nome):
    url = _config['url']  # where does this come from?

# CORRECT — explicit
def buscar_item(nome: str, base_url: str) -> Item: ...
```

```c
/* INCORRECT — static mutable state, invisible dependency */
static char ultimo_erro[256];
int processar() { strcpy(ultimo_erro, "falhou"); return -1; }

/* CORRECT — error propagated explicitly */
int processar(char *erro_out, size_t erro_len) {
    snprintf(erro_out, erro_len, "falhou: razão X");
    return -1;
}
```

### 14.9 High Cohesion, Low Coupling

- **Cohesion:** everything inside a module belongs together — one clear purpose.
- **Coupling:** modules depend on each other as little as possible, and only through stable interfaces.

**Rules:**
- MUST NOT place logic in a module because it is *convenient*, only because it *belongs*.
- Coupling between modules MUST go through interfaces, not concrete types.
- A module MUST be testable in isolation without instantiating the full system.
- Circular dependencies between modules MUST NOT exist.

```
CORRECT dependency graph:       INCORRECT:
  ui → service → repo             ui ↔ service     (circular)
            ↓                     repo → ui         (layer inversion)
          domain
```

### 14.10 Idempotency

An operation that can be retried N times MUST produce the same result as running it once.

**Rules:**
- MUST design write operations (HTTP PUT/DELETE, DB upserts, file overwrites) to be idempotent.
- MUST NOT accumulate state on repeated calls (e.g., double-append on retry).
- SHOULD use idempotency keys for operations that cannot be made naturally idempotent.
- MUST test the "call twice" scenario for all mutation endpoints.

```python
# INCORRECT — double call charges twice
def cobrar_cliente(cliente_id, valor):
    db.execute("INSERT INTO cobranças VALUES (?, ?)", cliente_id, valor)

# CORRECT — idempotent via unique key
def cobrar_cliente(cliente_id, valor, idempotency_key: str):
    db.execute("""
        INSERT INTO cobranças (cliente_id, valor, chave)
        VALUES (?, ?, ?)
        ON CONFLICT (chave) DO NOTHING
    """, cliente_id, valor, idempotency_key)
```

```typescript
// PUT is idempotent — safe to retry
// PUT /usuarios/:id   → replace resource, same result every time
// POST /usuarios      → creates new; NOT safe to retry without idempotency key
```

### 14.11 Tell, Don't Ask

Don't query an object's state to make a decision externally — tell the object to act and let it decide internally.

> Distinct from Law of Demeter (§ 14.4): LoD governs *how far* you reach into the object graph; TDA governs *where decisions live*. Both can be violated independently.

**Rules:**
- MUST NOT extract state from an object, compute a decision outside, then push the result back in.
- MUST place the decision inside the object that owns the relevant data.
- SHOULD expose behavior-revealing methods (`aprovar()`, `descontar()`) over state-revealing getters (`getStatus()`, `getValor()`).

```python
# INCORRECT — decision made outside the object that owns the data
if pedido.get_status() == 'pendente' and pedido.get_valor() > 0:
    pedido.set_status('aprovado')

# CORRECT — tell the object to act; it owns the rule
pedido.aprovar()  # validates status and value internally
```

```cpp
// INCORRECT
if (conta.getSaldo() >= valor && conta.getAtiva()) {
    conta.setSaldo(conta.getSaldo() - valor);
}

// CORRECT
conta.debitar(valor);  // conta validates its own invariants
```

```rust
// INCORRECT
if pedido.status == Status::Pendente && pedido.valor > 0.0 {
    pedido.status = Status::Aprovado;
}

// CORRECT
pedido.aprovar()?;  // returns Err if invariants not met
```

```typescript
// INCORRECT
if (usuario.getPerfil() === 'admin' && usuario.getAtivo()) {
    usuario.setPerfil('superadmin');
}

// CORRECT
usuario.promoverParaSuperAdmin();  // encapsulates the promotion rule
```

### 14.12 POLA — Principle of Least Astonishment

A function, method, or API MUST behave exactly as its name and signature lead the caller to expect. Surprising behavior is a bug, even when documented.

**Rules:**
- MUST NOT perform side effects that the name does not indicate (`buscar*` MUST NOT write; `calcular*` MUST NOT mutate).
- MUST NOT return a different type or shape depending on a hidden flag or global state.
- MUST NOT silently ignore parameters — if a parameter is accepted, it MUST affect behavior.
- MUST use names that reveal what the unit does at the abstraction level of the caller.
- Boolean parameters that change the *kind* of operation MUST be replaced by two separate functions.

```python
# INCORRECT — name says "buscar", behavior secretly writes cache
def buscar_item(nome: str) -> Item:
    item = api.get(nome)
    db.salvar(item)      # surprise side effect
    return item

# CORRECT — make the side effect explicit in the name or split into two
def buscar_e_cachear_item(nome: str) -> Item: ...
# or
def buscar_item(nome: str) -> Item: ...         # pure fetch
def cachear_item(item: Item) -> None: ...      # explicit cache write
```

```c
/* INCORRECT — boolean flag that inverts the operation entirely */
int processar(int valor, int reverso);  /* reverso=1 undoes the operation */

/* CORRECT — two functions with distinct names */
int processar(int valor);
int reverter(int valor);
```

```typescript
// INCORRECT — silent parameter ignore
function paginar(itens: Item[], pagina: number, tamanho: number): Item[] {
    return itens.slice(0, 10);  // ignores pagina and tamanho!
}

// INCORRECT — different return shape based on hidden flag
function buscar(id: number): Usuario | null | { erro: string } { ... }

// CORRECT — consistent return type, parameters always honored
function buscar(id: number): Usuario | null { ... }
```

```rust
// INCORRECT — method named "len" that sorts as a side effect
impl Lista {
    fn len(&mut self) -> usize {  // &mut self on a query = red flag
        self.itens.sort();        // surprise mutation
        self.itens.len()
    }
}

// CORRECT
fn len(&self) -> usize { self.itens.len() }
fn ordenar(&mut self) { self.itens.sort(); }
```

---

## 15. API Design — REST

> Applies to any HTTP API regardless of stack. For per-stack checklists see [AUDITORIAS.md](AUDITORIAS.md).

### 15.1 Resource Naming & HTTP Verbs

- URLs MUST use nouns, not verbs. MUST NOT: `/getUser`, `/deleteOrder`, `/doLogin`.
- Resources MUST be plural: `/users`, `/orders`, `/products`.
- MUST use verbs as HTTP methods, not URL segments.

| Method | Semantics | Idempotent |
|--------|-----------|------------|
| `GET` | Read only — MUST NOT change state | Yes |
| `POST` | Create resource | No |
| `PUT` | Full replacement of resource | Yes |
| `PATCH` | Partial update | No |
| `DELETE` | Remove resource — returns `204 No Content` | Yes |

```
✅ GET    /api/v1/users
✅ POST   /api/v1/users
✅ GET    /api/v1/users/{id}
✅ PUT    /api/v1/users/{id}
✅ DELETE /api/v1/users/{id}

❌ GET  /api/getUser?id=1
❌ POST /api/deleteOrder
❌ GET  /api/users/doActivate
```

### 15.2 Status Codes

MUST use semantically correct status codes. MUST NOT return `200` for errors.

| Situation | Correct Code | Common Anti-Pattern |
|-----------|-------------|---------------------|
| Resource created | `201 Created` | `200` on POST |
| No body to return | `204 No Content` | `200` with `{}` |
| Resource not found | `404 Not Found` | `200` with error in body |
| Validation failed | `422 Unprocessable Entity` | generic `400` |
| Not authenticated | `401 Unauthorized` | `403` |
| Authenticated, no permission | `403 Forbidden` | `401` |
| Rate limit exceeded | `429 Too Many Requests` | `400` |
| Server error | `500 Internal Server Error` | `200` with `success: false` |

### 15.3 Versioning & Response Format

- MUST version APIs from day one: `/api/v1/`.
- MUST use consistent response envelope across all endpoints.

```json
// ✅ Success
{ "data": { "id": "123", "nome": "...", "criado_em": "2024-01-15T14:30:00Z" },
  "meta": { "request_id": "a1b2c3d4" } }

// ✅ Paginated list
{ "data": [ {...} ],
  "meta": { "total": 150, "per_page": 20, "current_page": 1, "last_page": 8 } }

// ✅ Error
{ "error": { "code": "VALIDATION_ERROR",
             "message": "Dados inválidos.",
             "details": { "email": ["E-mail já em uso."] } },
  "meta": { "request_id": "a1b2c3d4" } }

// ❌ Inconsistent — never mix formats
{ "success": false, "msg": "error", "data": null }
```

**Rules:**
- MUST NOT expose internal fields: `password_hash`, `deleted_at`, internal IDs.
- Dates MUST be ISO 8601 (`2024-01-15T14:30:00Z`).
- Numeric IDs SHOULD be sent as strings in JSON (avoids JS precision loss for large IDs).
- Every list endpoint MUST be paginated with a configurable `per_page` and a hard maximum.

### 15.4 Authentication & Rate Limiting

- MUST authenticate API endpoints via JWT or API Key — MUST NOT use cookie sessions.
- JWT access tokens MUST expire in ≤ 1 hour. Refresh tokens MUST rotate on use.
- Tokens MUST NOT appear in URLs (`?token=xxx`). MUST use `Authorization: Bearer {token}`.
- MUST implement rate limiting on all public endpoints. Auth endpoints MUST have stricter limits.
- MUST return `X-RateLimit-Limit`, `X-RateLimit-Remaining`, `Retry-After` headers.
- CORS MUST NOT use `Access-Control-Allow-Origin: *` on authenticated APIs. MUST use an explicit allowlist.

### 15.5 Input Validation

- MUST validate every request field at the API boundary before processing (Fail Fast § 14.3).
- MUST validate data types, max lengths, and allowed values server-side — never trust client.
- MUST NOT allow SQL-injectable values through filter/sort parameters — use allowlists.
- File upload endpoints MUST validate MIME type and file size server-side.

### 15.6 Documentation

- MUST maintain an OpenAPI 3.0 spec kept in sync with the implementation.
- MUST document every endpoint with request/response examples and all error codes.
- Swagger UI SHOULD be accessible at `/api/docs` in non-production environments only.

---

## 16. Logging & Observability

> Applies across all stacks. For stack-specific tooling see [AUDITORIAS.md](AUDITORIAS.md).

### 16.1 Structured Logging

- MUST emit logs in structured JSON format — not free-form strings.
- MUST use standard severity levels: `DEBUG`, `INFO`, `WARNING`, `ERROR`, `CRITICAL`.
- MUST include stack trace on every `ERROR` and `CRITICAL` entry.
- MUST propagate a unique `request_id` across all log entries for the same request.
- MUST NOT log PII: passwords, CPF, credit card numbers, tokens, session IDs (LGPD Art. 46).
- MUST define log rotation policy and retention period.

```json
// ✅ Correct structured log
{ "level": "error", "timestamp": "2024-01-15T14:30:00Z",
  "request_id": "a1b2c3d4", "message": "Falha ao processar pagamento",
  "order_id": 4521, "gateway": "pagarme", "error_code": 402 }

// ❌ Free-form string — unsearchable, unparseable
"ERRO: pagamento falhou para order 4521 no pagarme com código 402"
```

### 16.2 Health Endpoint

- MUST expose a `/health` endpoint that returns `200` when the service is operational.
- MUST check all critical dependencies (database, cache, external queue) in the health response.
- MUST return `503 Service Unavailable` if any critical dependency is unhealthy.
- Health endpoint MUST NOT require authentication.

```json
// GET /health → 200
{ "status": "ok", "timestamp": "2024-01-15T14:30:00Z",
  "checks": { "database": "ok", "cache": "ok", "disk": "ok" } }

// GET /health → 503 (degraded)
{ "status": "degraded",
  "checks": { "database": "ok", "cache": "fail", "disk": "ok" } }
```

### 16.3 SLOs & Alerts

Define and monitor these minimum SLOs for production services:

| Signal | Target | Alert threshold |
|--------|--------|-----------------|
| Error rate (5xx) | < 0.1% | > 1% over 5 min |
| Response time p95 | < 1 s | > 2 s |
| Memory usage | < 80% | > 85% |
| Uptime | ≥ 99.5% | Any downtime |

- MUST instrument the three golden signals: **latency**, **error rate**, **saturation**.
- MUST NOT ship to production without at least one alert configured for error rate.

---

## 17. LGPD Compliance Baseline

> Lei Geral de Proteção de Dados — Lei 13.709/2018.
> Applies to any project that collects, stores, or processes personal data of Brazilian individuals.
> For full compliance checklists see [AUDITORIAS.md](AUDITORIAS.md) — section III7.

### 17.1 Data Minimization (Art. 6º, I e III)

- MUST collect only data strictly necessary for the declared functionality.
- MUST NOT store data beyond the legally required retention period.
- MUST implement a hard-delete or anonymization mechanism for expired data.
- MUST support the user's right to erasure (Art. 18, VI) and data portability (Art. 18, V).

### 17.2 Data Protection (Art. 46)

- MUST encrypt sensitive personal data at rest (CPF, e-mail, phone, financial data).
- MUST store credentials, API keys, and secrets exclusively in environment variables — MUST NOT commit them.
- MUST use the principle of least privilege for database users.
- MUST back up encrypted data and periodically test restoration.
- MUST log access to personal data for audit trail (Art. 37) — without logging the data itself.

### 17.3 No PII in Logs or Error Messages

- MUST NOT log personal data in any log level (see § 16.1).
- MUST NOT include personal data in exception messages or stack traces.
- MUST NOT expose personal data in API error responses (e.g., "CPF 123 already in use").

### 17.4 Incident Response (Art. 48)

LGPD requires notifying ANPD and affected data subjects within **72 hours** of discovering a breach.

| Phase | Action | Deadline |
|-------|--------|----------|
| 1 | Contain: revoke tokens, block IPs, take endpoint offline | Immediate |
| 2 | Assess scope: which data, how many subjects | < 12 h |
| 3 | Notify management / DPO | < 24 h |
| 4 | Notify ANPD at portal.anpd.gov.br | **< 72 h** |
| 5 | Notify affected data subjects | **< 72 h** |
| 6 | Post-mortem and pipeline hardening | < 7 days |

- MUST designate a Data Protection Officer (DPO / Encarregado) before going to production.
- MUST document the data processing record (ROPA — Art. 37) before collecting personal data.

---

*This contract is the authoritative reference for all code written in this project.*
*Last updated: 2026-04-29 — sync with TESTES.md for testing procedures.*
