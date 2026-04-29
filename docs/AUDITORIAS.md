# Auditorias Técnicas — Referência Completa

> Checklists operacionais de auditoria para projetos C++ · Python · Web (PHP/JS/HTML).
> Cada item tem prioridade **🔴 CRÍTICO · 🟠 IMPORTANTE · 🟢 COSMÉTICO**.
>
> Para princípios e regras de desenvolvimento: [CONTRACT.md](CONTRACT.md)
> Para procedimentos de teste automatizados: [TESTES.md](TESTES.md)

---

## Índice

### Parte I — C++ / Qt6 / MySQL
1. [Índice Mestre e Roadmap](#i1--índice-mestre-e-roadmap)
2. [Arquitetura — 4 Camadas, SOLID, DRY, TDD](#i2--arquitetura--4-camadas-solid-dry-tdd)
3. [Segurança — Memory Safety, SQL Injection, Binário, LGPD](#i3--segurança--memory-safety-sql-injection-binário-lgpd)
4. [C++23 Moderno — Tipos, Concorrência, Move Semantics](#i4--c23-moderno--tipos-concorrência-move-semantics)
5. [Qt6 Específico — Signals/Slots, Model/View, QML, i18n](#i5--qt6-específico--signalsslots-modelview-qml-i18n)
6. [MySQL — Schema, Queries, EXPLAIN, Migrations, LGPD](#i6--mysql--schema-queries-explain-migrations-lgpd)
7. [Performance — Event Loop, Alocações, Profiling](#i7--performance--event-loop-alocações-profiling)
8. [DevOps & Build — CMake, GitHub Actions, CPack](#i8--devops--build--cmake-github-actions-cpack)
9. [Qualidade de Código — God Classes, Complexidade, Dead Code](#i9--qualidade-de-código--god-classes-complexidade-dead-code)

### Parte II — Python / Qt6 / SQLite
10. [Auditoria Completa Python/Qt6](#ii1--auditoria-completa-pythonqt6)
11. [Arquitetura Python — 4 Camadas, SOLID, DRY, TDD](#ii2--arquitetura-python--4-camadas-solid-dry-tdd)
12. [Segurança Python — SQL Injection, Senhas, LGPD](#ii3--segurança-python--sql-injection-senhas-lgpd)
13. [Python Moderno — Type Hints, Async, Dataclasses](#ii4--python-moderno--type-hints-async-dataclasses)

### Parte III — Web (PHP / JavaScript / HTML / MySQL)
14. [Arquitetura 4 Camadas Web — SOLID, DRY, TDD com PHPUnit](#iii1--arquitetura-4-camadas-web--solid-dry-tdd-com-phpunit)
15. [Qualidade e Dívida Técnica PHP/JS](#iii2--qualidade-e-dívida-técnica-phpjs)
16. [Performance & Observabilidade — Core Web Vitals, OPcache, Logging](#iii3--performance--observabilidade--core-web-vitals-opcache-logging)
17. [DevOps & Deploy — Docker, CI/CD, Zero-Downtime](#iii4--devops--deploy--docker-cicd-zero-downtime)
18. [API Design REST — Verbos, Status Codes, Auth, OpenAPI](#iii5--api-design-rest--verbos-status-codes-auth-openapi)
19. [Acessibilidade, SEO, i18n, Dependências, Documentação](#iii6--acessibilidade-seo-i18n-dependências-documentação)
20. [Hardening PHP + LGPD — Pipeline OWASP ZAP, Incidentes](#iii7--hardening-php--lgpd--pipeline-owasp-zap-incidentes)

---

# Parte I — C++ / Qt6 / MySQL


---

## I1 — Índice Mestre e Roadmap

# 📋 Índice Mestre de Auditorias
## Stack: C++23 · Qt6 · MySQL

| Versão | Atualização | Escopo |
|--------|------------|--------|
| 1.0 | 2024 | Todos os documentos de auditoria |

---

## Documentos de Auditoria

| # | Documento | Foco Principal | Prioridade de Execução |
|---|-----------|---------------|----------------------|
| 1 | `cpp_qt6_auditoria_seguranca.md` | Memory safety, SQL Injection, flags de compilação, sanitizers, LGPD | 🔴 Primeiro |
| 2 | `cpp_qt6_auditoria_arquitetura.md` | 4 camadas, SOLID, DRY, TDD, GTest/GMock, QTest | 🔴 Segundo |
| 3 | `cpp_qt6_auditoria_cpp23_moderno.md` | C++20/23 features, smart pointers, RAII, concorrência, move semantics | 🔴 Terceiro |
| 4 | `cpp_qt6_auditoria_qt6_especifico.md` | Widgets, layouts, Model/View, QML, signals/slots, i18n pt-BR | 🔴 Quarto |
| 5 | `cpp_qt6_auditoria_mysql.md` | Schema, queries, transações, índices, migrations, backup | 🟠 Quinto |
| 6 | `cpp_qt6_auditoria_performance.md` | Event loop, N+1, profiling, LTO, crash reports, logging | 🟠 Sexto |
| 7 | `cpp_qt6_auditoria_devops_build.md` | CMake, CI/CD, empacotamento, versionamento | 🟠 Sétimo |
| 8 | `cpp_qt6_auditoria_qualidade_codigo.md` | Code smells, complexidade ciclomática, nomenclatura, dead code | 🟢 Oitavo |

---

## Prompt Mestre — Auditoria Completa para Claude Opus

> Cole na raiz do projeto no Claude Code com o modelo `claude-opus-4-5`.

```
Faça uma auditoria completa e abrangente deste projeto C++23/Qt6/MySQL.
Reporte TUDO em português. Seja preciso, técnico e impiedoso — encontre
todos os problemas reais, não apenas os óbvios.

═══════════════════════════════════════
BLOCO 1 — SEGURANÇA (avaliar primeiro)
═══════════════════════════════════════

## SQL Injection
- Há QSqlQuery sem prepare()/bindValue()?
- Há concatenação de QString em queries SQL?

## Memory Safety C++
- Há raw new/delete sem smart pointer?
- Há raw pointers com ownership implícito?
- Há risco de use-after-free ou double-free?
- Regra dos 5/0 seguida em classes com recursos?

## Segurança do Binário
- Flags -fstack-protector-strong, -D_FORTIFY_SOURCE, -fPIE presentes?
- Sanitizers (ASan/UBSan/TSan) rodando no CI?

## Dados e LGPD
- Credenciais hardcoded no código?
- Dados pessoais logados em logs?
- Criptografia de dados sensíveis em repouso?

═══════════════════════════════════════
BLOCO 2 — ARQUITETURA
═══════════════════════════════════════

## 4 Camadas
- Há QSqlQuery dentro de QWidget ou slots de UI?
- Services dependem de interfaces ou de classes concretas?
- Toda query SQL está confinada em Repositories?
- Domain layer tem dependência de Qt ou infraestrutura?

## SOLID
- Identifique as 3 piores violações de SRP
- Há DIP violado (new ConcreteClass() dentro de Services)?
- Interfaces são coesas (ISP) ou há GodInterface?

## TDD
- Há testes unitários para Services (GTest + GMock)?
- QSignalSpy usado para testar signals?
- Qual é a cobertura estimada por camada?

═══════════════════════════════════════
BLOCO 3 — C++23 MODERNO
═══════════════════════════════════════

- Há NULL em vez de nullptr?
- Há raw arrays C em vez de std::array/std::vector?
- std::optional usado para retornos opcionais?
- enum class em vez de enum simples?
- Há funções perigosas: strcpy, sprintf, gets?
- Move semantics implementado onde necessário?
- Thread safety: dados compartilhados protegidos?

═══════════════════════════════════════
BLOCO 4 — QT6 ESPECÍFICO
═══════════════════════════════════════

- Há setFixedSize() ou coordenadas absolutas?
- Connections usam sintaxe de ponteiro de membro?
- Há lambdas sem contexto (dangling reference)?
- Há conexões duplicadas (connect() múltiplas vezes)?
- QAbstractItemModel usado para listas e tabelas?
- beginInsertRows/endInsertRows emitidos corretamente?
- Cross-thread signals com QueuedConnection?
- Todas as strings de UI em tr()?
- QLocale configurado para pt-BR?
- Timezone configurado (America/Sao_Paulo)?

═══════════════════════════════════════
BLOCO 5 — BANCO DE DADOS
═══════════════════════════════════════

- Charset utf8mb4 em todas as tabelas?
- FLOAT/DOUBLE para dinheiro (deve ser DECIMAL)?
- Há N+1 queries (loop com query por iteração)?
- EXPLAIN executado nas queries mais frequentes?
- Há full table scan em tabelas > 1000 linhas?
- Schema versionado com migrations?
- Backup automático e criptografado configurado?

═══════════════════════════════════════
BLOCO 6 — PERFORMANCE
═══════════════════════════════════════

- Há operações bloqueantes na event loop do Qt?
- QApplication::processEvents() usado como gambiarra?
- data() do Model executa query por chamada?
- QStringLiteral usado para literais de string?
- LTO (-flto) habilitado no build de release?

═══════════════════════════════════════
BLOCO 7 — BUILD E DEVOPS
═══════════════════════════════════════

- CMAKE_CXX_STANDARD 23 configurado?
- Biblioteca separada do executável (para testes)?
- CI/CD automatizado (GitHub Actions ou equivalente)?
- Análise estática (clang-tidy/cppcheck) no CI?
- Build de release com empacotamento funcional?
- Qt6 DLLs/SOs incluídas no pacote?

═══════════════════════════════════════
BLOCO 8 — QUALIDADE DE CÓDIGO
═══════════════════════════════════════

- Identifique as 5 maiores classes (God Classes)
- Identifique os 5 métodos de maior complexidade ciclomática
- Há Magic Numbers ou Magic Strings?
- Há Primitive Obsession (double para dinheiro)?
- Convenção de nomenclatura é consistente?
- Há includes desnecessários nos headers?

═══════════════════════════════════════
DIAGNÓSTICO FINAL
═══════════════════════════════════════

1. Lista priorizada de TODOS os problemas encontrados:
   - CRÍTICO (bloqueia deploy / risco de segurança / data loss)
   - IMPORTANTE (degradação significativa de qualidade)
   - COSMÉTICO (melhoria de legibilidade/manutenibilidade)

2. Top 10 ações de maior impacto (ordenadas por ROI)

3. Estimativa de esforço por grupo:
   - CRÍTICO: X horas/dias
   - IMPORTANTE: X horas/dias
   - COSMÉTICO: X horas/dias

4. Roadmap sugerido de sprints (2 semanas cada)
```

---

## Ordem de Execução dos Sprints (Roadmap Consolidado)

| Sprint | Semana | Blocos | Objetivo | Critério de done |
|--------|--------|--------|----------|-----------------|
| 🔴 1 | 1-2 | Seg + Arq | SQL Injection zero + separar camadas | prepare() em 100% das queries; QSqlQuery fora de QWidget |
| 🔴 2 | 3-4 | C++ + Seg | Memory safety + flags de compilação | ASan sem erros; checksec confirma proteções |
| 🔴 3 | 5-6 | Qt6 + Arq | Signals modernos + DIP nos Services | Grep sem SIGNAL(; Services com interfaces |
| 🟠 4 | 7-8 | TDD | Testes unitários nos Services | Cobertura ≥ 60% com GMock |
| 🟠 5 | 9-10 | MySQL + Perf | Índices + N+1 eliminado | EXPLAIN sem full table scan |
| 🟠 6 | 11-12 | DevOps | CI/CD completo + sanitizers | Pipeline verde multi-plataforma |
| 🟠 7 | 13-14 | Qt6 + i18n | tr() em tudo + QLocale pt-BR + layouts | lupdate sem strings; funciona redimensionado |
| 🟢 8 | 15-16 | Qualidade | Complexidade reduzida + nomenclatura | CC máxima ≤ 15; nenhuma God Class |
| 🟢 9 | 17-18 | Empacotamento | CPack + AppImage testado em máquina limpa | Deploy funcional |
| 🟢 10 | 19-20 | Observabilidade | Logging + crash report + métricas | Stack trace em todo crash |

---

## Ferramentas por Domínio

| Domínio | Ferramenta | Instalação | Uso |
|---------|-----------|-----------|-----|
| Análise estática C++ | clang-tidy | `sudo apt install clang-tidy` | `run-clang-tidy -p build/` |
| Análise estática C++ | cppcheck | `sudo apt install cppcheck` | `cppcheck --enable=all src/` |
| Funções perigosas | Flawfinder | `pip install flawfinder` | `flawfinder src/` |
| Memory bugs | AddressSanitizer | built-in GCC/Clang | `-fsanitize=address` |
| Thread bugs | ThreadSanitizer | built-in GCC/Clang | `-fsanitize=thread` |
| UB bugs | UBSan | built-in GCC/Clang | `-fsanitize=undefined` |
| Leaks de memória | Valgrind | `sudo apt install valgrind` | `valgrind --tool=memcheck ./app` |
| Profiling de heap | Heaptrack | `sudo apt install heaptrack` | `heaptrack ./app` |
| Profiling de CPU | Valgrind/Callgrind | `sudo apt install valgrind` | `valgrind --tool=callgrind ./app` |
| Complexidade | lizard | `pip install lizard` | `lizard src/ -l cpp -C 10` |
| Includes | iwyu | `sudo apt install iwyu` | Via CMake |
| Cobertura | lcov | `sudo apt install lcov` | `lcov --capture --directory build` |
| Testes unitários | GTest/GMock | vcpkg ou `apt install libgtest-dev` | `find_package(GTest)` |
| Testes Qt | QTest | Incluído no Qt6 | `find_package(Qt6 COMPONENTS Test)` |
| Análise semântica | CodeQL | GitHub Actions | `github/codeql-action` |
| Segredos no git | TruffleHog | `pip install trufflehog` | `trufflehog git file://.` |
| Slow queries | pt-query-digest | Percona Toolkit | `pt-query-digest slow.log` |

---

*Índice mestre — Claude Opus — uso interno confidencial*

---

## I2 — Arquitetura — 4 Camadas, SOLID, DRY, TDD

# 🏛️ Auditoria de Arquitetura — 4 Camadas + SOLID + DRY + TDD
## Stack: C++23 · Qt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | C++23 / Qt6 / MySQL |

---

## 1. Arquitetura em 4 Camadas

```
┌─────────────────────────────────────────────────────────┐
│  LAYER 1 — PRESENTATION   Qt6 Widgets / QML             │
│  Views, ViewModels, Delegates, UI state                  │
├─────────────────────────────────────────────────────────┤
│  LAYER 2 — APPLICATION    Services / Use Cases           │
│  Regras de negócio, orquestração, DTOs                   │
├─────────────────────────────────────────────────────────┤
│  LAYER 3 — DOMAIN         Entities / Repositories        │
│  Modelos de domínio, interfaces, value objects           │
├─────────────────────────────────────────────────────────┤
│  LAYER 4 — INFRASTRUCTURE MySQL / QSql / Arquivos        │
│  Implementações concretas de repositórios, I/O           │
└─────────────────────────────────────────────────────────┘
```

### Regra de dependência obrigatória

```
PRESENTATION → APPLICATION (via interfaces / signals)
APPLICATION  → DOMAIN      (via interfaces puras)
DOMAIN       → nada        (zero dependências externas)
INFRASTRUCTURE → DOMAIN    (implementa interfaces do domain)
```

> ⚠️ **Violação típica de IA fraca em Qt6:** `QSqlQuery` diretamente dentro de slots de `QWidget`, sem Repository, sem Service — as 4 camadas colapsadas em um único arquivo `.cpp`.

---

## 2. Checklist por Camada

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

### 2.1 Layer 1 — Presentation (Qt6 Widgets / QML)

#### Separação View / ViewModel

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Widgets não contêm lógica de negócio — apenas apresentação e input | 🔴 CRÍTICO | SRP |
| ☐ | Sem `QSqlQuery` ou acesso a banco dentro de qualquer `QWidget` | 🔴 CRÍTICO | Camadas |
| ☐ | ViewModels/Presenters separados das Views (MVVM ou MVP) | 🔴 CRÍTICO | SRP |
| ☐ | Slots de UI delegam para Services — não implementam lógica | 🔴 CRÍTICO | SRP |
| ☐ | `QAbstractItemModel` subclassificado para listas/tabelas — sem dados hardcoded | 🟠 IMPORTANTE | SRP / OCP |
| ☐ | Sem `QApplication::processEvents()` em loops de negócio | 🔴 CRÍTICO | SRP |

#### Signals e Slots

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Conexões usando sintaxe de ponteiro de membro — não SIGNAL()/SLOT() macro | 🔴 CRÍTICO | Modernidade |
| ☐ | Sem conexões duplicadas (chamar `connect()` múltiplas vezes sem `disconnect()`) | 🔴 CRÍTICO | Segurança |
| ☐ | Signals/slots cross-thread usando `Qt::QueuedConnection` | 🔴 CRÍTICO | Thread safety |
| ☐ | Objetos temporários não conectados a slots de objetos persistentes | 🔴 CRÍTICO | Lifetime |
| ☐ | `QObject::connect()` com terceiro argumento `this` (contexto) para desconexão automática | 🔴 CRÍTICO | Memory safety |

**Sintaxe correta de connect em Qt6:**

```cpp
// ✅ CORRETO — ponteiro de membro + contexto
connect(button, &QPushButton::clicked,
        this, &MyWidget::onButtonClicked);

// ✅ CORRETO — lambda com contexto
connect(button, &QPushButton::clicked,
        this, [this]() { handleClick(); });

// ❌ ERRADO — sintaxe Qt4, sem verificação em compile-time
connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

// ❌ ERRADO — lambda sem contexto (risco de dangling reference)
connect(button, &QPushButton::clicked,
        [this]() { handleClick(); }); // sem contexto!
```

#### Parent/Child e Ownership

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Todo `QObject` criado com `new` tem parent definido ou é gerenciado por smart pointer | 🔴 CRÍTICO | Memory safety |
| ☐ | Sem `QWidget` criado com `new` e esquecido sem parent | 🔴 CRÍTICO | Memory safety |
| ☐ | Layouts gerenciando ownership dos widgets corretamente | 🔴 CRÍTICO | Memory safety |
| ☐ | `QDialog` criado com parent para herdar ownership | 🟠 IMPORTANTE | Memory safety |

---

### 2.2 Layer 2 — Application (Services / Use Cases)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Classes de Service com responsabilidade única — não "GodService" | 🔴 CRÍTICO | SRP |
| ☐ | Services dependem de interfaces de Repository — não de implementações concretas | 🔴 CRÍTICO | DIP |
| ☐ | Services injetados via construtor — sem instanciação interna | 🔴 CRÍTICO | DIP |
| ☐ | Services sem referência a widgets ou UI | 🔴 CRÍTICO | SRP / Camadas |
| ☐ | DTOs (Data Transfer Objects) para comunicação entre camadas | 🟠 IMPORTANTE | SRP |
| ☐ | Services emitem signals ou usam callbacks para notificar a UI — não bloqueiam | 🟠 IMPORTANTE | SRP |
| ☐ | Operações longas em `QThread` ou `QtConcurrent` — não na thread principal | 🔴 CRÍTICO | Performance |

---

### 2.3 Layer 3 — Domain (Entities / Interfaces)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Interfaces puras (`= 0`) para todos os Repositories | 🔴 CRÍTICO | DIP / ISP |
| ☐ | Entidades de domínio sem dependência de Qt (sem `QObject` no domínio puro) | 🟠 IMPORTANTE | SRP / Camadas |
| ☐ | Value Objects para conceitos do domínio (Email, CPF, Money) | 🟠 IMPORTANTE | DRY |
| ☐ | Regras de invariância aplicadas nos construtores das entidades | 🟠 IMPORTANTE | SRP |
| ☐ | Domínio testável sem dependência de banco ou UI | 🔴 CRÍTICO | TDD |

---

### 2.4 Layer 4 — Infrastructure (MySQL / QSql)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Implementações de Repository isoladas na camada de infraestrutura | 🔴 CRÍTICO | DIP / Camadas |
| ☐ | Toda query SQL dentro dos Repositories — nunca em Services ou Views | 🔴 CRÍTICO | SRP |
| ☐ | `QSqlQuery::prepare()` + `bindValue()` em 100% das queries | 🔴 CRÍTICO | Segurança |
| ☐ | Transações explícitas para operações compostas | 🟠 IMPORTANTE | Consistência |
| ☐ | Erros de query verificados e propagados adequadamente | 🔴 CRÍTICO | Robustez |
| ☐ | Connection pool gerenciado centralmente | 🟠 IMPORTANTE | Performance |

**Estrutura de Repository correta:**

```cpp
// IUserRepository.h — Layer 3 (Domain)
class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<User> findById(int id) = 0;
    virtual std::vector<User> findAll() = 0;
    virtual bool save(const User& user) = 0;
    virtual bool remove(int id) = 0;
};

// MySqlUserRepository.h — Layer 4 (Infrastructure)
class MySqlUserRepository : public IUserRepository {
public:
    explicit MySqlUserRepository(QSqlDatabase& db);
    std::optional<User> findById(int id) override;
    // ...
private:
    QSqlDatabase& m_db;
    User mapRowToUser(const QSqlQuery& query) const;
};
```

---

## 3. SOLID — Checklist Detalhado

### 3.1 S — Single Responsibility Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `MainWindow` com 2000 linhas fazendo tudo | Extrair em ViewModel + Services | Presentation |
| ☐ | Slot que salva no banco, valida e atualiza UI | Separar em Service + Repository + slot de UI | Application |
| ☐ | Classe que renderiza tabela E busca dados no banco | QAbstractItemModel + Repository separados | Presentation / Infra |
| ☐ | Arquivo `.cpp` com mais de 300 linhas | Decompor em classes menores | Todas |

### 3.2 O — Open/Closed Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `if (type == "pdf") ... else if (type == "csv")` em exportação | Interface `IExporter` + implementações | Application |
| ☐ | Switch crescente para tipos de relatório | Strategy pattern via interface pura | Application |
| ☐ | Novo formato de arquivo exige editar parser existente | Herança ou composição via interface | Infra |

### 3.3 L — Liskov Substitution Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | Subclasse que lança exceção em método que pai não lançaria | Revisar hierarquia | Todas |
| ☐ | `MySqlUserRepository` não implementa todos os métodos de `IUserRepository` | Implementar completamente | Infra |
| ☐ | Override que retorna tipo mais restrito que o contrato da interface | Corrigir assinatura | Domain |

### 3.4 I — Interface Segregation Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | Interface com 20+ métodos | Quebrar em `IReadableRepository` + `IWritableRepository` | Domain |
| ☐ | Classe forçada a implementar métodos que não usa | Segregar interface | Domain |
| ☐ | Service dependendo de interface com métodos irrelevantes | Criar interface específica | Application |

### 3.5 D — Dependency Inversion Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `auto repo = std::make_unique<MySqlUserRepository>()` dentro de Service | Injetar via construtor | Application |
| ☐ | `MainWindow` instanciando Services diretamente | DI Container ou factory | Presentation |
| ☐ | Type hints em classes concretas em vez de interfaces | `IUserRepository*` não `MySqlUserRepository*` | Todas |

**Exemplo correto de DIP em C++23:**

```cpp
// ✅ CORRETO — depende de abstração
class UserService {
public:
    explicit UserService(
        std::shared_ptr<IUserRepository> repo,
        std::shared_ptr<IMailer> mailer)
        : m_repo(std::move(repo))
        , m_mailer(std::move(mailer)) {}

private:
    std::shared_ptr<IUserRepository> m_repo;
    std::shared_ptr<IMailer> m_mailer;
};

// ❌ ERRADO — depende de implementação concreta
class UserService {
public:
    UserService() {
        m_repo = std::make_unique<MySqlUserRepository>(); // concreto!
    }
};
```

---

## 4. DRY — Don't Repeat Yourself

### 4.1 DRY por camada

| Camada | Duplicação comum | Solução |
|--------|-----------------|---------|
| Presentation | Mesmo estilo de `QMessageBox::critical()` em 20 slots | Método `showError(const QString&)` em classe base | 
| Presentation | Mesma configuração de `QTableView` repetida | Helper `configureTable(QTableView*)` |
| Application | Mesma validação de e-mail em 3 Services | Classe `EmailValidator` ou Value Object `Email` |
| Application | Lógica de paginação repetida | Template ou classe `Paginator<T>` |
| Infrastructure | Mapeamento query → struct repetido | Método `hydrate()` no Repository base |
| Infrastructure | Abertura de conexão SQL repetida | Classe `DatabaseConnection` centralizada |
| Domain | Mesmas invariantes em múltiplas entidades | Classe base ou concept C++20 |

### 4.2 DRY em CMake

| # | Anti-padrão | Solução |
|---|-------------|---------|
| ☐ | Flags de compilação repetidas em cada `target_compile_options` | Função CMake `apply_common_flags(target)` |
| ☐ | Dependências Qt repetidas em múltiplos `target_link_libraries` | Interface library `Qt6CommonDeps` |
| ☐ | Mesmo bloco de teste repetido em múltiplos `CMakeLists.txt` | Macro `add_unit_test(name sources)` |

---

## 5. TDD — Red / Green / Refactor

### 5.1 O ciclo

```
🔴 RED      Escreva o teste antes. Ele falha — correto.
🟢 GREEN    Mínimo de código para passar. Sem otimização.
🔵 REFACTOR Melhore sem quebrar. Aqui entram SOLID e DRY.
```

### 5.2 Framework de testes para C++23/Qt6

| Framework | Uso | Instalação |
|-----------|-----|-----------|
| Google Test (GTest) | Testes unitários de C++ puro | `find_package(GTest)` |
| Qt Test (QTest) | Testes de classes Qt (signals, models) | `find_package(Qt6 COMPONENTS Test)` |
| Google Mock (GMock) | Mocks de interfaces C++ | Junto com GTest |

**Estrutura de diretórios:**

```
tests/
├── unit/
│   ├── services/
│   │   ├── UserServiceTest.cpp
│   │   └── OrderServiceTest.cpp
│   ├── domain/
│   │   ├── UserTest.cpp
│   │   └── EmailTest.cpp          ← Value Objects
│   └── CMakeLists.txt
├── integration/
│   ├── repositories/
│   │   └── MySqlUserRepositoryTest.cpp
│   └── CMakeLists.txt
└── ui/
    ├── MainWindowTest.cpp          ← QTest
    └── CMakeLists.txt
```

### 5.3 Checklists de TDD

#### Testes Unitários — Services (GTest + GMock)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Cada método público de Service tem teste unitário | 🔴 CRÍTICO |
| ☐ | Testes escritos ANTES da implementação (Red primeiro) | 🔴 CRÍTICO |
| ☐ | Dependências (Repositories) substituídas por mocks GMock | 🔴 CRÍTICO |
| ☐ | Caminho feliz E casos de erro cobertos | 🔴 CRÍTICO |
| ☐ | Nomes descritivos: `UserService_Register_ThrowsWhenEmailExists` | 🟠 IMPORTANTE |
| ☐ | Um único `ASSERT_*` ou `EXPECT_*` por comportamento testado | 🟠 IMPORTANTE |
| ☐ | Testes independentes — sem estado compartilhado entre testes | 🔴 CRÍTICO |

#### Testes de Integração — Repositories

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Repositories testados contra banco SQLite in-memory ou MySQL de teste | 🔴 CRÍTICO |
| ☐ | Schema criado e destruído por fixture de teste | 🔴 CRÍTICO |
| ☐ | CRUD completo coberto por testes de integração | 🔴 CRÍTICO |
| ☐ | Transações testadas (rollback em falha) | 🟠 IMPORTANTE |

#### Testes de UI — Qt Widgets (QTest)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Signals emitidos corretamente verificados com `QSignalSpy` | 🔴 CRÍTICO |
| ☐ | `QAbstractItemModel` subclasses testadas com `ModelTest` | 🟠 IMPORTANTE |
| ☐ | Interações de UI simuladas com `QTest::mouseClick()`, `QTest::keyClick()` | 🟠 IMPORTANTE |
| ☐ | Testes de UI não dependem de banco real | 🔴 CRÍTICO |

### 5.4 Exemplo completo — ciclo TDD em C++23/Qt6

```cpp
// 🔴 RED — UserServiceTest.cpp (escreva ANTES de UserService)
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Mock do Repository
class MockUserRepository : public IUserRepository {
public:
    MOCK_METHOD(std::optional<User>, findByEmail,
                (const QString& email), (override));
    MOCK_METHOD(bool, save, (const User& user), (override));
};

TEST(UserService_Register, ThrowsWhenEmailAlreadyExists) {
    // Arrange
    auto mockRepo = std::make_shared<MockUserRepository>();
    EXPECT_CALL(*mockRepo, findByEmail("existing@test.com"))
        .WillOnce(testing::Return(User{"existing@test.com"}));

    UserService service(mockRepo);

    // Act + Assert
    EXPECT_THROW(
        service.registerUser("existing@test.com", "Password123!"),
        EmailAlreadyExistsException
    );
}

// 🟢 GREEN — UserService.cpp — mínimo para passar
void UserService::registerUser(const QString& email, const QString& password) {
    if (m_repo->findByEmail(email).has_value()) {
        throw EmailAlreadyExistsException{};
    }
    // ...
}

// 🔵 REFACTOR — extrair validação de e-mail para Email Value Object
// Extrair hash de senha para PasswordHasher
// Manter os testes verdes
```

### 5.5 CMakeLists.txt para testes

```cmake
find_package(GTest REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Test)

macro(add_unit_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name}
        PRIVATE
            GTest::gtest_main
            GTest::gmock
            AppLib          # biblioteca da aplicação (sem main())
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

add_unit_test(UserServiceTest services/UserServiceTest.cpp)
add_unit_test(UserTest         domain/UserTest.cpp)
```

### 5.6 Métricas de Cobertura

| # | Item | Meta | Prioridade |
|---|------|------|-----------|
| ☐ | Cobertura de linhas nos Services | ≥ 80% | 🔴 CRÍTICO |
| ☐ | Cobertura de branches nos Services | ≥ 70% | 🟠 IMPORTANTE |
| ☐ | Cobertura de linhas nas entidades de Domain | ≥ 90% | 🟠 IMPORTANTE |
| ☐ | Relatório gerado com gcov + lcov no CI | — | 🟠 IMPORTANTE |

```bash
# Gerar relatório de cobertura com lcov
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
cmake --build build
cd build && ctest
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

---

## 6. Estrutura de Diretórios Recomendada

```
projeto/
├── CMakeLists.txt
├── cmake/
│   ├── CompilerFlags.cmake      ← flags de segurança e warnings
│   └── TestHelpers.cmake        ← macros add_unit_test
│
├── src/
│   ├── presentation/            ← LAYER 1
│   │   ├── views/               ← QWidget subclasses
│   │   ├── viewmodels/          ← ViewModels/Presenters
│   │   └── delegates/           ← QAbstractItemDelegate
│   │
│   ├── application/             ← LAYER 2
│   │   ├── services/            ← Use cases
│   │   └── dto/                 ← Data Transfer Objects
│   │
│   ├── domain/                  ← LAYER 3
│   │   ├── entities/            ← User, Order, Product
│   │   ├── interfaces/          ← IUserRepository, IMailer
│   │   └── valueobjects/        ← Email, Money, CPF
│   │
│   └── infrastructure/          ← LAYER 4
│       ├── repositories/        ← MySqlUserRepository
│       └── database/            ← DatabaseConnection
│
└── tests/
    ├── unit/
    ├── integration/
    └── ui/
```

---

## 7. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria completa de arquitetura em 4 camadas deste projeto C++23/Qt6/MySQL.
Reporte em português.

## LAYER 1 — PRESENTATION
- Há QSqlQuery dentro de QWidget ou slots de UI?
- Connections usam sintaxe moderna (ponteiro de membro)?
- Há conexões sem contexto (risco de dangling reference)?
- QObjects criados com new têm parent ou são smart pointers?
- ViewModels separados das Views?

## LAYER 2 — APPLICATION
- Services têm responsabilidade única (SRP)?
- Services dependem de interfaces ou de implementações concretas (DIP)?
- Operações longas estão em QThread/QtConcurrent?
- Services têm referência a widgets ou UI?

## LAYER 3 — DOMAIN
- Interfaces puras definidas para todos os Repositories?
- Entidades de domínio livres de dependência Qt?
- Value Objects implementados para conceitos do domínio?

## LAYER 4 — INFRASTRUCTURE
- Toda query SQL está nos Repositories?
- QSqlQuery::prepare() usado em 100% das queries?
- Transações explícitas para operações compostas?

## TDD
- Existe cobertura de testes para Services (GTest + GMock)?
- QSignalSpy usado para testar signals?
- Repositories testados com banco in-memory?

## SOLID e DRY
- Identifique as 3 piores violações de SRP
- Identifique as 3 piores violações de DRY
- Há injeção de dependência ou instanciação interna (DIP)?

## DIAGNÓSTICO FINAL
- Mapa de dependências entre camadas
- 5 ações de maior impacto arquitetural
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 8. Roadmap de Refatoração

| Sprint | Camada | Objetivo | Critério |
|--------|--------|----------|----------|
| 🔴 1 | Infra | Mover SQL para Repositories | Zero QSqlQuery fora de Repositories |
| 🔴 2 | Application | Criar Services e interfaces | Services sem referência a QWidget |
| 🔴 3 | Presentation | Separar ViewModels | Slots delegam para Services |
| 🔴 4 | Domain | Interfaces puras e Value Objects | Services dependem de interfaces |
| 🟠 5 | Application | TDD nos Services | Cobertura ≥ 80% com GMock |
| 🟠 6 | Infra | TDD nos Repositories | Testes com SQLite in-memory |
| 🟠 7 | Presentation | TDD na UI | QSignalSpy em signals críticos |
| 🟢 8 | Todas | DRY e Value Objects | PHPStan equivalente (clang-tidy) zero warnings |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I3 — Segurança — Memory Safety, SQL Injection, Binário, LGPD

# 🔒 Auditoria de Segurança & Hardening
## Stack: C++23 · Qt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | C++23 / Qt6 / MySQL / Infraestrutura |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  C++23          Memory safety, UB, buffer overflows      │
├─────────────────────────────────────────────────────────┤
│  Qt6            Input sanitization, QSql, rede, plugins  │
├─────────────────────────────────────────────────────────┤
│  MySQL          Prepared statements, permissões, TLS     │
├─────────────────────────────────────────────────────────┤
│  BINÁRIO        Flags de compilação, ASLR, stack canary  │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. C++23 — Memory Safety

### 2.1 Gerenciamento de Memória

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem `new`/`delete` raw — usar `std::unique_ptr` e `std::shared_ptr` | 🔴 CRÍTICO | clang-tidy / cppcheck |
| ☐ | Sem `malloc`/`free` em código C++ | 🔴 CRÍTICO | clang-tidy |
| ☐ | Sem raw pointers como donos de memória | 🔴 CRÍTICO | clang-tidy |
| ☐ | Sem `reinterpret_cast` sem justificativa documentada | 🔴 CRÍTICO | clang-tidy |
| ☐ | Arrays com tamanho fixo usando `std::array` — não C arrays | 🟠 IMPORTANTE | clang-tidy |
| ☐ | Sem VLA (Variable Length Arrays) — undefined no C++ standard | 🔴 CRÍTICO | clang-tidy |
| ☐ | `std::span` usado para passar arrays sem cópia | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Sem pointer arithmetic desnecessária | 🔴 CRÍTICO | cppcheck |

### 2.2 Buffer Overflows e Undefined Behavior

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Acesso a containers via `.at()` em código crítico (lança exceção) | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sem `strcpy`, `sprintf`, `gets` — usar versões seguras ou std:: | 🔴 CRÍTICO | cppcheck / grep |
| ☐ | `std::string` em vez de buffers char[] para strings dinâmicas | 🔴 CRÍTICO | clang-tidy |
| ☐ | Iteradores não invalidados — verificar modificação de container durante iteração | 🔴 CRÍTICO | Revisão manual |
| ☐ | Integer overflow checado em operações aritméticas críticas | 🔴 CRÍTICO | UBSan |
| ☐ | Sem signed/unsigned mismatch em comparações | 🟠 IMPORTANTE | clang-tidy |
| ☐ | Sem use-after-free — verificado com AddressSanitizer | 🔴 CRÍTICO | ASan |
| ☐ | Sem double-free — verificado com AddressSanitizer | 🔴 CRÍTICO | ASan |

### 2.3 Input Validation em C++

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Todo input externo validado antes de usar (tamanho, tipo, range) | 🔴 CRÍTICO | Revisão manual |
| ☐ | Parsing de arquivos com validação de formato antes de processar | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sem deserialização de dados externos sem schema validation | 🔴 CRÍTICO | Revisão manual |
| ☐ | Argumentos de linha de comando validados e sanitizados | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Sem `system()` com input do usuário | 🔴 CRÍTICO | cppcheck / grep |
| ☐ | Sem `popen()` com strings não sanitizadas | 🔴 CRÍTICO | cppcheck |

---

## 3. Qt6 — Segurança Específica

### 3.1 Banco de Dados com QSql

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Toda query SQL via `QSqlQuery::prepare()` + `bindValue()` | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sem concatenação de QString em queries SQL | 🔴 CRÍTICO | grep / Revisão |
| ☐ | Credenciais de banco não hardcoded no código-fonte | 🔴 CRÍTICO | grep |
| ☐ | Credenciais lidas de arquivo de config seguro ou variável de ambiente | 🔴 CRÍTICO | Revisão manual |
| ☐ | Conexão MySQL com TLS/SSL habilitado | 🔴 CRÍTICO | Configuração |
| ☐ | Timeout de conexão configurado | 🟠 IMPORTANTE | Configuração |

**Exemplo correto — QSqlQuery com prepared statement:**

```cpp
// ✅ CORRETO — prepared statement
QSqlQuery query;
query.prepare("SELECT id, name FROM users WHERE email = :email AND active = :active");
query.bindValue(":email", userInput);
query.bindValue(":active", true);
query.exec();

// ❌ ERRADO — SQL Injection
QString sql = "SELECT * FROM users WHERE email = '" + userEmail + "'";
QSqlQuery query(sql); // NUNCA fazer isso
```

### 3.2 Rede e QNetworkAccessManager

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Conexões HTTPS verificam certificado SSL (sem `QSslConfiguration::setPeerVerifyMode(QSslSocket::VerifyNone)`) | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QSslSocket::SslHandshakeError` tratado adequadamente | 🔴 CRÍTICO | Revisão manual |
| ☐ | URLs de requisição validadas antes de usar | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Timeout configurado em todas as requisições de rede | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Dados sensíveis não logados em requisições/respostas HTTP | 🔴 CRÍTICO | Revisão manual |
| ☐ | Certificate pinning implementado para APIs críticas | 🟠 IMPORTANTE | Revisão manual |

### 3.3 Armazenamento Seguro de Dados

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Senhas armazenadas com bcrypt/argon2 — nunca MD5/SHA1 plain | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QSettings` não usado para armazenar dados sensíveis (armazena em plain text) | 🔴 CRÍTICO | Revisão manual |
| ☐ | Dados sensíveis em memória zerados após uso (`std::fill` ou `OPENSSL_cleanse`) | 🔴 CRÍTICO | Revisão manual |
| ☐ | Arquivos temporários com dados sensíveis removidos com segurança | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Criptografia via Qt Cryptographic Architecture (QCA) ou OpenSSL — nunca implementação própria | 🔴 CRÍTICO | Revisão manual |

### 3.4 Plugins e Recursos Dinâmicos

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Plugins carregados apenas de caminhos confiáveis | 🔴 CRÍTICO | Revisão manual |
| ☐ | Arquivos de recurso (`.qrc`) não incluem dados sensíveis | 🔴 CRÍTICO | Revisão manual |
| ☐ | QML sem execução de JavaScript arbitrário de fontes externas | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QWebEngineView` (se usado) com sandbox habilitado | 🔴 CRÍTICO | Revisão manual |

---

## 4. MySQL — Segurança

### 4.1 Acesso e Permissões

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Usuário de aplicação com permissões mínimas (SELECT, INSERT, UPDATE, DELETE apenas nas tabelas necessárias) | 🔴 CRÍTICO | MySQL audit |
| ☐ | Sem uso de usuário root pela aplicação | 🔴 CRÍTICO | Configuração |
| ☐ | Acesso ao banco apenas de hosts específicos — sem `%` no host | 🔴 CRÍTICO | MySQL audit |
| ☐ | Senha do banco com complexidade mínima definida | 🔴 CRÍTICO | Configuração |
| ☐ | Conexão MySQL com SSL/TLS | 🔴 CRÍTICO | Configuração Qt |
| ☐ | Porta 3306 não exposta publicamente | 🔴 CRÍTICO | Firewall |

**Configuração de conexão MySQL com TLS no Qt6:**

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
db.setHostName("db.server.com");
db.setDatabaseName("appdb");
db.setUserName("appuser");
db.setPassword(readPasswordFromSecureStore()); // nunca hardcoded

// Configurar TLS
db.setConnectOptions(
    "SSL_CA=/etc/ssl/mysql/ca-cert.pem;"
    "SSL_CERT=/etc/ssl/mysql/client-cert.pem;"
    "SSL_KEY=/etc/ssl/mysql/client-key.pem;"
    "MYSQL_OPT_SSL_MODE=SSL_MODE_REQUIRED"
);
```

### 4.2 Dados e Retenção

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Dados sensíveis criptografados em repouso | 🔴 CRÍTICO | Revisão manual |
| ☐ | Backups criptografados | 🔴 CRÍTICO | Processo |
| ☐ | Política de retenção definida e implementada | 🟠 IMPORTANTE | Processo |
| ☐ | Logs do MySQL sem dados pessoais | 🔴 CRÍTICO | Configuração |

---

## 5. Segurança do Binário Compilado

### 5.1 Flags de Compilação de Segurança

| # | Flag | Propósito | Prioridade |
|---|------|----------|-----------|
| ☐ | `-D_FORTIFY_SOURCE=3` | Detecção de buffer overflow em runtime | 🔴 CRÍTICO |
| ☐ | `-fstack-protector-strong` | Stack canary — detecta stack smashing | 🔴 CRÍTICO |
| ☐ | `-Wformat -Wformat-security` | Detecta format string vulnerabilities | 🔴 CRÍTICO |
| ☐ | `-fPIE -pie` | Position Independent Executable (ASLR) | 🔴 CRÍTICO |
| ☐ | `-Wl,-z,relro -Wl,-z,now` | RELRO — seções somente leitura após load | 🔴 CRÍTICO |
| ☐ | `-fcf-protection=full` | Control Flow Integrity (CFI) | 🟠 IMPORTANTE |
| ☐ | `-fno-omit-frame-pointer` | Melhora stack traces em crash reports | 🟢 COSMÉTICO |

**CMakeLists.txt com flags de segurança:**

```cmake
# Flags de segurança obrigatórias
target_compile_options(${PROJECT_NAME} PRIVATE
    -D_FORTIFY_SOURCE=3
    -fstack-protector-strong
    -Wformat
    -Wformat-security
    -Werror=format-security
    -fPIE
    -fcf-protection=full
)

target_link_options(${PROJECT_NAME} PRIVATE
    -pie
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
)
```

### 5.2 Sanitizers (Build de desenvolvimento/CI)

| # | Sanitizer | Detecta | Prioridade |
|---|-----------|--------|-----------|
| ☐ | AddressSanitizer (ASan) | Use-after-free, buffer overflow, double-free | 🔴 CRÍTICO |
| ☐ | UndefinedBehaviorSanitizer (UBSan) | Integer overflow, null deref, misaligned access | 🔴 CRÍTICO |
| ☐ | ThreadSanitizer (TSan) | Data races, deadlocks | 🔴 CRÍTICO |
| ☐ | MemorySanitizer (MSan) | Leitura de memória não inicializada | 🟠 IMPORTANTE |
| ☐ | LeakSanitizer (LSan) | Memory leaks | 🔴 CRÍTICO |

**CMakeLists.txt para build com sanitizers:**

```cmake
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

if(ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address,undefined,leak
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address,undefined,leak
    )
endif()
```

```bash
# Build com sanitizers no CI
cmake -B build -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/app  # detecta erros em runtime
```

---

## 6. Análise Estática de Segurança

### 6.1 Ferramentas

| Ferramenta | Foco | Configuração |
|-----------|------|-------------|
| clang-tidy | C++ moderno, bugs, segurança | `.clang-tidy` |
| cppcheck | Bugs, memory leaks, UB | `--enable=all` |
| Flawfinder | Funções C perigosas | `flawfinder src/` |
| Semgrep | Padrões customizáveis | `semgrep --config=auto` |
| CodeQL | Análise semântica profunda | GitHub Actions |

**Arquivo `.clang-tidy` com verificações de segurança:**

```yaml
Checks: >
  clang-analyzer-*,
  clang-analyzer-security.*,
  bugprone-*,
  bugprone-use-after-move,
  bugprone-dangling-handle,
  cert-*,
  cppcoreguidelines-*,
  cppcoreguidelines-avoid-c-arrays,
  cppcoreguidelines-no-malloc,
  hicpp-*,
  modernize-*,
  performance-*,
  readability-*

WarningsAsErrors: >
  clang-analyzer-security.*,
  bugprone-use-after-move,
  cppcoreguidelines-no-malloc

CheckOptions:
  - key: modernize-use-nullptr.NullMacros
    value: 'NULL'
```

### 6.2 GitHub Actions com análise estática

```yaml
  security-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: clang-tidy
        run: |
          cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
          run-clang-tidy -p build -checks='clang-analyzer-security.*,cert-*'

      - name: cppcheck
        run: |
          cppcheck --enable=all --error-exitcode=1 \
            --suppress=missingInclude src/

      - name: Flawfinder
        run: |
          pip install flawfinder
          flawfinder --error-level=2 src/

      - name: CodeQL
        uses: github/codeql-action/analyze@v3
        with:
          languages: cpp
```

---

## 7. Checklist LGPD para Aplicações Desktop Qt6

| # | Item de Verificação | Artigo LGPD | Prioridade |
|---|---------------------|------------|-----------|
| ☐ | Dados pessoais não logados em arquivos de log da aplicação | Art. 46 | 🔴 CRÍTICO |
| ☐ | Strings de conexão sem credenciais visíveis em crash dumps | Art. 46 | 🔴 CRÍTICO |
| ☐ | Dados pessoais em memória zerados após uso (`OPENSSL_cleanse`) | Art. 46 | 🔴 CRÍTICO |
| ☐ | Exportação de dados do titular implementada (portabilidade) | Art. 18, V | 🟠 IMPORTANTE |
| ☐ | Exclusão completa de dados do titular implementada | Art. 18, VI | 🔴 CRÍTICO |
| ☐ | Consentimento registrado com timestamp e versão da política | Art. 7, I | 🟠 IMPORTANTE |
| ☐ | Dados pessoais criptografados no banco MySQL | Art. 46 | 🔴 CRÍTICO |
| ☐ | Plano de resposta a incidentes documentado (72h para notificar ANPD) | Art. 48 | 🔴 CRÍTICO |

---

## 8. Roadmap de Segurança

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Eliminar SQL Injection | QSqlQuery::prepare() em todas as queries | grep sem concatenação SQL |
| 🔴 2 | Memory safety | Substituir raw pointers por smart pointers | ASan sem erros |
| 🔴 3 | Flags de compilação | Adicionar -fstack-protector, ASLR, RELRO | checksec confirma proteções |
| 🔴 4 | Remover funções perigosas | Eliminar strcpy/sprintf/gets | Flawfinder score zero |
| 🟠 5 | Sanitizers no CI | ASan + UBSan + TSan em build de teste | CI verde com sanitizers |
| 🟠 6 | TLS no MySQL | Conexão com SSL obrigatório | Wireshark confirma criptografia |
| 🟠 7 | Clang-tidy + CodeQL | Integrar análise estática no pipeline | Zero warnings de segurança |
| 🟢 8 | LGPD | Implementar exclusão e exportação de dados | Checklist LGPD completo |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I4 — C++23 Moderno — Tipos, Concorrência, Move Semantics

# ⚙️ Auditoria de C++23 Moderno
## Recursos, Memory Safety, Concorrência e Boas Práticas

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | C++23 / C++20 / C++17 |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  MODERNIDADE     C++17/20/23 vs código legado C-style    │
├─────────────────────────────────────────────────────────┤
│  MEMORY SAFETY   Smart pointers, RAII, ownership         │
├─────────────────────────────────────────────────────────┤
│  CONCORRÊNCIA    Threads, atomics, coroutines            │
├─────────────────────────────────────────────────────────┤
│  PERFORMANCE     Move semantics, ranges, views           │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Modernidade — C++17/20/23

### 2.1 Tipos e Expressões

| # | Anti-padrão legado | Substituição C++ moderno | Padrão | Prioridade |
|---|-------------------|--------------------------|--------|-----------|
| ☐ | `NULL` ou `0` como ponteiro nulo | `nullptr` | C++11 | 🔴 CRÍTICO |
| ☐ | `typedef struct { } Foo;` | `struct Foo { };` ou `using Foo = ...;` | C++11 | 🟠 IMPORTANTE |
| ☐ | `std::pair<int, std::string>` retornado de função | Structured bindings: `auto [id, name] = ...` | C++17 | 🟢 COSMÉTICO |
| ☐ | `if (ptr != nullptr)` | `if (ptr)` ou `if (auto p = tryGet())` | C++17 | 🟢 COSMÉTICO |
| ☐ | Ausência de `[[nodiscard]]` em retornos críticos | `[[nodiscard]] bool save(...)` | C++17 | 🟠 IMPORTANTE |
| ☐ | Ausência de `[[maybe_unused]]` | Atributo explícito em parâmetros não usados | C++17 | 🟢 COSMÉTICO |
| ☐ | `std::optional` não usado para retornos opcionais | `std::optional<User> findById(int id)` | C++17 | 🔴 CRÍTICO |
| ☐ | `std::variant` não usado onde aplicável | Substituir unions e polimorfismo simples | C++17 | 🟠 IMPORTANTE |
| ☐ | `std::expected` não usado para erros sem exceções | `std::expected<T, Error>` | C++23 | 🟠 IMPORTANTE |

### 2.2 Strings e String Views

| # | Anti-padrão | Substituição | Padrão | Prioridade |
|---|-------------|-------------|--------|-----------|
| ☐ | `const std::string&` em parâmetros de leitura | `std::string_view` | C++17 | 🟠 IMPORTANTE |
| ☐ | Concatenação com `+` em loop | `std::ostringstream` ou `std::format` | C++20 | 🟠 IMPORTANTE |
| ☐ | `sprintf` para formatação | `std::format("{} {}", a, b)` | C++20 | 🔴 CRÍTICO |
| ☐ | `strcmp`, `strlen` com `char*` | `std::string` e `std::string_view` | C++17 | 🔴 CRÍTICO |

### 2.3 Ranges e Algoritmos (C++20/23)

| # | Anti-padrão | Substituição | Padrão | Prioridade |
|---|-------------|-------------|--------|-----------|
| ☐ | Loop manual para filtrar/transformar coleções | `std::ranges::filter_view` / `std::views::transform` | C++20 | 🟠 IMPORTANTE |
| ☐ | `std::sort(v.begin(), v.end())` | `std::ranges::sort(v)` | C++20 | 🟢 COSMÉTICO |
| ☐ | Loop manual de cópia | `std::ranges::copy` | C++20 | 🟢 COSMÉTICO |
| ☐ | Acumulação manual com loop | `std::ranges::fold_left` | C++23 | 🟢 COSMÉTICO |

**Exemplo — ranges vs loop manual:**

```cpp
// ❌ Loop manual — verboso e propenso a erro
std::vector<User> activeUsers;
for (const auto& user : allUsers) {
    if (user.isActive()) {
        activeUsers.push_back(user);
    }
}

// ✅ Ranges C++20 — declarativo e seguro
auto activeUsers = allUsers
    | std::views::filter([](const User& u) { return u.isActive(); })
    | std::ranges::to<std::vector>(); // C++23
```

### 2.4 Enums

| # | Anti-padrão | Substituição | Prioridade |
|---|-------------|-------------|-----------|
| ☐ | `enum Status { ACTIVE, INACTIVE }` — polui namespace | `enum class Status { Active, Inactive }` | 🔴 CRÍTICO |
| ☐ | Comparação de enum com int diretamente | Usar `enum class` — não converte implicitamente | 🔴 CRÍTICO |
| ☐ | String status hardcoded `"active"` em vez de enum | `enum class Status` com conversão explícita | 🔴 CRÍTICO |

---

## 3. Memory Safety e RAII

### 3.1 Smart Pointers

| # | Item de Verificação | Regra | Prioridade |
|---|---------------------|-------|-----------|
| ☐ | `std::unique_ptr` para ownership exclusivo | Default para objetos heap-allocated | 🔴 CRÍTICO |
| ☐ | `std::shared_ptr` apenas quando ownership compartilhado é necessário | Não usar por padrão — tem custo | 🟠 IMPORTANTE |
| ☐ | `std::weak_ptr` para referências sem ownership (quebra ciclos) | Sempre que `shared_ptr` circular é possível | 🔴 CRÍTICO |
| ☐ | `std::make_unique` / `std::make_shared` — nunca `new` diretamente | Exception-safe | 🔴 CRÍTICO |
| ☐ | Raw pointers apenas como referências não-owning (observing pointers) | `T*` ou `T&` apenas para "não possuo este objeto" | 🟠 IMPORTANTE |
| ☐ | `std::shared_ptr<this>` via `std::enable_shared_from_this` quando necessário | Evita dangling pointer | 🔴 CRÍTICO |

**Regras de ownership — resumo:**

```cpp
// Dono exclusivo → unique_ptr
auto dialog = std::make_unique<MyDialog>(this);

// Dono compartilhado → shared_ptr (use com moderação)
auto config = std::make_shared<AppConfig>();

// Referência sem ownership → raw pointer ou referência
void processUser(const User* user);   // não possui
void processUser(const User& user);   // preferível

// Referência fraca → weak_ptr (quebra ciclo shared_ptr)
std::weak_ptr<Observer> m_observer;

// ❌ NUNCA
User* user = new User();   // quem deleta?
delete user;               // e se lançar exceção antes disso?
```

### 3.2 RAII

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Recursos (arquivo, socket, mutex) adquiridos em construtor e liberados em destrutor | 🔴 CRÍTICO |
| ☐ | `std::lock_guard` ou `std::unique_lock` — nunca lock/unlock manual | 🔴 CRÍTICO |
| ☐ | Arquivos via `std::ifstream`/`std::ofstream` — nunca `fopen`/`fclose` manual | 🔴 CRÍTICO |
| ☐ | Transações de banco encapsuladas em RAII guard | 🟠 IMPORTANTE |
| ☐ | `std::jthread` (C++20) em vez de `std::thread` raw — join automático | 🟠 IMPORTANTE |

**Exemplo RAII para transação MySQL:**

```cpp
class SqlTransaction {
public:
    explicit SqlTransaction(QSqlDatabase& db) : m_db(db) {
        m_db.transaction();
    }
    ~SqlTransaction() {
        if (!m_committed) {
            m_db.rollback(); // rollback automático se não committed
        }
    }
    void commit() {
        m_db.commit();
        m_committed = true;
    }
private:
    QSqlDatabase& m_db;
    bool m_committed = false;
};

// Uso
void UserRepository::transferPoints(int fromId, int toId, int points) {
    SqlTransaction tx(m_db);  // RAII — rollback automático em exceção
    deductPoints(fromId, points);
    addPoints(toId, points);
    tx.commit();
}
```

### 3.3 Lifetime e Dangling References

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem retorno de referência a variável local | 🔴 CRÍTICO | clang-tidy / compilador |
| ☐ | `std::string_view` não armazena view de temporário | 🔴 CRÍTICO | clang-tidy |
| ☐ | Iteradores não usados após invalidação do container | 🔴 CRÍTICO | ASan |
| ☐ | Lambdas que capturam por referência não sobrevivem ao escopo | 🔴 CRÍTICO | clang-tidy |
| ☐ | `QPointer<T>` usado para ponteiros Qt que podem ser deletados externamente | 🔴 CRÍTICO | Revisão manual |

---

## 4. Concorrência

### 4.1 Qt6 e Threads

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | UI modificada apenas na thread principal | 🔴 CRÍTICO |
| ☐ | Operações longas em `QThread` ou `QtConcurrent::run()` | 🔴 CRÍTICO |
| ☐ | Cross-thread signals com `Qt::QueuedConnection` | 🔴 CRÍTICO |
| ☐ | Sem subclassificação de `QThread` — usar moveToThread pattern | 🟠 IMPORTANTE |
| ☐ | `QMutex` protegendo dados compartilhados entre threads | 🔴 CRÍTICO |
| ☐ | `QReadWriteLock` para dados com leitura frequente e escrita rara | 🟠 IMPORTANTE |

**Pattern correto — Worker com moveToThread:**

```cpp
// ✅ CORRETO — Worker movido para thread, não subclasse de QThread
class DataWorker : public QObject {
    Q_OBJECT
public slots:
    void processData() {
        // roda na thread secundária
        auto result = heavyComputation();
        emit resultReady(result);
    }
signals:
    void resultReady(const QVariant& result);
};

// Setup na MainWindow
auto thread = new QThread(this);
auto worker = new DataWorker;
worker->moveToThread(thread);
connect(thread, &QThread::finished, worker, &QObject::deleteLater);
connect(this, &MainWindow::startWork, worker, &DataWorker::processData);
connect(worker, &DataWorker::resultReady, this, &MainWindow::onResult);
thread->start();

// ❌ ERRADO — subclassificar QThread
class MyThread : public QThread {
    void run() override { /* lógica misturada */ }
};
```

### 4.2 std::thread e Primitivas C++

| # | Item de Verificação | Padrão | Prioridade |
|---|---------------------|--------|-----------|
| ☐ | `std::jthread` com `stop_token` em vez de `std::thread` raw | C++20 | 🟠 IMPORTANTE |
| ☐ | `std::atomic<T>` para variáveis compartilhadas simples | C++11 | 🔴 CRÍTICO |
| ☐ | `std::mutex` protegendo seções críticas | C++11 | 🔴 CRÍTICO |
| ☐ | `std::lock_guard` / `std::scoped_lock` — nunca mutex.lock() manual | C++17 | 🔴 CRÍTICO |
| ☐ | `std::condition_variable` para sincronização de estado | C++11 | 🟠 IMPORTANTE |
| ☐ | `std::future` / `std::async` para tarefas assíncronas simples | C++11 | 🟠 IMPORTANTE |
| ☐ | Sem data races — verificado com ThreadSanitizer | C++11 | 🔴 CRÍTICO |

### 4.3 Coroutines C++20 (se aplicável)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `co_await` / `co_yield` usados para operações assíncronas I/O bound | 🟠 IMPORTANTE |
| ☐ | Coroutines com lifetime explícito e claro | 🟠 IMPORTANTE |
| ☐ | Sem coroutines onde `std::future` seria mais simples | 🟢 COSMÉTICO |

---

## 5. Performance C++ Moderno

### 5.1 Move Semantics

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Move constructor e move assignment operator definidos em classes com recursos | 🔴 CRÍTICO |
| ☐ | `std::move()` usado ao passar objetos grandes para funções que tomam por valor | 🟠 IMPORTANTE |
| ☐ | Return Value Optimization (RVO) não inibida por `return std::move(local)` desnecessário | 🟠 IMPORTANTE |
| ☐ | Objetos retornados por valor (RVO) — não por ponteiro ou referência desnecessariamente | 🟠 IMPORTANTE |
| ☐ | Regra dos 5/0: definir destrutor, copy/move constructor/assignment ou nenhum deles | 🔴 CRÍTICO |

**Regra dos 5 e Regra dos 0:**

```cpp
// ✅ Regra dos 0 — deixar compilador gerar tudo (preferível)
class User {
    std::string m_name;
    int m_id;
    // compilador gera destrutor, copy e move corretos automaticamente
};

// ✅ Regra dos 5 — quando gerencia recurso manual (raro com smart pointers)
class FileHandle {
public:
    FileHandle(const char* path);
    ~FileHandle();                            // destrutor
    FileHandle(const FileHandle&);            // copy constructor
    FileHandle& operator=(const FileHandle&); // copy assignment
    FileHandle(FileHandle&&) noexcept;        // move constructor
    FileHandle& operator=(FileHandle&&) noexcept; // move assignment
private:
    FILE* m_file;
};

// ❌ Regra dos 3 incompleta — perigoso
class BadClass {
public:
    ~BadClass() { delete m_ptr; }
    // sem copy/move → compilador gera shallow copy → double free!
private:
    int* m_ptr;
};
```

### 5.2 Alocações e Containers

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `reserve()` em `std::vector` quando tamanho é conhecido antecipadamente | 🟠 IMPORTANTE |
| ☐ | `emplace_back()` em vez de `push_back(T{...})` — evita cópia temporária | 🟠 IMPORTANTE |
| ☐ | Escolha correta de container: `vector` > `list` > `deque` para acesso sequencial | 🟠 IMPORTANTE |
| ☐ | `std::unordered_map` para lookup O(1) em vez de `std::map` O(log n) | 🟠 IMPORTANTE |
| ☐ | `std::string_view` para leitura de string sem alocação | 🟠 IMPORTANTE |
| ☐ | Sem cópias desnecessárias em range-based for: `for (const auto& item : vec)` | 🔴 CRÍTICO |

---

## 6. Warnings e Diagnóstico do Compilador

### 6.1 Flags de Warning obrigatórias

```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    # Warnings base — obrigatórios
    -Wall
    -Wextra
    -Wpedantic
    -Werror                    # warnings são erros em CI

    # Warnings específicos de segurança
    -Wformat=2
    -Wformat-security
    -Wnull-dereference
    -Wstack-protector
    -Wshadow

    # C++ moderno
    -Wold-style-cast           # detecta (int)x em vez de static_cast<int>(x)
    -Woverloaded-virtual       # detecta override sem virtual na base
    -Wnon-virtual-dtor         # classe polimórfica sem virtual destrutor
    -Wdouble-promotion         # float promovido para double acidentalmente

    # Undefined Behavior
    -Wstrict-aliasing=2
    -Wundef
)
```

### 6.2 Checklist de Warnings

| # | Warning | Significado | Prioridade |
|---|---------|------------|-----------|
| ☐ | `-Wnon-virtual-dtor` sem erro | Classe base polimórfica sem `virtual ~` | 🔴 CRÍTICO |
| ☐ | `-Woverloaded-virtual` sem erro | Função virtual obscurecida na subclasse | 🔴 CRÍTICO |
| ☐ | `-Wshadow` sem erro | Variável local obscurece variável de escopo maior | 🟠 IMPORTANTE |
| ☐ | `-Wold-style-cast` sem erro | C-style cast (`(int)x`) em vez de `static_cast` | 🟠 IMPORTANTE |
| ☐ | `-Wnull-dereference` sem erro | Possível desreferência de nullptr | 🔴 CRÍTICO |
| ☐ | Sem `#pragma warning(disable:...)` sem justificativa | Supressão silenciosa de warning | 🟠 IMPORTANTE |

---

## 7. Conceitos C++20 (Concepts)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Templates com constraints usando `requires` em vez de SFINAE | 🟠 IMPORTANTE |
| ☐ | `std::concept` definido para interfaces de template | 🟢 COSMÉTICO |
| ☐ | `auto` em parâmetros de função apenas onde tipo é realmente irrelevante | 🟢 COSMÉTICO |

**Exemplo — Concepts vs SFINAE:**

```cpp
// ❌ SFINAE — ilegível
template<typename T,
         typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T square(T x) { return x * x; }

// ✅ C++20 Concepts — legível e descritivo
template<std::integral T>
T square(T x) { return x * x; }

// ✅ Ou com requires
template<typename T>
    requires std::is_arithmetic_v<T>
T square(T x) { return x * x; }
```

---

## 8. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria de qualidade de código C++23 moderno neste projeto.
Reporte em português.

## MODERNIDADE C++
- Há uso de NULL em vez de nullptr?
- Há typedef em vez de using?
- std::optional é usado para retornos opcionais?
- std::format é usado em vez de sprintf?
- enum class em vez de enum simples?
- Ranges/views C++20 onde aplicável?

## MEMORY SAFETY
- Há raw new/delete sem smart pointer?
- Há raw pointers com ownership implícito?
- make_unique/make_shared usados em vez de new direto?
- Há risco de use-after-free ou double-free?
- RAII aplicado para todos os recursos?
- Regra dos 5/0 seguida?

## CONCORRÊNCIA
- UI modificada em threads não-principais?
- QThread usado corretamente (moveToThread, não subclasse)?
- Dados compartilhados protegidos por mutex ou atomic?
- ThreadSanitizer rodou sem erros?

## PERFORMANCE
- Há cópias desnecessárias em range-based for?
- Move semantics implementado em classes com recursos?
- string_view usado em vez de const string& onde possível?
- Containers com reserve() quando tamanho é conhecido?

## WARNINGS
- Build compila sem warnings com -Wall -Wextra -Werror?
- Há -Wnon-virtual-dtor em classes polimórficas?

## DIAGNÓSTICO FINAL
- Identifique as 5 piores violações de C++ moderno
- Identifique os 3 maiores riscos de memory safety
- Priorize: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 9. Roadmap de Modernização C++

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Eliminar new/delete raw | Substituir por unique_ptr/make_unique | ASan sem leaks |
| 🔴 2 | nullptr em todo lugar | Substituir NULL e 0 como ponteiro | Grep sem NULL |
| 🔴 3 | Thread safety | Proteger dados compartilhados com mutex/atomic | TSan sem erros |
| 🟠 4 | std::optional | Retornos opcionais sem ponteiro nulo | clang-tidy validando |
| 🟠 5 | enum class | Substituir enums simples | Grep sem enum sem class |
| 🟠 6 | std::format | Substituir sprintf/ostringstream | Build com C++20 ativo |
| 🟠 7 | Regra dos 5/0 | Auditar classes com destrutor | -Wnon-virtual-dtor zero |
| 🟢 8 | Ranges C++20 | Refatorar loops para ranges/views | Build limpo C++23 |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I5 — Qt6 Específico — Signals/Slots, Model/View, QML, i18n

# 🖥️ Auditoria Qt6 Específica
## UI/UX · QML · Models/Views · Signals/Slots · Internacionalização

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Qt6 / C++23 |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  WIDGETS        Layout, responsividade, acessibilidade   │
├─────────────────────────────────────────────────────────┤
│  MODEL/VIEW     QAbstractItemModel, delegates, proxies   │
├─────────────────────────────────────────────────────────┤
│  QML            Bindings, performance, integração C++    │
├─────────────────────────────────────────────────────────┤
│  RECURSOS       i18n, temas, assets, memória de UI       │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Qt6 Widgets — Layout e Responsividade

### 2.1 Layouts

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Sem `setFixedSize()` hardcoded — usar `QSizePolicy` | 🔴 CRÍTICO |
| ☐ | Sem `move()` ou `resize()` absoluto em pixels | 🔴 CRÍTICO |
| ☐ | Todos os widgets dentro de layouts (`QVBoxLayout`, `QHBoxLayout`, `QGridLayout`) | 🔴 CRÍTICO |
| ☐ | `QSplitter` para divisões redimensionáveis | 🟠 IMPORTANTE |
| ☐ | `QScrollArea` para conteúdo que pode não caber na tela | 🟠 IMPORTANTE |
| ☐ | Janela principal com tamanho mínimo definido (`setMinimumSize`) | 🟠 IMPORTANTE |
| ☐ | Sem coordenadas absolutas para posicionar widgets | 🔴 CRÍTICO |
| ☐ | Layout testado com janela redimensionada (mínimo, máximo, intermediário) | 🟠 IMPORTANTE |

### 2.2 HiDPI e Escalabilidade

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `Qt::AA_EnableHighDpiScaling` ou equivalente Qt6 habilitado | 🔴 CRÍTICO |
| ☐ | Ícones em formato SVG ou em múltiplas resoluções (`@2x`, `@3x`) | 🔴 CRÍTICO |
| ☐ | Tamanhos de fonte em pontos (pt) — não em pixels (px) | 🔴 CRÍTICO |
| ☐ | Imagens carregadas com `devicePixelRatio()` considerado | 🟠 IMPORTANTE |
| ☐ | Testado em monitor HiDPI (200% de escala) | 🟠 IMPORTANTE |

### 2.3 QSizePolicy

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | Widget que deveria expandir com `Fixed` | `QSizePolicy::Expanding` | 🔴 CRÍTICO |
| ☐ | Widget que deveria ser fixo com `Expanding` | `QSizePolicy::Fixed` ou `Preferred` | 🟠 IMPORTANTE |
| ☐ | `setFixedWidth` em widget de texto (quebra com fontes grandes) | `setMinimumWidth` + `Expanding` | 🟠 IMPORTANTE |

---

## 3. Qt6 Model/View Architecture

### 3.1 QAbstractItemModel

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Dados em tabelas/listas representados por `QAbstractItemModel` — não por widgets diretos | 🔴 CRÍTICO |
| ☐ | `QAbstractTableModel` ou `QAbstractListModel` subclassificados — não `QAbstractItemModel` direto | 🟠 IMPORTANTE |
| ☐ | `data()`, `rowCount()`, `columnCount()` implementados corretamente | 🔴 CRÍTICO |
| ☐ | `headerData()` implementado para tabelas com cabeçalho | 🟠 IMPORTANTE |
| ☐ | Emissão correta de `beginInsertRows`/`endInsertRows` ao inserir dados | 🔴 CRÍTICO |
| ☐ | Emissão correta de `beginRemoveRows`/`endRemoveRows` ao remover | 🔴 CRÍTICO |
| ☐ | `dataChanged()` emitido após atualização de dados — não `reset()` inteiro | 🔴 CRÍTICO |
| ☐ | Sem `reset()` desnecessário que perde seleção e scroll position | 🟠 IMPORTANTE |

**Exemplo correto de QAbstractTableModel:**

```cpp
class UserTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit UserTableModel(QObject* parent = nullptr)
        : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex& = {}) const override {
        return static_cast<int>(m_users.size());
    }

    int columnCount(const QModelIndex& = {}) const override {
        return 3; // ID, Nome, Email
    }

    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || role != Qt::DisplayRole)
            return {};

        const auto& user = m_users.at(index.row());
        switch (index.column()) {
            case 0: return user.id();
            case 1: return user.name();
            case 2: return user.email();
            default: return {};
        }
    }

    void addUser(const User& user) {
        const int row = rowCount();
        beginInsertRows({}, row, row);   // ✅ OBRIGATÓRIO antes de inserir
        m_users.push_back(user);
        endInsertRows();                 // ✅ OBRIGATÓRIO após inserir
    }

private:
    std::vector<User> m_users;
};
```

### 3.2 QSortFilterProxyModel

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Filtragem e ordenação via `QSortFilterProxyModel` — não recarregando dados do banco | 🔴 CRÍTICO |
| ☐ | `filterRegularExpression()` em vez de `filterRegExp()` (depreciado Qt6) | 🔴 CRÍTICO |
| ☐ | `setSortCaseSensitivity(Qt::CaseInsensitive)` configurado | 🟠 IMPORTANTE |
| ☐ | Proxy chain: source model → filter proxy → sort proxy → view | 🟠 IMPORTANTE |

### 3.3 Delegates

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Renderização customizada via `QStyledItemDelegate` — não embeddando widgets em células | 🟠 IMPORTANTE |
| ☐ | `paint()` eficiente — sem alocações por chamada | 🟠 IMPORTANTE |
| ☐ | Edição in-cell via `createEditor()` / `setEditorData()` / `setModelData()` | 🟠 IMPORTANTE |

---

## 4. QML (se aplicável)

### 4.1 Bindings e Performance

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Sem binding loops — propriedades com dependências circulares | 🔴 CRÍTICO |
| ☐ | `Connections` com `target` explícito — não binding implícito | 🟠 IMPORTANTE |
| ☐ | `ListView`/`GridView` com `delegate` leve — sem componentes pesados por item | 🔴 CRÍTICO |
| ☐ | `clip: true` apenas onde necessário (custo de performance) | 🟠 IMPORTANTE |
| ☐ | Animações usando `NumberAnimation` com `easing` correto | 🟢 COSMÉTICO |
| ☐ | Sem JavaScript pesado em `onCompleted` do item raiz | 🟠 IMPORTANTE |
| ☐ | Imagens com `sourceSize` explícito para evitar redimensionamento | 🟠 IMPORTANTE |
| ☐ | `Loader` para carregar componentes pesados sob demanda | 🟠 IMPORTANTE |

### 4.2 Integração C++/QML

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Propriedades expostas ao QML via `Q_PROPERTY` com `NOTIFY` signal | 🔴 CRÍTICO |
| ☐ | `qmlRegisterType` ou `QML_ELEMENT` (Qt6) para tipos C++ no QML | 🔴 CRÍTICO |
| ☐ | Lógica de negócio em C++ — não em JavaScript do QML | 🔴 CRÍTICO |
| ☐ | Sem `setContextProperty` em excesso (prefira tipos registrados) | 🟠 IMPORTANTE |
| ☐ | Models C++ (`QAbstractItemModel`) usados diretamente no QML | 🟠 IMPORTANTE |

**Exemplo correto de Q_PROPERTY:**

```cpp
class UserViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    QString name() const { return m_name; }
    void setName(const QString& name) {
        if (m_name == name) return;  // evita signal desnecessário
        m_name = name;
        emit nameChanged();
    }

    bool isLoading() const { return m_isLoading; }

signals:
    void nameChanged();
    void isLoadingChanged();

    Q_INVOKABLE void loadUser(int id);  // chamável do QML

private:
    QString m_name;
    bool m_isLoading = false;
};
```

### 4.3 QML — Organização

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Componentes QML em arquivos separados — não tudo em `main.qml` | 🔴 CRÍTICO |
| ☐ | Estados (`states`) para variações de UI — não condicionais em JavaScript | 🟠 IMPORTANTE |
| ☐ | `Transition` para animações entre estados | 🟢 COSMÉTICO |
| ☐ | Sem IDs desnecessários (`id:`) — usar bindings diretos | 🟢 COSMÉTICO |

---

## 5. Signals e Slots — Checklist Avançado

### 5.1 Problemas de Conexão

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Conexões duplicadas — `connect()` chamado múltiplas vezes para o mesmo par | 🔴 CRÍTICO |
| ☐ | Desconexão explícita quando objetos são destruídos antes do sender | 🟠 IMPORTANTE |
| ☐ | `QMetaObject::Connection` armazenado para desconexão posterior quando necessário | 🟠 IMPORTANTE |
| ☐ | Sem signals emitidos em construtores (receivers podem não estar conectados) | 🟠 IMPORTANTE |

### 5.2 Thread Safety de Signals

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Signals emitidos de threads secundárias conectados com `Qt::QueuedConnection` | 🔴 CRÍTICO |
| ☐ | Sem acesso a widgets de thread secundária | 🔴 CRÍTICO |
| ☐ | `Qt::AutoConnection` (padrão) funciona corretamente apenas quando sender e receiver na mesma thread | 🟠 IMPORTANTE |
| ☐ | `QMetaObject::invokeMethod(obj, "slot", Qt::QueuedConnection)` para thread-safe UI update | 🟠 IMPORTANTE |

### 5.3 Lifetime e Dangling

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Lambda em `connect()` com captura `[this]` tem contexto passado como 3º argumento | 🔴 CRÍTICO |
| ☐ | `QPointer<T>` usado em lambdas que capturam widgets | 🔴 CRÍTICO |
| ☐ | Receivers destruídos antes do sender têm conexão desfeita automaticamente (garante parent correto) | 🔴 CRÍTICO |

---

## 6. Internacionalização (i18n) Qt6

### 6.1 Strings traduzíveis

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Toda string visível ao usuário em `tr()` | 🔴 CRÍTICO |
| ☐ | Sem strings de UI hardcoded fora de `tr()` | 🔴 CRÍTICO |
| ☐ | Comentários de contexto em `tr()` para strings ambíguas: `tr("Open", "menu action")` | 🟠 IMPORTANTE |
| ☐ | `QCoreApplication::translate()` em classes sem `Q_OBJECT` | 🟠 IMPORTANTE |
| ☐ | `lupdate` executado e arquivos `.ts` atualizados | 🟠 IMPORTANTE |
| ☐ | `lrelease` gerando `.qm` no processo de build | 🟠 IMPORTANTE |

### 6.2 Localização Brasileira

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `QLocale` configurado: `QLocale(QLocale::Portuguese, QLocale::Brazil)` | 🔴 CRÍTICO |
| ☐ | Datas formatadas via `QLocale::toString(QDate, QLocale::ShortFormat)` | 🔴 CRÍTICO |
| ☐ | Moeda formatada em Real: `QLocale::toCurrencyString(valor)` | 🔴 CRÍTICO |
| ☐ | Separador decimal: vírgula (`,`) — não ponto | 🔴 CRÍTICO |
| ☐ | Separador de milhar: ponto (`.`) — `1.234,56` | 🔴 CRÍTICO |
| ☐ | Timezone configurado: `QTimeZone("America/Recife")` ou `"America/Sao_Paulo"` | 🔴 CRÍTICO |
| ☐ | Validação de CPF, CNPJ, CEP implementada | 🟠 IMPORTANTE |
| ☐ | Formatação de telefone BR: `(00) 00000-0000` | 🟠 IMPORTANTE |

**Configuração de locale no Qt6:**

```cpp
// main.cpp
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Locale brasileiro
    QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));

    // Timezone
    // Usar ao trabalhar com QDateTime
    QTimeZone tz("America/Sao_Paulo");

    // Tradução
    QTranslator translator;
    if (translator.load(QLocale(), "app", "_", ":/translations")) {
        app.installTranslator(&translator);
    }

    // Uso correto de locale
    QLocale locale;
    QString preco = locale.toCurrencyString(1234.56); // "R$ 1.234,56"
    QString data  = locale.toString(QDate::currentDate(), QLocale::ShortFormat); // "15/01/2024"

    return app.exec();
}
```

---

## 7. Temas e Estilização

### 7.1 QSS (Qt Style Sheets)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Estilo centralizado em arquivo `.qss` — não inline via `setStyleSheet()` espalhado | 🔴 CRÍTICO |
| ☐ | Seletores específicos — não `QWidget { }` que afeta tudo | 🟠 IMPORTANTE |
| ☐ | Suporte a modo escuro (dark mode) via paleta ou QSS alternativo | 🟢 COSMÉTICO |
| ☐ | Testado nos temas nativos dos SOs alvo (Windows, Linux, macOS) | 🟠 IMPORTANTE |
| ☐ | Sem hardcode de cores — usar variáveis ou paleta do sistema | 🟠 IMPORTANTE |

### 7.2 Ícones e Recursos

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Ícones via `QIcon::fromTheme()` para ícones de sistema | 🟠 IMPORTANTE |
| ☐ | Ícones customizados em SVG no `.qrc` | 🟠 IMPORTANTE |
| ☐ | Sem imagens PNG hardcoded sem fallback | 🟠 IMPORTANTE |
| ☐ | Recursos pesados (imagens grandes) carregados sob demanda | 🟢 COSMÉTICO |

---

## 8. Acessibilidade em Qt6

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Propriedade `accessibleName` definida em widgets sem texto visível | 🔴 CRÍTICO |
| ☐ | `accessibleDescription` para widgets complexos | 🟠 IMPORTANTE |
| ☐ | Tab order definido explicitamente para formulários | 🟠 IMPORTANTE |
| ☐ | Atalhos de teclado (`setShortcut`) em ações principais | 🟠 IMPORTANTE |
| ☐ | Testado com leitor de tela (NVDA/Orca) | 🟢 COSMÉTICO |
| ☐ | Contraste de cores adequado (≥ 4.5:1 para texto normal) | 🔴 CRÍTICO |

---

## 9. Performance de UI

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Sem operações bloqueantes na event loop (I/O, banco, rede) | 🔴 CRÍTICO |
| ☐ | `QApplication::processEvents()` não usado como gambiarra de thread | 🔴 CRÍTICO |
| ☐ | Atualizações de UI em batch — não uma por registro em lista grande | 🔴 CRÍTICO |
| ☐ | Pintura customizada (paintEvent) otimizada com cache | 🟠 IMPORTANTE |
| ☐ | `QTimer` para operações periódicas — não sleep em thread de UI | 🔴 CRÍTICO |
| ☐ | Virtual scrolling via Model/View — não criando widget por item | 🔴 CRÍTICO |

---

## 10. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria completa de código Qt6 deste projeto C++/Qt6.
Reporte em português.

## WIDGETS E LAYOUT
- Há setFixedSize() ou coordenadas absolutas?
- Todos os widgets estão dentro de layouts?
- QSizePolicy está configurado corretamente?
- Funciona em telas HiDPI (200% de escala)?

## MODEL/VIEW
- Listas e tabelas usam QAbstractItemModel?
- beginInsertRows/endInsertRows emitidos corretamente?
- dataChanged() usado em vez de reset() desnecessário?
- QSortFilterProxyModel para filtragem em vez de recarregar dados?

## SIGNALS E SLOTS
- Conexões usam sintaxe de ponteiro de membro?
- Há lambdas sem contexto (3º argumento)?
- Há conexões duplicadas?
- Cross-thread signals com QueuedConnection?

## QML (se aplicável)
- Há binding loops?
- Lógica de negócio está em C++ ou em JavaScript do QML?
- Q_PROPERTY com NOTIFY signal correto?
- Delegates de ListView são leves?

## i18n E LOCALE
- Todas as strings de UI em tr()?
- QLocale configurado para pt_BR?
- Datas e moedas formatadas com QLocale?
- Timezone configurado para Brazil?

## PERFORMANCE
- Há operações bloqueantes na event loop?
- QApplication::processEvents() usado como gambiarra?

## DIAGNÓSTICO FINAL
- 5 piores problemas Qt6 encontrados
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 11. Roadmap Qt6

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Layouts corretos | Remover setFixedSize, colocar tudo em layout | Funciona redimensionado |
| 🔴 2 | Model/View | Substituir listas diretas por QAbstractItemModel | Zero dados fora de Model |
| 🔴 3 | Signals modernos | Converter SIGNAL/SLOT macro para ponteiro de membro | Grep sem SIGNAL( |
| 🔴 4 | Thread safety | QueuedConnection cross-thread + mover heavy ops | Event loop nunca trava |
| 🟠 5 | i18n | tr() em todas as strings + QLocale pt_BR | lupdate sem strings novas |
| 🟠 6 | HiDPI | SVG icons + tamanhos em pt | Testado em 200% escala |
| 🟠 7 | QML otimização | Delegates leves, sem binding loops | Qt Quick Profiler limpo |
| 🟢 8 | Acessibilidade | accessibleName + tab order + atalhos | Orca/NVDA navegável |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I6 — MySQL — Schema, Queries, EXPLAIN, Migrations, LGPD

# 🗄️ Auditoria de Banco de Dados — MySQL
## Stack: C++23 · Qt6 · QSql

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | MySQL / QSql / Schema / Performance |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  SCHEMA         Modelagem, tipos, constraints, charset   │
├─────────────────────────────────────────────────────────┤
│  QUERIES        Prepared statements, N+1, paginação      │
├─────────────────────────────────────────────────────────┤
│  PERFORMANCE    Índices, explain, slow log, pool         │
├─────────────────────────────────────────────────────────┤
│  OPERAÇÕES      Migrations, backup, recuperação          │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Schema e Modelagem

### 2.1 Estrutura de Tabelas

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Toda tabela com chave primária definida | 🔴 CRÍTICO |
| ☐ | PK com `AUTO_INCREMENT` ou `UUID` — nunca lógica de negócio como PK | 🟠 IMPORTANTE |
| ☐ | Chaves estrangeiras com `ON DELETE` e `ON UPDATE` explícitos | 🟠 IMPORTANTE |
| ☐ | Constraints `NOT NULL` onde o campo é obrigatório | 🟠 IMPORTANTE |
| ☐ | Constraints `UNIQUE` onde o negócio exige unicidade | 🟠 IMPORTANTE |
| ☐ | Constraint `CHECK` para validações simples (MySQL 8.0.16+) | 🟢 COSMÉTICO |
| ☐ | Nomes de tabelas em inglês, snake_case, plural: `user_orders` | 🟠 IMPORTANTE |
| ☐ | Nomes de colunas em snake_case: `created_at`, `user_id` | 🟠 IMPORTANTE |

### 2.2 Tipos de Dados

| # | Anti-padrão | Tipo correto | Prioridade |
|---|-------------|-------------|-----------|
| ☐ | `VARCHAR` para datas | `DATE`, `DATETIME`, `TIMESTAMP` | 🔴 CRÍTICO |
| ☐ | `INT` ou `TINYINT` para booleano | `TINYINT(1)` ou `BOOLEAN` com constraint | 🟠 IMPORTANTE |
| ☐ | `FLOAT`/`DOUBLE` para dinheiro (perda de precisão) | `DECIMAL(15,2)` | 🔴 CRÍTICO |
| ☐ | `TEXT` onde `VARCHAR(n)` é suficiente | `VARCHAR(255)` ou tamanho adequado | 🟠 IMPORTANTE |
| ☐ | `CHAR` para campo de tamanho variável | `VARCHAR` | 🟠 IMPORTANTE |
| ☐ | `VARCHAR` para UUID | `CHAR(36)` ou `BINARY(16)` | 🟢 COSMÉTICO |
| ☐ | `LONGTEXT` para JSON pequeno | `JSON` (MySQL 5.7.8+) | 🟢 COSMÉTICO |

### 2.3 Charset e Collation

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Charset `utf8mb4` em todas as tabelas e colunas | 🔴 CRÍTICO |
| ☐ | Collation `utf8mb4_unicode_ci` ou `utf8mb4_0900_ai_ci` | 🔴 CRÍTICO |
| ☐ | Charset do servidor configurado para `utf8mb4` | 🔴 CRÍTICO |
| ☐ | Sem mistura de `utf8` (3 bytes) e `utf8mb4` (4 bytes) | 🔴 CRÍTICO |
| ☐ | Conexão Qt6 com charset correto: `SET NAMES utf8mb4` | 🔴 CRÍTICO |

**Configuração my.cnf para utf8mb4:**

```ini
[mysqld]
character-set-server = utf8mb4
collation-server     = utf8mb4_unicode_ci

[client]
default-character-set = utf8mb4
```

**Configuração de conexão Qt6 com charset:**

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
// ...
db.open();

// Forçar charset após conexão
QSqlQuery q(db);
q.exec("SET NAMES utf8mb4");
q.exec("SET CHARACTER SET utf8mb4");
```

---

## 3. Queries e QSql

### 3.1 Prepared Statements

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | 100% das queries com `QSqlQuery::prepare()` + `bindValue()` | 🔴 CRÍTICO |
| ☐ | Sem concatenação de `QString` em queries SQL | 🔴 CRÍTICO |
| ☐ | Nomes de bind com `:parametro` (padrão Qt) ou `?` — consistente | 🟠 IMPORTANTE |
| ☐ | Queries preparadas reutilizadas quando chamadas em loop | 🟠 IMPORTANTE |
| ☐ | `QSqlQuery::lastError()` verificado após `exec()` | 🔴 CRÍTICO |

**Padrão completo de query com tratamento de erro:**

```cpp
std::optional<User> MySqlUserRepository::findById(int id) {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, email, created_at FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "findById falhou:" << query.lastError().text();
        return std::nullopt;
    }

    if (!query.next()) {
        return std::nullopt;  // não encontrado
    }

    return mapRowToUser(query);
}

User MySqlUserRepository::mapRowToUser(const QSqlQuery& q) const {
    return User{
        q.value("id").toInt(),
        q.value("name").toString(),
        q.value("email").toString(),
        q.value("created_at").toDateTime()
    };
}
```

### 3.2 Transações

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Operações compostas (múltiplos INSERTs/UPDATEs relacionados) em transação | 🔴 CRÍTICO |
| ☐ | Rollback explícito em caso de falha | 🔴 CRÍTICO |
| ☐ | Transações encapsuladas em RAII guard | 🟠 IMPORTANTE |
| ☐ | Transações de menor duração possível | 🟠 IMPORTANTE |
| ☐ | Sem requisição de rede ou I/O dentro de transação aberta | 🔴 CRÍTICO |

```cpp
// RAII Transaction Guard
class SqlTransaction {
public:
    explicit SqlTransaction(QSqlDatabase& db) : m_db(db) {
        if (!m_db.transaction()) {
            throw std::runtime_error("Falha ao iniciar transação: " +
                m_db.lastError().text().toStdString());
        }
    }

    ~SqlTransaction() {
        if (!m_committed) {
            m_db.rollback();
            qWarning() << "Transação revertida (rollback)";
        }
    }

    void commit() {
        if (!m_db.commit()) {
            throw std::runtime_error("Falha no commit: " +
                m_db.lastError().text().toStdString());
        }
        m_committed = true;
    }

private:
    QSqlDatabase& m_db;
    bool m_committed = false;
};
```

### 3.3 N+1 Queries

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | Loop em C++ com query por iteração | JOIN ou IN() para busca em lote | 🔴 CRÍTICO |
| ☐ | `data()` do QAbstractItemModel executando query por célula | Carregar tudo em `fetchAll()` | 🔴 CRÍTICO |
| ☐ | Contar registros filhos com SELECT COUNT por item pai | JOIN com COUNT() | 🔴 CRÍTICO |

### 3.4 Paginação

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Toda listagem com `LIMIT` e `OFFSET` | 🔴 CRÍTICO |
| ☐ | Total de registros buscado com `COUNT(*)` separado | 🟠 IMPORTANTE |
| ☐ | `LIMIT` máximo definido (ex: 500 registros por página) | 🔴 CRÍTICO |
| ☐ | Sem `SELECT *` — colunas explícitas em todas as queries | 🟠 IMPORTANTE |

```cpp
// Paginação correta em Qt6/QSql
struct Page {
    int number;
    int size;
    int offset() const { return number * size; }
};

QVector<User> MySqlUserRepository::findPaginated(const Page& page) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT id, name, email, created_at
        FROM users
        WHERE deleted_at IS NULL
        ORDER BY created_at DESC
        LIMIT :limit OFFSET :offset
    )");
    query.bindValue(":limit", page.size);
    query.bindValue(":offset", page.offset());

    if (!query.exec()) {
        qCritical() << query.lastError().text();
        return {};
    }

    QVector<User> result;
    result.reserve(page.size);
    while (query.next()) {
        result.append(mapRowToUser(query));
    }
    return result;
}
```

---

## 4. Performance

### 4.1 Índices

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `EXPLAIN` em todas as queries com frequência > 100/dia | 🔴 CRÍTICO | MySQL Workbench |
| ☐ | Nenhum `type: ALL` (full table scan) em tabelas > 1000 linhas | 🔴 CRÍTICO | EXPLAIN |
| ☐ | Índices em todas colunas de `WHERE`, `JOIN`, `ORDER BY` | 🔴 CRÍTICO | EXPLAIN |
| ☐ | Índice composto na ordem correta (seletividade maior primeiro) | 🟠 IMPORTANTE | EXPLAIN |
| ☐ | Índices não utilizados identificados e removidos | 🟢 COSMÉTICO | `sys.schema_unused_indexes` |
| ☐ | Colunas de FK indexadas em ambas as tabelas do JOIN | 🔴 CRÍTICO | EXPLAIN |

**Análise de índice:**

```sql
-- Verificar índices existentes
SHOW INDEX FROM users;

-- Verificar índices não utilizados (MySQL 8+)
SELECT * FROM sys.schema_unused_indexes
WHERE object_schema = 'myapp';

-- EXPLAIN para análise de query
EXPLAIN SELECT u.id, u.name, COUNT(o.id) as orders
FROM users u
LEFT JOIN orders o ON o.user_id = u.id
WHERE u.active = 1
GROUP BY u.id;
-- Verificar: type=ref ou eq_ref, não ALL
```

### 4.2 Configuração MySQL para Performance

```ini
# my.cnf — otimizações para aplicação desktop/servidor
[mysqld]
# InnoDB — principal mecanismo de storage
innodb_buffer_pool_size = 1G          # 70-80% da RAM disponível
innodb_buffer_pool_instances = 2      # 1 por GB de buffer pool
innodb_log_file_size = 256M
innodb_flush_log_at_trx_commit = 2   # performance vs durabilidade

# Queries
query_cache_type = 0                  # depreciado no MySQL 8 — desabilitar
max_connections = 50                  # adequar à carga real

# Slow query
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 1
log_queries_not_using_indexes = 1
```

---

## 5. Migrations e Versionamento de Schema

### 5.1 Checklist de Migrations

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Schema versionado com migrations — nunca alteração manual direta | 🔴 CRÍTICO |
| ☐ | Migrations em controle de versão junto ao código | 🔴 CRÍTICO |
| ☐ | Cada migration tem `up()` (aplicar) e `down()` (reverter) | 🟠 IMPORTANTE |
| ☐ | Migrations testadas em ambiente de staging antes de produção | 🔴 CRÍTICO |
| ☐ | Sem migrations destrutivas sem backup | 🔴 CRÍTICO |
| ☐ | Migrations executadas automaticamente no startup da aplicação | 🟠 IMPORTANTE |

**Sistema simples de migrations em C++/Qt6:**

```cpp
class MigrationRunner {
public:
    explicit MigrationRunner(QSqlDatabase& db) : m_db(db) {
        ensureMigrationsTable();
    }

    void run() {
        const QVector<Migration> migrations = {
            {1, "create_users_table",   &MigrationRunner::m001_createUsers},
            {2, "add_email_index",      &MigrationRunner::m002_addEmailIndex},
            {3, "create_orders_table",  &MigrationRunner::m003_createOrders},
        };

        for (const auto& migration : migrations) {
            if (!isApplied(migration.version)) {
                qInfo() << "Aplicando migration" << migration.version
                        << migration.name;
                apply(migration);
            }
        }
    }

private:
    void ensureMigrationsTable() {
        QSqlQuery q(m_db);
        q.exec(R"(
            CREATE TABLE IF NOT EXISTS schema_migrations (
                version     INT PRIMARY KEY,
                name        VARCHAR(255) NOT NULL,
                applied_at  DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");
    }

    bool isApplied(int version) {
        QSqlQuery q(m_db);
        q.prepare("SELECT 1 FROM schema_migrations WHERE version = :v");
        q.bindValue(":v", version);
        q.exec();
        return q.next();
    }
};
```

---

## 6. Backup e Recuperação

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Backup automático diário configurado | 🔴 CRÍTICO |
| ☐ | Backup testado — restauração verificada mensalmente | 🔴 CRÍTICO |
| ☐ | Backups criptografados | 🔴 CRÍTICO |
| ☐ | Retenção de backups definida (ex: 30 dias) | 🟠 IMPORTANTE |
| ☐ | Binary log habilitado para point-in-time recovery | 🟠 IMPORTANTE |
| ☐ | RTO (Recovery Time Objective) definido e documentado | 🟠 IMPORTANTE |
| ☐ | RPO (Recovery Point Objective) definido e documentado | 🟠 IMPORTANTE |

**Script de backup com mysqldump:**

```bash
#!/bin/bash
# backup_mysql.sh
DB_HOST="localhost"
DB_USER="backup_user"
DB_PASS="${MYSQL_BACKUP_PASSWORD}"  # variável de ambiente
DB_NAME="myapp"
BACKUP_DIR="/var/backups/mysql"
DATE=$(date +%Y%m%d_%H%M%S)

mkdir -p "$BACKUP_DIR"

# Dump comprimido e criptografado
mysqldump \
    --host="$DB_HOST" \
    --user="$DB_USER" \
    --password="$DB_PASS" \
    --single-transaction \    # backup consistente sem lock
    --routines \
    --triggers \
    "$DB_NAME" \
    | gzip \
    | openssl enc -aes-256-cbc -salt -k "$ENCRYPTION_KEY" \
    > "$BACKUP_DIR/backup_${DATE}.sql.gz.enc"

# Remover backups com mais de 30 dias
find "$BACKUP_DIR" -name "*.enc" -mtime +30 -delete

echo "Backup concluído: backup_${DATE}.sql.gz.enc"
```

---

## 7. Auditoria e Compliance

| # | Item de Verificação | Artigo LGPD | Prioridade |
|---|---------------------|------------|-----------|
| ☐ | Binary log habilitado para trilha de auditoria | Art. 37 | 🟠 IMPORTANTE |
| ☐ | Dados pessoais criptografados em colunas sensíveis (CPF, e-mail) | Art. 46 | 🔴 CRÍTICO |
| ☐ | Soft delete implementado (`deleted_at`) para preservar histórico | Art. 16 | 🟠 IMPORTANTE |
| ☐ | Log de acesso a dados pessoais em tabela de auditoria | Art. 37 | 🟠 IMPORTANTE |
| ☐ | Exclusão física de dados após prazo de retenção legal | Art. 15/16 | 🔴 CRÍTICO |
| ☐ | Usuário do banco sem permissão de DROP ou TRUNCATE | Art. 46 | 🔴 CRÍTICO |

**Tabela de auditoria:**

```sql
CREATE TABLE audit_log (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    table_name  VARCHAR(64)  NOT NULL,
    record_id   INT UNSIGNED NOT NULL,
    action      ENUM('INSERT','UPDATE','DELETE') NOT NULL,
    user_id     INT UNSIGNED,
    ip_address  VARCHAR(45),
    changed_at  DATETIME DEFAULT CURRENT_TIMESTAMP,
    old_data    JSON,
    new_data    JSON,
    INDEX idx_table_record (table_name, record_id),
    INDEX idx_changed_at   (changed_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

---

## 8. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria completa do banco de dados MySQL deste projeto Qt6.
Reporte em português.

## SCHEMA
- Todas as tabelas têm PK definida?
- Há VARCHAR para datas ou FLOAT/DOUBLE para dinheiro?
- Charset é utf8mb4 em todas as tabelas?
- Constraints FK, NOT NULL, UNIQUE definidas adequadamente?

## QUERIES
- Há queries sem QSqlQuery::prepare() (SQL Injection)?
- Há concatenação de QString em queries SQL?
- QSqlQuery::lastError() verificado após exec()?
- Há N+1 queries (query por iteração de loop)?
- Listagens têm LIMIT/OFFSET?
- Há SELECT * em vez de colunas explícitas?

## PERFORMANCE
- Há índices nas colunas de WHERE, JOIN, ORDER BY?
- EXPLAIN foi executado nas queries mais frequentes?
- Há full table scan em tabelas grandes?

## MIGRATIONS
- Schema é versionado com migrations?
- Migrations estão em controle de versão?
- Há migrations destrutivas sem backup prévio?

## BACKUP E SEGURANÇA
- Backup automático configurado?
- Backups criptografados?
- Usuário do banco tem permissões mínimas?
- Dados sensíveis criptografados em repouso?

## DIAGNÓSTICO FINAL
- 5 piores problemas encontrados
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 9. Roadmap de Banco de Dados

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Segurança de queries | prepare()+bindValue() em 100% das queries | grep sem concatenação SQL |
| 🔴 2 | Charset universal | utf8mb4 em todas as tabelas e conexão Qt | Sem dados corrompidos com emoji |
| 🔴 3 | Tipos corretos | DECIMAL para dinheiro, DATETIME para datas | Zero FLOAT para dinheiro |
| 🟠 4 | Migrations | Versionar schema, implementar runner | Zero alteração manual de schema |
| 🟠 5 | Índices e EXPLAIN | Indexar colunas de WHERE/JOIN | Nenhum full table scan |
| 🟠 6 | Backup automático | Script diário + rotação + criptografia | Restauração testada |
| 🟢 7 | Auditoria | Tabela de audit_log para dados pessoais | Conformidade LGPD Art. 37 |
| 🟢 8 | Slow query | Habilitar slow log + analisar semanalmente | Slow log vazio em uso normal |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I7 — Performance — Event Loop, Alocações, Profiling

# ⚡ Auditoria de Performance & Observabilidade
## Stack: C++23 · Qt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | C++23 / Qt6 / MySQL / Infraestrutura |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  C++ / Qt6      Event loop, threads, alocações, cache    │
├─────────────────────────────────────────────────────────┤
│  MYSQL          Slow queries, índices, N+1, pool         │
├─────────────────────────────────────────────────────────┤
│  BUILD          Link time optimization, PGO              │
├─────────────────────────────────────────────────────────┤
│  OBSERVABILIDADE  Logging, profiling, crash reports      │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Performance Qt6 / C++

### 2.1 Event Loop

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Nenhuma operação > 16ms na thread principal (UI trava em 60fps) | 🔴 CRÍTICO | Qt Creator Profiler |
| ☐ | Sem `QApplication::processEvents()` como solução de thread | 🔴 CRÍTICO | Revisão manual |
| ☐ | I/O de arquivo em thread secundária | 🔴 CRÍTICO | Revisão manual |
| ☐ | Chamadas de rede em `QNetworkAccessManager` (assíncrono por padrão) | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QTimer::singleShot(0, ...)` para deferir trabalho sem bloquear | 🟠 IMPORTANTE | Revisão manual |
| ☐ | `QtConcurrent::run()` para operações CPU-bound | 🟠 IMPORTANTE | Revisão manual |

### 2.2 Alocações de Memória

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem alocações em hot paths (loops críticos, `paintEvent`, `data()` do Model) | 🔴 CRÍTICO | Valgrind / Heaptrack |
| ☐ | `QString` passado por `const QString&` ou `QStringView` em leitura | 🔴 CRÍTICO | clang-tidy |
| ☐ | `QStringLiteral("...")` em vez de `QString("...")` para literais | 🟠 IMPORTANTE | clang-tidy |
| ☐ | `QLatin1StringView` para strings ASCII puras | 🟢 COSMÉTICO | clang-tidy |
| ☐ | `reserve()` em `QVector`/`std::vector` quando tamanho é previsível | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Objetos reutilizados em vez de recriados por iteração | 🟠 IMPORTANTE | Heaptrack |

**Strings Qt6 — hierarquia de performance:**

```cpp
// Do mais rápido ao mais lento:

// ✅ 1. QStringLiteral — construído em compile time
auto msg = QStringLiteral("Olá, usuário");

// ✅ 2. QLatin1StringView — zero alocação, ASCII only
auto label = QLatin1StringView("Status: ");

// ✅ 3. QStringView — view sem alocação
void process(QStringView name) { /* leitura apenas */ }

// ⚠️ 4. const QString& — sem cópia mas com indireção
void display(const QString& text);

// ❌ 5. QString por valor — cópia desnecessária
void display(QString text); // copia em cada chamada
```

### 2.3 Rendering e Pintura

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `paintEvent()` customizado usa `QPixmap` como cache (não repinta tudo sempre) | 🔴 CRÍTICO | Qt Creator Profiler |
| ☐ | `update()` chamado com `QRect` específico — não `update()` inteiro quando possível | 🟠 IMPORTANTE | Qt Creator Profiler |
| ☐ | Sem criação de `QFont` ou `QBrush` dentro de `paintEvent()` | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QOpenGLWidget` para rendering de alto desempenho se necessário | 🟢 COSMÉTICO | Revisão manual |

### 2.4 Model/View Performance

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `data()` do Model é O(1) — sem query ao banco por chamada | 🔴 CRÍTICO | Qt Creator Profiler |
| ☐ | Dados carregados em lote — não um por um | 🔴 CRÍTICO | Revisão manual |
| ☐ | `setUniformItemSizes(true)` em `QListView` quando itens têm tamanho uniforme | 🟠 IMPORTANTE | Revisão manual |
| ☐ | `QAbstractItemView::setUpdatesEnabled(false)` durante atualizações em lote | 🟠 IMPORTANTE | Revisão manual |

```cpp
// ✅ Atualização em lote eficiente
model->setUpdatesEnabled(false);
for (const auto& item : newItems) {
    model->addItem(item);
}
model->setUpdatesEnabled(true);
```

---

## 3. MySQL — Performance

### 3.1 Slow Query Log

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Slow query log habilitado | 🔴 CRÍTICO | my.cnf |
| ☐ | `long_query_time = 1` (1 segundo) | 🔴 CRÍTICO | my.cnf |
| ☐ | Slow queries analisadas com `pt-query-digest` | 🟠 IMPORTANTE | Percona Toolkit |
| ☐ | `log_queries_not_using_indexes = 1` | 🟠 IMPORTANTE | my.cnf |

```ini
# my.cnf
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 1
log_queries_not_using_indexes = 1
min_examined_row_limit = 100
```

### 3.2 Índices

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `EXPLAIN` executado nas queries mais frequentes | 🔴 CRÍTICO | MySQL Workbench |
| ☐ | Nenhum `type: ALL` em tabelas > 1000 linhas | 🔴 CRÍTICO | EXPLAIN |
| ☐ | Índices em colunas usadas em WHERE, JOIN, ORDER BY | 🔴 CRÍTICO | EXPLAIN |
| ☐ | Índices compostos na ordem correta (seletividade maior primeiro) | 🟠 IMPORTANTE | EXPLAIN |
| ☐ | Índices não utilizados identificados e removidos | 🟢 COSMÉTICO | `sys.schema_unused_indexes` |

### 3.3 N+1 em Qt6/QSql

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | Query dentro de loop sobre resultados de outra query | JOIN ou IN() para carregar em lote | 🔴 CRÍTICO |
| ☐ | `data()` do Model executando query por index | Carregar todos os dados em `fetchAll()` | 🔴 CRÍTICO |
| ☐ | Contar com `SELECT COUNT(*)` por iteração | Uma query de contagem por lote | 🔴 CRÍTICO |

```cpp
// ❌ N+1 — uma query por usuário
QSqlQuery usersQuery("SELECT id FROM users");
while (usersQuery.next()) {
    int userId = usersQuery.value(0).toInt();
    QSqlQuery ordersQuery;
    ordersQuery.prepare("SELECT * FROM orders WHERE user_id = ?");
    ordersQuery.addBindValue(userId);
    ordersQuery.exec(); // N queries!
}

// ✅ CORRETO — uma query com JOIN
QSqlQuery query;
query.prepare(R"(
    SELECT u.id, u.name, COUNT(o.id) as order_count
    FROM users u
    LEFT JOIN orders o ON o.user_id = u.id
    GROUP BY u.id, u.name
)");
query.exec();
```

### 3.4 Connection Pool

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Conexão MySQL reutilizada — não aberta e fechada por operação | 🔴 CRÍTICO |
| ☐ | `QSqlDatabase::addDatabase()` com nome único por thread | 🔴 CRÍTICO |
| ☐ | Conexão verificada antes de usar (`isOpen()` + reconexão se necessário) | 🟠 IMPORTANTE |
| ☐ | Pool de conexões implementado para aplicações multithread | 🟠 IMPORTANTE |

```cpp
// Connection pool simples com QSqlDatabase nomeado por thread
QSqlDatabase DatabasePool::getConnection() {
    const QString connName = QString("conn_%1")
        .arg(reinterpret_cast<quintptr>(QThread::currentThread()));

    if (QSqlDatabase::contains(connName)) {
        auto db = QSqlDatabase::database(connName);
        if (db.isOpen()) return db;
    }

    auto db = QSqlDatabase::addDatabase("QMYSQL", connName);
    db.setHostName(m_host);
    db.setDatabaseName(m_dbName);
    db.setUserName(m_user);
    db.setPassword(m_password);
    db.open();
    return db;
}
```

---

## 4. Build — Otimizações de Compilação

### 4.1 Flags de Otimização

| # | Flag | Efeito | Build type |
|---|------|--------|-----------|
| ☐ | `-O2` ou `-O3` | Otimizações gerais | Release | 
| ☐ | `-march=native` | Usa instruções do CPU atual | Release (cuidado com distribuição) |
| ☐ | `-flto` (Link Time Optimization) | Otimização entre unidades de compilação | Release |
| ☐ | `-fomit-frame-pointer` | Libera registrador (cuidado: dificulta profiling) | Release |
| ☐ | Unity builds habilitado no CMake | Reduz tempo de compilação | Dev/CI |

```cmake
# CMakeLists.txt — configuração de release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3
        -flto
        -fomit-frame-pointer
    )
    target_link_options(${PROJECT_NAME} PRIVATE -flto)
endif()

# Unity build — acelera compilação em desenvolvimento
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)
```

### 4.2 Profile-Guided Optimization (PGO)

```bash
# 1. Build com instrumentação
cmake -B build_pgo -DCMAKE_CXX_FLAGS="-fprofile-generate" -DCMAKE_BUILD_TYPE=Release
cmake --build build_pgo

# 2. Executar com dados reais para gerar perfil
./build_pgo/app  # usar com dataset representativo

# 3. Build final com perfil
cmake -B build_final -DCMAKE_CXX_FLAGS="-fprofile-use" -DCMAKE_BUILD_TYPE=Release
cmake --build build_final
```

---

## 5. Profiling

### 5.1 Qt Creator Profiler

```
Ferramentas para usar dentro do Qt Creator:
- QML Profiler: detecta jank em QML (frames perdidos)
- CPU Usage Analyzer: identifica funções que consomem mais CPU
- Memory Analyzer: detecta leaks e uso excessivo de heap
- Flame Graph: visualiza call stack durante execução
```

### 5.2 Ferramentas Externas

| Ferramenta | Uso | Comando |
|-----------|-----|---------|
| Valgrind/Callgrind | Profiling de CPU | `valgrind --tool=callgrind ./app` |
| Heaptrack | Profiling de heap | `heaptrack ./app` |
| Perf | Profiling de sistema (Linux) | `perf record -g ./app && perf report` |
| gprof | Profiling clássico | Build com `-pg` + `gprof ./app gmon.out` |

---

## 6. Observabilidade

### 6.1 Logging Estruturado com Qt6

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `qInstallMessageHandler()` com handler customizado em produção | 🔴 CRÍTICO |
| ☐ | Níveis corretos: `qDebug`, `qInfo`, `qWarning`, `qCritical`, `qFatal` | 🔴 CRÍTICO |
| ☐ | `QT_LOGGING_RULES` para filtrar por categoria em produção | 🟠 IMPORTANTE |
| ☐ | Logs direcionados para arquivo em produção — não apenas stdout | 🔴 CRÍTICO |
| ☐ | Dados pessoais NUNCA logados (LGPD Art. 46) | 🔴 CRÍTICO |
| ☐ | `QLoggingCategory` para categorias de log por módulo | 🟠 IMPORTANTE |

**Handler de logging customizado:**

```cpp
// Logger customizado para produção
void customMessageHandler(QtMsgType type,
                           const QMessageLogContext& context,
                           const QString& msg) {
    // Sanitizar dados pessoais antes de logar
    // NUNCA logar: senha, CPF, cartão, token

    QString level;
    switch (type) {
        case QtDebugMsg:    level = "DEBUG";    break;
        case QtInfoMsg:     level = "INFO";     break;
        case QtWarningMsg:  level = "WARNING";  break;
        case QtCriticalMsg: level = "ERROR";    break;
        case QtFatalMsg:    level = "FATAL";    break;
    }

    // Formato JSON para log estruturado
    const QString entry = QString(R"({"level":"%1","msg":"%2","file":"%3","line":%4})")
        .arg(level, msg, context.file, context.line);

    QFile logFile("app.log");
    logFile.open(QIODevice::Append | QIODevice::Text);
    QTextStream stream(&logFile);
    stream << entry << "\n";
}

// main.cpp
qInstallMessageHandler(customMessageHandler);
```

### 6.2 Crash Reporting

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Crash handler instalado em produção | 🔴 CRÍTICO | Breakpad / Sentry |
| ☐ | Símbolos de debug gerados separadamente (`.pdb` / `.dSYM` / dwarf) | 🔴 CRÍTICO | strip + debuglink |
| ☐ | Core dumps habilitados em produção (`ulimit -c unlimited`) | 🟠 IMPORTANTE | Sistema |
| ☐ | Stack trace legível após crash | 🔴 CRÍTICO | addr2line / eu-addr2line |
| ☐ | Crash reports enviados automaticamente com contexto | 🟠 IMPORTANTE | Sentry / Crashpad |

```cmake
# Separar símbolos de debug do binário de produção
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
            $<TARGET_FILE:${PROJECT_NAME}>
            $<TARGET_FILE:${PROJECT_NAME}>.debug
    COMMAND ${CMAKE_OBJCOPY} --strip-debug
            $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_OBJCOPY}
            --add-gnu-debuglink=$<TARGET_FILE:${PROJECT_NAME}>.debug
            $<TARGET_FILE:${PROJECT_NAME}>
)
```

### 6.3 Health e Métricas

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Uso de memória monitorado em produção | 🔴 CRÍTICO | `/proc/self/status` + alertas |
| ☐ | Tempo de resposta de operações críticas medido e logado | 🟠 IMPORTANTE | `std::chrono` |
| ☐ | Métrica de taxa de erros de banco | 🟠 IMPORTANTE | Contador interno |
| ☐ | Watchdog timer para detectar freeze da aplicação | 🟠 IMPORTANTE | QTimer customizado |

**Medição de tempo de operação:**

```cpp
// RAII timer para medir operações
class ScopedTimer {
public:
    explicit ScopedTimer(const QString& operation)
        : m_operation(operation)
        , m_start(std::chrono::steady_clock::now()) {}

    ~ScopedTimer() {
        auto elapsed = std::chrono::steady_clock::now() - m_start;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        if (ms > 100) { // só loga operações lentas
            qWarning() << m_operation << "levou" << ms << "ms";
        }
    }
private:
    QString m_operation;
    std::chrono::steady_clock::time_point m_start;
};

// Uso
void UserRepository::findAll() {
    ScopedTimer timer("UserRepository::findAll");
    // ... query ao banco
}
```

---

## 7. Roadmap de Performance

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Event loop desbloqueada | Mover I/O e banco para threads | UI responde sempre em < 16ms |
| 🔴 2 | Eliminar N+1 | Auditar queries com slow log, refatorar para JOIN | Slow log vazio em uso normal |
| 🔴 3 | Model/View eficiente | data() sem query, carregamento em lote | Qt Profiler: zero spikes |
| 🟠 4 | Strings otimizadas | QStringLiteral, QStringView nos hot paths | Heaptrack: menos alocações |
| 🟠 5 | Índices MySQL | EXPLAIN em todas as queries frequentes | Nenhum full table scan |
| 🟠 6 | Logging e crash report | Handler customizado + Sentry | Stack trace em todo crash |
| 🟢 7 | LTO + PGO | Build de release com -O3 -flto + PGO | Benchmark 10%+ mais rápido |
| 🟢 8 | Profiling contínuo | Heaptrack no CI para detectar regressões | Alertas de regressão de performance |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I8 — DevOps & Build — CMake, GitHub Actions, CPack

# 🚀 Auditoria de DevOps, Build System & Deploy
## Stack: C++23 · Qt6 · CMake · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | CMake / CI/CD / Empacotamento / Deploy |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  CMAKE          Estrutura, targets, dependências         │
├─────────────────────────────────────────────────────────┤
│  CI/CD          Build, testes, análise estática, release │
├─────────────────────────────────────────────────────────┤
│  EMPACOTAMENTO  CPack, AppImage, instalador, DLL deploy  │
├─────────────────────────────────────────────────────────┤
│  VERSIONAMENTO  Semver, changelogs, tags, releases       │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. CMake — Build System

### 2.1 Estrutura e Organização

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `CMakeLists.txt` na raiz + subdiretorios com `add_subdirectory()` | 🔴 CRÍTICO |
| ☐ | Versão mínima do CMake declarada: `cmake_minimum_required(VERSION 3.21)` | 🔴 CRÍTICO |
| ☐ | Versão do projeto declarada: `project(App VERSION 1.0.0)` | 🔴 CRÍTICO |
| ☐ | Padrão C++ declarado: `set(CMAKE_CXX_STANDARD 23)` | 🔴 CRÍTICO |
| ☐ | `CMAKE_CXX_STANDARD_REQUIRED ON` e `CMAKE_CXX_EXTENSIONS OFF` | 🔴 CRÍTICO |
| ☐ | Sem flags hardcoded no `CMakeLists.txt` — usar targets e options | 🟠 IMPORTANTE |
| ☐ | Build out-of-source sempre (diretório `build/` separado do source) | 🔴 CRÍTICO |

### 2.2 Targets e Dependências

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `target_compile_options` em vez de `add_compile_options` (escopo de target) | 🔴 CRÍTICO |
| ☐ | `target_link_libraries` com `PRIVATE`/`PUBLIC`/`INTERFACE` corretos | 🔴 CRÍTICO |
| ☐ | Biblioteca da aplicação separada do executável (para testes) | 🔴 CRÍTICO |
| ☐ | `find_package()` para dependências externas — não caminhos hardcoded | 🔴 CRÍTICO |
| ☐ | `FetchContent` ou vcpkg para dependências não disponíveis no sistema | 🟠 IMPORTANTE |
| ☐ | `CMAKE_EXPORT_COMPILE_COMMANDS=ON` para ferramentas de análise estática | 🟠 IMPORTANTE |

**CMakeLists.txt base recomendado:**

```cmake
cmake_minimum_required(VERSION 3.21)
project(MyApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Evitar builds in-source
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds proibidos. Use: cmake -B build")
endif()

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
qt_standard_project_setup()

# Biblioteca da aplicação (sem main — para poder testar)
add_library(AppLib STATIC)
add_subdirectory(src)

# Executável principal
qt_add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE AppLib)

# Testes
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Include flags de segurança
include(cmake/CompilerFlags.cmake)
include(cmake/Sanitizers.cmake)
```

### 2.3 Qt6 Específico no CMake

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `qt_add_executable()` em vez de `add_executable()` | 🔴 CRÍTICO |
| ☐ | `qt_add_resources()` para arquivos `.qrc` | 🟠 IMPORTANTE |
| ☐ | `qt_add_translations()` para arquivos de tradução | 🟠 IMPORTANTE |
| ☐ | `AUTOMOC ON` ou `qt_standard_project_setup()` habilitado | 🔴 CRÍTICO |
| ☐ | `AUTOUIC ON` para arquivos `.ui` do Qt Designer | 🟠 IMPORTANTE |
| ☐ | `AUTORCC ON` para arquivos `.qrc` | 🟠 IMPORTANTE |

---

## 3. Presets do CMake

```json
// CMakePresets.json — configurações padrão para dev e CI
{
  "version": 6,
  "configurePresets": [
    {
      "name": "dev",
      "displayName": "Desenvolvimento",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/dev",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "ENABLE_SANITIZERS": "ON",
        "BUILD_TESTS": "ON"
      }
    },
    {
      "name": "release",
      "displayName": "Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "ENABLE_SANITIZERS": "OFF",
        "BUILD_TESTS": "OFF"
      }
    },
    {
      "name": "ci",
      "displayName": "CI/CD",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/ci",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo",
        "ENABLE_SANITIZERS": "ON",
        "BUILD_TESTS": "ON",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    }
  ],
  "buildPresets": [
    { "name": "dev",     "configurePreset": "dev" },
    { "name": "release", "configurePreset": "release" },
    { "name": "ci",      "configurePreset": "ci" }
  ],
  "testPresets": [
    {
      "name": "ci",
      "configurePreset": "ci",
      "output": { "outputOnFailure": true },
      "execution": { "noTestsAction": "error", "stopOnFailure": false }
    }
  ]
}
```

---

## 4. CI/CD Pipeline

### 4.1 Checklist do pipeline

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Build automatizado a cada push/PR | 🔴 CRÍTICO |
| ☐ | Testes (GTest + QTest) executados no CI | 🔴 CRÍTICO |
| ☐ | Análise estática (clang-tidy + cppcheck) no CI | 🔴 CRÍTICO |
| ☐ | Build com sanitizers (ASan + UBSan) no CI | 🔴 CRÍTICO |
| ☐ | Múltiplas plataformas testadas (Linux, Windows, macOS) | 🟠 IMPORTANTE |
| ☐ | Múltiplos compiladores testados (GCC + Clang) | 🟠 IMPORTANTE |
| ☐ | Relatório de cobertura gerado e publicado | 🟠 IMPORTANTE |
| ☐ | Build de release gerado em tags | 🟠 IMPORTANTE |
| ☐ | Tempo total do pipeline < 15 minutos | 🟠 IMPORTANTE |

### 4.2 GitHub Actions — Pipeline Completo

```yaml
name: CI/CD

on:
  push:
    branches: [main, develop]
    tags: ['v*']
  pull_request:
    branches: [main]

jobs:
  build-and-test:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        compiler: [gcc, clang]
        exclude:
          - os: windows-latest
            compiler: clang  # simplificação
    runs-on: ${{ matrix.os }}

    steps:
      - uses: actions/checkout@v4

      - name: Instalar Qt6
        uses: jurplel/install-qt-action@v3
        with:
          version: '6.7.*'
          modules: 'qtmysql'

      - name: Instalar GTest
        run: |
          sudo apt-get install -y libgtest-dev libgmock-dev  # Linux
          # Windows: vcpkg install gtest

      - name: Configure
        run: cmake --preset ci

      - name: Build
        run: cmake --build --preset ci

      - name: Testes
        run: ctest --preset ci --output-on-failure

      - name: Cobertura
        if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'gcc'
        run: |
          lcov --capture --directory build/ci --output-file coverage.info
          lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
          genhtml coverage.info --output-directory coverage_html

      - name: Upload cobertura
        if: matrix.os == 'ubuntu-latest' && matrix.compiler == 'gcc'
        uses: codecov/codecov-action@v4

  static-analysis:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Configure (para compile_commands.json)
        run: cmake --preset ci

      - name: clang-tidy
        run: |
          run-clang-tidy -p build/ci \
            -checks='clang-analyzer-*,cert-*,cppcoreguidelines-*,bugprone-*' \
            -warnings-as-errors='clang-analyzer-security.*'

      - name: cppcheck
        run: |
          cppcheck --enable=all --error-exitcode=1 \
            --suppress=missingInclude \
            --project=build/ci/compile_commands.json

      - name: Flawfinder
        run: |
          pip install flawfinder
          flawfinder --error-level=2 src/

  release:
    runs-on: ubuntu-latest
    needs: [build-and-test, static-analysis]
    if: startsWith(github.ref, 'refs/tags/v')
    steps:
      - uses: actions/checkout@v4

      - name: Instalar Qt6
        uses: jurplel/install-qt-action@v3
        with:
          version: '6.7.*'

      - name: Build Release
        run: |
          cmake --preset release
          cmake --build --preset release

      - name: Empacotar (CPack)
        run: |
          cd build/release
          cpack -G DEB     # Linux .deb
          cpack -G TGZ     # tarball

      - name: Publicar Release no GitHub
        uses: softprops/action-gh-release@v2
        with:
          files: build/release/*.deb
```

---

## 5. Empacotamento

### 5.1 CPack (multi-plataforma)

```cmake
# cmake/Packaging.cmake
include(CPack)
include(GNUInstallDirs)

# Instalar binário
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Instalar Qt6 runtime (via windeployqt/linuxdeployqt)
if(WIN32)
    find_program(WINDEPLOYQT windeployqt)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${WINDEPLOYQT} $<TARGET_FILE:${PROJECT_NAME}>
    )
endif()

# Configurações CPack
set(CPACK_PACKAGE_NAME "MyApp")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Sua Empresa")
set(CPACK_PACKAGE_DESCRIPTION "Descrição da aplicação")
set(CPACK_PACKAGE_CONTACT "dev@suaempresa.com.br")

# Linux
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libmysqlclient21")
set(CPACK_GENERATOR "DEB;TGZ")

# Windows
# set(CPACK_GENERATOR "NSIS;ZIP")
```

### 5.2 Linux — AppImage

```bash
# Gerar AppImage (self-contained, sem dependências externas)
cmake --preset release
cmake --build --preset release
cmake --install build/release --prefix AppDir/usr

# linuxdeployqt
linuxdeployqt AppDir/usr/bin/myapp \
    -qmldir=src/qml \
    -appimage \
    -extra-plugins=sqldrivers/libqsqlmysql.so
```

### 5.3 Checklist de Empacotamento

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Todas as DLLs/SOs do Qt6 incluídas no pacote | 🔴 CRÍTICO |
| ☐ | Driver MySQL Qt6 (`libqsqlmysql.so`/`qsqlmysql.dll`) incluído | 🔴 CRÍTICO |
| ☐ | Arquivos de tradução (`.qm`) incluídos no pacote | 🔴 CRÍTICO |
| ☐ | Ícone e metadata da aplicação incluídos | 🟠 IMPORTANTE |
| ☐ | Versão embarcada no binário (`-DAPP_VERSION=${PROJECT_VERSION}`) | 🟠 IMPORTANTE |
| ☐ | Hash SHA256 do pacote gerado para verificação de integridade | 🟠 IMPORTANTE |
| ☐ | Testado em máquina limpa sem Qt instalado | 🔴 CRÍTICO |

---

## 6. Versionamento Semântico

### 6.1 Convenções

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Versão em formato SemVer: `MAJOR.MINOR.PATCH` | 🔴 CRÍTICO |
| ☐ | Versão declarada em `CMakeLists.txt` como única fonte de verdade | 🔴 CRÍTICO |
| ☐ | Tags Git no formato `v1.2.3` para cada release | 🔴 CRÍTICO |
| ☐ | `CHANGELOG.md` atualizado a cada release | 🟠 IMPORTANTE |
| ☐ | Versão exibida na UI (`--version` ou menu Sobre) | 🟠 IMPORTANTE |
| ☐ | Build number incorporado (hash do commit) em versões de desenvolvimento | 🟢 COSMÉTICO |

**Embutir versão no código:**

```cmake
# Gerar header com versão em tempo de build
configure_file(
    cmake/version.h.in
    ${CMAKE_BINARY_DIR}/generated/version.h
)

# cmake/version.h.in
#pragma once
#define APP_VERSION "@PROJECT_VERSION@"
#define APP_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define APP_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define APP_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define APP_GIT_HASH "@GIT_HASH@"
```

---

## 7. Qualidade de Código no Pipeline

### 7.1 Cobertura de Testes — Gate de Qualidade

```yaml
- name: Verificar cobertura mínima
  run: |
    COVERAGE=$(lcov --summary coverage.info 2>&1 | grep 'lines' | grep -oP '\d+\.\d+(?=%)')
    echo "Cobertura: $COVERAGE%"
    if (( $(echo "$COVERAGE < 60" | bc -l) )); then
        echo "❌ Cobertura $COVERAGE% abaixo do mínimo de 60%"
        exit 1
    fi
    echo "✅ Cobertura $COVERAGE% aprovada"
```

### 7.2 Métricas de Build

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Tempo de build monitorado — alerta se aumentar > 20% | 🟠 IMPORTANTE |
| ☐ | Tamanho do binário monitorado — alerta se aumentar > 10% | 🟢 COSMÉTICO |
| ☐ | Build paralelo habilitado (`cmake --build build -j$(nproc)`) | 🟠 IMPORTANTE |
| ☐ | `ccache` configurado para acelerar rebuilds | 🟢 COSMÉTICO |

```cmake
# Habilitar ccache se disponível
find_program(CCACHE ccache)
if(CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    message(STATUS "ccache habilitado: ${CCACHE}")
endif()
```

---

## 8. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria de build system e DevOps deste projeto C++23/Qt6.
Reporte em português.

## CMAKE
- cmake_minimum_required e project() corretos?
- CMAKE_CXX_STANDARD 23 configurado?
- target_compile_options/link_libraries com escopo PRIVATE/PUBLIC?
- Biblioteca separada do executável (para testes)?
- find_package() para Qt6 e dependências?
- CMAKE_EXPORT_COMPILE_COMMANDS habilitado?

## CI/CD
- Build automatizado no CI?
- Testes executados no CI?
- Análise estática (clang-tidy/cppcheck) no CI?
- Build com sanitizers (ASan/UBSan) no CI?
- Múltiplas plataformas testadas?

## EMPACOTAMENTO
- CPack configurado?
- Qt6 DLLs/SOs incluídas?
- Driver MySQL incluído?
- Testado em máquina limpa?

## VERSIONAMENTO
- SemVer usado?
- Tags Git para releases?
- CHANGELOG atualizado?
- Versão embutida no binário?

## DIAGNÓSTICO FINAL
- Principais lacunas de CI/CD
- Riscos no empacotamento
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 9. Roadmap DevOps

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | CMake moderno | cmake_minimum 3.21, CXX_STANDARD 23, target-scoped | Build reproduzível de dev |
| 🔴 2 | CI básico | GitHub Actions: build + testes em Ubuntu | PR sem CI bloqueado |
| 🔴 3 | Análise estática | clang-tidy + cppcheck no CI | Zero warnings de segurança |
| 🟠 4 | Sanitizers no CI | ASan + UBSan em build de teste | CI com sanitizers verdes |
| 🟠 5 | Multi-plataforma | Matrix build: Linux + Windows | Binário para ambos |
| 🟠 6 | Empacotamento | CPack DEB + AppImage ou NSIS | Testado em máquina limpa |
| 🟢 7 | Cobertura | lcov + gate de 60% mínimo | CI rejeita PR abaixo do mínimo |
| 🟢 8 | ccache + PGO | Builds mais rápidos + otimização guiada | Build CI < 10 minutos |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## I9 — Qualidade de Código — God Classes, Complexidade, Dead Code

# 🧹 Auditoria de Qualidade de Código & Dívida Técnica
## Stack: C++23 · Qt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | C++23 / Qt6 |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  CODE SMELLS       God class, feature envy, magic nums   │
├─────────────────────────────────────────────────────────┤
│  COMPLEXIDADE      Ciclomática, acoplamento, coesão      │
├─────────────────────────────────────────────────────────┤
│  NOMENCLATURA      Convenções C++, Qt, namespace         │
├─────────────────────────────────────────────────────────┤
│  DEAD CODE         Código morto, includes órfãos         │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Code Smells — C++23/Qt6

### 2.1 God Class / God Function

| # | Sintoma | Limiar | Prioridade | Ferramenta |
|---|---------|--------|-----------|------------|
| ☐ | Classe com mais de 300 linhas | > 300 linhas | 🔴 CRÍTICO | cloc / wc |
| ☐ | Método com mais de 30 linhas | > 30 linhas | 🔴 CRÍTICO | clang-tidy |
| ☐ | Classe com mais de 10 métodos públicos | > 10 | 🟠 IMPORTANTE | Revisão manual |
| ☐ | `MainWindow.cpp` com mais de 500 linhas | > 500 | 🔴 CRÍTICO | wc -l |
| ☐ | Construtor com mais de 20 linhas | > 20 | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Função com mais de 5 parâmetros | > 5 params | 🟠 IMPORTANTE | clang-tidy |
| ☐ | Nível de indentação acima de 4 | > 4 | 🟠 IMPORTANTE | clang-tidy |

### 2.2 Qt-Específicos

| # | Sintoma | Prioridade |
|---|---------|-----------|
| ☐ | Slot com mais de 10 linhas fazendo lógica de negócio | 🔴 CRÍTICO |
| ☐ | `QWidget` subclass com acesso direto a banco de dados | 🔴 CRÍTICO |
| ☐ | Signals/slots usados onde chamada direta seria suficiente (overengineering) | 🟢 COSMÉTICO |
| ☐ | `QTimer::singleShot` espalhado como gambiarra de sincronização | 🟠 IMPORTANTE |
| ☐ | `QVariant` usado como tipo "coringa" em vez de tipos concretos | 🟠 IMPORTANTE |
| ☐ | `qobject_cast` sem verificação de nullptr | 🔴 CRÍTICO |

### 2.3 Feature Envy

| # | Sintoma | Prioridade |
|---|---------|-----------|
| ☐ | Método que acessa mais dados de outra classe do que da própria | 🟠 IMPORTANTE |
| ☐ | Service que manipula internamente os campos de uma entidade | 🟠 IMPORTANTE |
| ☐ | Slot que acessa diretamente propriedades de múltiplos widgets de outro form | 🟠 IMPORTANTE |

### 2.4 Primitive Obsession em C++/Qt6

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | Dinheiro como `double` (perda de precisão) | `int` em centavos ou Value Object `Money` | 🔴 CRÍTICO |
| ☐ | Status como `QString` ou `int` mágico | `enum class Status { Active, Inactive }` | 🔴 CRÍTICO |
| ☐ | CPF como `QString` sem validação | Value Object `CPF` com validação no construtor | 🟠 IMPORTANTE |
| ☐ | Datas como `QString` | `QDate` ou `std::chrono::year_month_day` | 🔴 CRÍTICO |
| ☐ | IDs como `int` puro sem tipo explícito | `using UserId = int;` ou Strong Typedef | 🟢 COSMÉTICO |

### 2.5 Magic Numbers e Magic Strings em Qt6

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | `setFixedWidth(120)` hardcoded | Constante nomeada ou proporcional ao conteúdo | 🟠 IMPORTANTE |
| ☐ | `Qt::UserRole + 3` sem constante | `const int IdRole = Qt::UserRole + 1;` | 🟠 IMPORTANTE |
| ☐ | `"users"` (nome de tabela) repetido em vários arquivos | Constante em namespace dedicado | 🔴 CRÍTICO |
| ☐ | Cores hardcoded em código: `QColor(52, 73, 94)` | Constante nomeada ou tema | 🟠 IMPORTANTE |
| ☐ | Timeouts hardcoded: `QTimer::singleShot(5000, ...)` | Constante `kDefaultTimeoutMs` | 🟠 IMPORTANTE |

---

## 3. Complexidade Ciclomática

### 3.1 Limites recomendados

| Complexidade | Classificação | Ação |
|-------------|--------------|------|
| 1–5 | ✅ Simples | Nenhuma |
| 6–10 | ⚠️ Moderado | Monitorar |
| 11–20 | 🟠 Alto | Refatorar — decompor em funções |
| > 20 | 🔴 Crítico | Urgente — código não testável |

### 3.2 Checklist de Complexidade

| # | Item | Meta | Prioridade | Ferramenta |
|---|------|------|-----------|------------|
| ☐ | Complexidade ciclomática máxima por função | ≤ 15 | 🔴 CRÍTICO | lizard / pmccabe |
| ☐ | Complexidade média do projeto | ≤ 5 | 🟠 IMPORTANTE | lizard |
| ☐ | Funções com complexidade > 10 identificadas e refatoradas | — | 🔴 CRÍTICO | lizard |

```bash
# Analisar complexidade com lizard
pip install lizard
lizard src/ -l cpp -C 10 -w  # avisa em complexidade > 10
lizard src/ -l cpp --csv > complexity_report.csv
```

---

## 4. Acoplamento

### 4.1 Checklist

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Includes desnecessários em headers (prefer forward declarations) | 🔴 CRÍTICO | include-what-you-use |
| ☐ | Headers com `#include` de implementação (vaza dependências) | 🔴 CRÍTICO | iwyu |
| ☐ | Dependências circulares entre módulos | 🔴 CRÍTICO | Revisão manual |
| ☐ | Classes de domínio sem dependência de Qt | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Número de includes por header | ≤ 10 | 🟠 IMPORTANTE | wc |

**include-what-you-use:**

```bash
# Instalar e rodar
sudo apt install iwyu
cmake -B build -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=include-what-you-use
cmake --build build 2>&1 | grep "should"
```

### 4.2 Forward Declarations

```cpp
// ✅ CORRETO — forward declaration no header, include no .cpp
// UserService.h
class IUserRepository;  // forward declaration
class User;             // forward declaration

class UserService {
    std::unique_ptr<IUserRepository> m_repo;  // ponteiro — ok com forward decl
};

// UserService.cpp
#include "IUserRepository.h"  // include apenas onde necessário
#include "User.h"

// ❌ ERRADO — include desnecessário no header propaga dependências
// UserService.h
#include "IUserRepository.h"  // propaga para todos que incluem UserService.h
#include "User.h"
#include <QSqlDatabase>       // vaza dependência de infraestrutura
```

---

## 5. Dead Code

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Funções/métodos nunca chamados | 🟠 IMPORTANTE | clang-tidy `misc-unused-*` |
| ☐ | Variáveis declaradas e nunca usadas | 🔴 CRÍTICO | `-Wunused-variable` |
| ☐ | Includes nunca usados | 🟠 IMPORTANTE | iwyu |
| ☐ | Código comentado em bloco (`/* ... */`) de mais de 10 linhas | 🟢 COSMÉTICO | Grep manual |
| ☐ | `#ifdef 0` / código nunca compilado | 🟠 IMPORTANTE | Grep manual |
| ☐ | Slots sem sinal conectado | 🟢 COSMÉTICO | Revisão manual |
| ☐ | Arquivos `.cpp`/`.h` sem `#include` em nenhum lugar | 🟠 IMPORTANTE | CMake / Grep |
| ☐ | `TODO`/`FIXME`/`HACK` catalogados e priorizados | 🟢 COSMÉTICO | `grep -rn "TODO\|FIXME\|HACK"` |

---

## 6. Nomenclatura — Convenções C++/Qt6

### 6.1 Padrão Qt6 (recomendado para projetos Qt)

| Entidade | Convenção Qt | Exemplo |
|----------|-------------|---------|
| Classes | PascalCase | `UserRepository`, `MainWindow` |
| Métodos / funções | camelCase | `findById()`, `setName()` |
| Variáveis de membro | `m_` prefix + camelCase | `m_userName`, `m_isActive` |
| Variáveis locais | camelCase | `userId`, `itemCount` |
| Constantes / enum values | PascalCase (C++11 enum class) | `Status::Active` |
| Constantes globais | `k` prefix ou `ALL_CAPS` | `kMaxRetries`, `MAX_TIMEOUT` |
| Namespaces | lowercase | `app::domain::user` |
| Signals | camelCase, passado | `dataChanged`, `userCreated` |
| Slots | camelCase, presente | `onDataChanged`, `handleUserCreated` |
| Arquivos | PascalCase = classe | `UserRepository.cpp/.h` |

### 6.2 Checklist de Nomenclatura

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Convenção escolhida e aplicada consistentemente em todo o projeto | 🔴 CRÍTICO |
| ☐ | Sem mistura de idiomas: nomes todos em inglês ou todos em português | 🔴 CRÍTICO |
| ☐ | Variáveis de membro com prefixo `m_` | 🟠 IMPORTANTE |
| ☐ | Booleanos com prefixo `is`/`has`/`can`: `m_isLoading`, `m_hasError` | 🟠 IMPORTANTE |
| ☐ | Classes com sufixo de papel: `...Service`, `...Repository`, `...Widget` | 🟠 IMPORTANTE |
| ☐ | Signals em passado (evento ocorreu): `clicked`, `dataLoaded` | 🟠 IMPORTANTE |
| ☐ | Slots em presente (reação): `handleClick`, `onDataLoaded` | 🟠 IMPORTANTE |
| ☐ | Sem abreviações obscuras: `usrNm` → `userName` | 🟠 IMPORTANTE |
| ☐ | Funções getter sem `get`: `name()` — não `getName()` (padrão Qt) | 🟢 COSMÉTICO |

---

## 7. Headers e Organização de Arquivos

### 7.1 Checklist de Headers

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Include guards em todo header: `#pragma once` | 🔴 CRÍTICO |
| ☐ | Ordem de includes: próprio header → C++ std → Qt → terceiros → projeto | 🟠 IMPORTANTE |
| ☐ | Sem implementação de método não-trivial em header (exceto templates) | 🟠 IMPORTANTE |
| ☐ | Um arquivo `.h` + um `.cpp` por classe — sem exceções sem justificativa | 🟠 IMPORTANTE |
| ☐ | Headers autocontidos (compilam sozinhos sem includes implícitos) | 🔴 CRÍTICO |

**Ordem correta de includes:**

```cpp
// MyWidget.cpp
#include "MyWidget.h"          // 1. próprio header primeiro

#include <algorithm>           // 2. C++ standard library
#include <memory>

#include <QApplication>        // 3. Qt headers
#include <QVBoxLayout>
#include <QPushButton>

#include <spdlog/spdlog.h>     // 4. terceiros

#include "UserService.h"       // 5. projeto (outros módulos)
#include "domain/User.h"
```

---

## 8. Ferramentas de Análise de Qualidade

| Ferramenta | Foco | Comando |
|-----------|------|---------|
| **lizard** | Complexidade ciclomática | `lizard src/ -l cpp -C 10 -w` |
| **clang-tidy** | Code smells, modernização | `run-clang-tidy -p build/` |
| **cppcheck** | Bugs, dead code | `cppcheck --enable=all src/` |
| **iwyu** | Includes desnecessários | Via CMake |
| **cloc** | Contagem de linhas | `cloc src/ --by-file` |
| **Doxygen** | Documentação / acoplamento | `doxygen Doxyfile` |

**Relatório de complexidade no CI:**

```yaml
- name: Análise de Complexidade
  run: |
    pip install lizard
    lizard src/ -l cpp -C 15 -w --csv > complexity.csv
    # Falha se há funções com complexidade > 15
    lizard src/ -l cpp -C 15 --error-exit-code-threshold 1
```

---

## 9. Índice de Manutenibilidade

| Faixa | Classificação | Ação |
|-------|--------------|------|
| 85–100 | ✅ Excelente | Manter |
| 65–84 | ⚠️ Aceitável | Monitorar |
| 40–64 | 🟠 Problemático | Refatorar |
| < 40 | 🔴 Crítico | Refatoração urgente |

---

## 10. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria de qualidade de código e dívida técnica deste projeto C++23/Qt6.
Reporte em português.

## CODE SMELLS
- Identifique as 5 maiores classes (God Classes) em linhas
- Identifique os 5 métodos mais longos e com maior complexidade
- Há slots com lógica de negócio?
- Há QWidget com acesso direto ao banco?
- Há Primitive Obsession (double para dinheiro, string para status)?
- Há Magic Numbers e Magic Strings?

## COMPLEXIDADE
- Quais são os 5 métodos com maior complexidade ciclomática?
- Qual a complexidade média estimada?

## ACOPLAMENTO
- Há includes desnecessários nos headers?
- Classes de domínio dependem de Qt?
- Há dependências circulares entre módulos?

## DEAD CODE
- Há métodos/funções claramente sem uso?
- Há #include sem uso?
- Há arquivos .cpp sem referência no CMake?
- Quantos TODO/FIXME há no código?

## NOMENCLATURA
- A convenção (Qt ou outra) é consistente?
- Há mistura de idiomas?
- Variáveis de membro têm prefixo m_?
- Signals e slots seguem convenção de tempo verbal?

## DIAGNÓSTICO FINAL
- Estime o Índice de Manutenibilidade geral
- 5 maiores dívidas técnicas por impacto
- Horas estimadas de refatoração por grupo
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 11. Roadmap de Qualidade

| Sprint | Objetivo | Ações | Métrica |
|--------|----------|-------|---------|
| 🔴 1 | Eliminar God Classes | Decompor as 3 maiores classes | Nenhuma classe > 300 linhas |
| 🔴 2 | Complexidade crítica | Refatorar funções com CC > 15 | CC máxima ≤ 15 |
| 🔴 3 | Eliminar Magic Strings | Constantes para nomes de tabelas e roles Qt | grep zero |
| 🟠 4 | Value Objects | Money, CPF, Email com tipos próprios | Zero double para dinheiro |
| 🟠 5 | Includes limpos | iwyu em todos os headers | Build sem warnings de include |
| 🟠 6 | Nomenclatura | Padronizar convenção escolhida | clang-tidy naming zero |
| 🟢 7 | Dead code | Remover código morto e comentado | Zero #ifdef 0 e blocos grandes |
| 🟢 8 | Manutenibilidade | lizard MI médio no CI | MI médio ≥ 65 |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

# Parte II — Python / Qt6 / SQLite


---

## II1 — Auditoria Completa Python/Qt6

# 🖥️ Auditoria Qt6/PySide6 Específico
## Widgets · Model/View · Signals · i18n · Acessibilidade

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | PySide6 / PyQt6 |

---

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

## 1. Layouts e Responsividade

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Sem `setFixedSize()` hardcoded — usar `QSizePolicy` | 🔴 CRÍTICO |
| ☐ | Sem `move()` ou `resize()` absoluto | 🔴 CRÍTICO |
| ☐ | Todos os widgets dentro de layouts | 🔴 CRÍTICO |
| ☐ | HiDPI: ícones SVG, tamanhos em `pt` não `px` | 🔴 CRÍTICO |
| ☐ | `QSplitter` para divisões redimensionáveis | 🟠 IMPORTANTE |
| ☐ | `QScrollArea` para conteúdo variável | 🟠 IMPORTANTE |

## 2. Model/View

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Listas/tabelas via `QAbstractItemModel` — não widgets diretos | 🔴 CRÍTICO |
| ☐ | `beginInsertRows`/`endInsertRows` emitidos corretamente | 🔴 CRÍTICO |
| ☐ | `dataChanged` emitido após update — não `reset()` inteiro | 🔴 CRÍTICO |
| ☐ | `QSortFilterProxyModel` para filtros — não recarregando banco | 🔴 CRÍTICO |
| ☐ | `data()` do model é O(1) — sem query por chamada | 🔴 CRÍTICO |

```python
# ✅ QAbstractTableModel correto em Python
from PySide6.QtCore import QAbstractTableModel, Qt, QModelIndex
from typing import Any

class UserTableModel(QAbstractTableModel):
    HEADERS = ["ID", "Nome", "E-mail"]

    def __init__(self, parent=None):
        super().__init__(parent)
        self._users: list[User] = []

    def rowCount(self, parent=QModelIndex()) -> int:
        return len(self._users)

    def columnCount(self, parent=QModelIndex()) -> int:
        return len(self.HEADERS)

    def data(self, index: QModelIndex, role=Qt.DisplayRole) -> Any:
        if not index.isValid() or role != Qt.DisplayRole:
            return None
        user = self._users[index.row()]
        match index.column():
            case 0: return user.id
            case 1: return user.name
            case 2: return user.email
            case _: return None

    def headerData(self, section, orientation, role=Qt.DisplayRole):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole:
            return self.HEADERS[section]

    def add_user(self, user: User) -> None:
        row = len(self._users)
        self.beginInsertRows(QModelIndex(), row, row)  # OBRIGATÓRIO
        self._users.append(user)
        self.endInsertRows()                            # OBRIGATÓRIO
```

## 3. Signals e Slots

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Conexões sem método duplicado (connect chamado múltiplas vezes) | 🔴 CRÍTICO |
| ☐ | Lambdas com referência a `self` — verificar lifetime | 🔴 CRÍTICO |
| ☐ | Cross-thread signals com `Qt.QueuedConnection` | 🔴 CRÍTICO |
| ☐ | `QSignalBlocker` quando precisa atualizar widget sem emitir signal | 🟠 IMPORTANTE |
| ☐ | Slots desconectados quando objeto é destruído | 🟠 IMPORTANTE |

```python
# ✅ Boas práticas de signals Python/PySide6
from PySide6.QtCore import Signal, QObject

class UserService(QObject):
    user_created = Signal(User)      # tipado
    error_occurred = Signal(str)
    loading_changed = Signal(bool)

# ✅ Conexão com contexto correto
self.service.user_created.connect(self.model.add_user)

# ✅ Desbloquear signals temporariamente
from PySide6.QtCore import QSignalBlocker
with QSignalBlocker(self.combo_box):
    self.combo_box.setCurrentIndex(index)  # sem emitir currentIndexChanged
```

## 4. i18n — Locale Brasileiro

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Toda string visível ao usuário em `self.tr()` | 🔴 CRÍTICO |
| ☐ | `QLocale` configurado: `QLocale(QLocale.Portuguese, QLocale.Brazil)` | 🔴 CRÍTICO |
| ☐ | Datas via `QLocale.toString(date, QLocale.ShortFormat)` | 🔴 CRÍTICO |
| ☐ | Moeda: `locale.toCurrencyString(valor)` → `R$ 1.234,56` | 🔴 CRÍTICO |
| ☐ | Timezone: `QTimeZone(b"America/Sao_Paulo")` | 🔴 CRÍTICO |
| ☐ | Separador decimal: vírgula — não ponto | 🔴 CRÍTICO |

```python
# main.py — configuração de locale
from PySide6.QtCore import QLocale, QTranslator
from PySide6.QtWidgets import QApplication
import sys

app = QApplication(sys.argv)

# Locale brasileiro
locale = QLocale(QLocale.Portuguese, QLocale.Brazil)
QLocale.setDefault(locale)

# Tradução
translator = QTranslator()
if translator.load(locale, "app", "_", ":/translations"):
    app.installTranslator(translator)

# Uso correto no código
locale = QLocale()
preco = locale.toCurrencyString(1234.56)  # "R$ 1.234,56"
data  = locale.toString(QDate.currentDate(), QLocale.ShortFormat)  # "15/01/2024"
```

## 5. Performance de UI

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Sem operações bloqueantes na event loop | 🔴 CRÍTICO |
| ☐ | `QApplication.processEvents()` não usado como gambiarra | 🔴 CRÍTICO |
| ☐ | Atualizações em batch com `setUpdatesEnabled(False/True)` | 🟠 IMPORTANTE |
| ☐ | `QTimer` para operações periódicas — não `time.sleep()` | 🔴 CRÍTICO |
| ☐ | Virtual scrolling via Model/View — não widget por item | 🔴 CRÍTICO |

## 6. Prompt de Auditoria Qt6/PySide6

```
Faça uma auditoria PySide6/PyQt6 deste projeto Python.
Reporte em português.

- Há setFixedSize() ou coordenadas absolutas?
- Listas/tabelas usam QAbstractItemModel?
- beginInsertRows/endInsertRows emitidos corretamente?
- data() do model executa query por chamada?
- Há conexões de signals duplicadas?
- Lambdas em connect sem verificação de lifetime?
- Cross-thread signals com QueuedConnection?
- Todas as strings de UI em tr()?
- QLocale configurado para pt_BR?
- Há QApplication.processEvents() como gambiarra?
- Operações longas bloqueiam a event loop?
```

---

---

# 🗄️ Auditoria de Banco de Dados — MySQL + QSql + SQLAlchemy

## Stack: Python · PySide6 · MySQL

---

## 1. QSql — Queries e Segurança

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | 100% das queries com `QSqlQuery.prepare()` + `bindValue()` | 🔴 CRÍTICO |
| ☐ | Sem f-string ou concatenação em SQL | 🔴 CRÍTICO |
| ☐ | `query.lastError()` verificado após todo `exec()` | 🔴 CRÍTICO |
| ☐ | Transações via `db.transaction()` + `db.commit()` / `db.rollback()` | 🟠 IMPORTANTE |
| ☐ | Sem N+1: loop Python com query por iteração | 🔴 CRÍTICO |
| ☐ | Toda listagem com `LIMIT` e `OFFSET` | 🔴 CRÍTICO |
| ☐ | Sem `SELECT *` — colunas explícitas | 🟠 IMPORTANTE |

```python
# ✅ Padrão completo QSql em Python
from PySide6.QtSql import QSqlQuery, QSqlDatabase
from contextlib import contextmanager

@contextmanager
def sql_transaction(db: QSqlDatabase):
    db.transaction()
    try:
        yield
        db.commit()
    except Exception:
        db.rollback()
        raise

class MySqlUserRepository(IUserRepository):
    def save(self, user: User) -> User:
        query = QSqlQuery(self._db)
        query.prepare("""
            INSERT INTO users (name, email, password_hash, created_at)
            VALUES (:name, :email, :password_hash, NOW())
        """)
        query.bindValue(":name", user.name)
        query.bindValue(":email", user.email)
        query.bindValue(":password_hash", user.password_hash)

        if not query.exec():
            raise RepositoryError(
                f"Falha ao salvar usuário: {query.lastError().text()}"
            )
        return User(id=query.lastInsertId(), **user.__dict__)
```

## 2. SQLAlchemy (alternativa ao QSql)

```python
# SQLAlchemy Core + Python — para projetos que preferem ORM
from sqlalchemy import create_engine, text
from sqlalchemy.orm import Session, DeclarativeBase, Mapped, mapped_column

class Base(DeclarativeBase):
    pass

class UserModel(Base):
    __tablename__ = "users"
    id:           Mapped[int]  = mapped_column(primary_key=True)
    name:         Mapped[str]  = mapped_column(nullable=False)
    email:        Mapped[str]  = mapped_column(unique=True, nullable=False)
    is_active:    Mapped[bool] = mapped_column(default=True)

class SQLAlchemyUserRepository(IUserRepository):
    def __init__(self, session: Session) -> None:
        self._session = session

    def find_by_id(self, user_id: int) -> User | None:
        model = self._session.get(UserModel, user_id)
        return self._to_entity(model) if model else None

    def save(self, user: User) -> User:
        model = UserModel(name=user.name, email=user.email)
        self._session.add(model)
        self._session.flush()  # obtém ID sem commit
        return self._to_entity(model)

    def _to_entity(self, model: UserModel) -> User:
        return User(id=model.id, name=model.name, email=model.email)
```

## 3. Schema MySQL

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Charset `utf8mb4` em todas as tabelas | 🔴 CRÍTICO |
| ☐ | `DECIMAL(15,2)` para valores monetários — nunca `FLOAT` | 🔴 CRÍTICO |
| ☐ | FK, NOT NULL, UNIQUE constraints definidas | 🟠 IMPORTANTE |
| ☐ | Índices em colunas de WHERE, JOIN, ORDER BY | 🔴 CRÍTICO |
| ☐ | Migrations versionadas (Alembic ou custom runner) | 🔴 CRÍTICO |
| ☐ | Backup automático e criptografado | 🔴 CRÍTICO |

## 4. Migrations com Alembic

```bash
# Instalação
pip install alembic

# Inicializar
alembic init migrations

# Gerar migration automática
alembic revision --autogenerate -m "create_users_table"

# Aplicar
alembic upgrade head

# Reverter
alembic downgrade -1
```

```python
# migrations/versions/001_create_users.py
def upgrade() -> None:
    op.create_table(
        "users",
        sa.Column("id", sa.Integer, primary_key=True, autoincrement=True),
        sa.Column("name", sa.String(255), nullable=False),
        sa.Column("email", sa.String(255), nullable=False, unique=True),
        sa.Column("password_hash", sa.String(255), nullable=False),
        sa.Column("is_active", sa.Boolean, default=True),
        sa.Column("created_at", sa.DateTime, server_default=sa.func.now()),
        mysql_charset="utf8mb4",
        mysql_collate="utf8mb4_unicode_ci",
    )
    op.create_index("idx_users_email", "users", ["email"])

def downgrade() -> None:
    op.drop_table("users")
```

## 5. Prompt de Auditoria MySQL/Python

```
Faça uma auditoria do banco de dados MySQL deste projeto Python/PySide6.
Reporte em português.

- Há queries sem prepare()/bindValue() (SQL Injection)?
- Há f-string ou concatenação em SQL?
- lastError() verificado após exec()?
- Há N+1 queries (loop com query por iteração)?
- Listagens têm LIMIT/OFFSET?
- Charset é utf8mb4?
- FLOAT/DOUBLE para dinheiro (deve ser DECIMAL)?
- Schema versionado com migrations?
- Backup automático configurado?
- Índices em colunas de WHERE/JOIN?
```

---

---

# ⚡ Auditoria de Performance & Observabilidade

## Stack: Python · PySide6 · MySQL

---

## 1. Performance Python/Qt

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem operações > 16ms na thread de UI | 🔴 CRÍTICO | cProfile |
| ☐ | Sem `time.sleep()` na thread principal | 🔴 CRÍTICO | Revisão manual |
| ☐ | `QStringLiteral` equivalente: strings Python são imutáveis — ok | ✅ N/A | — |
| ☐ | `functools.lru_cache` para computações repetidas | 🟠 IMPORTANTE | cProfile |
| ☐ | Generators para listas grandes — não listas em memória | 🟠 IMPORTANTE | memory_profiler |
| ☐ | `__slots__` em classes com muitas instâncias | 🟢 COSMÉTICO | memory_profiler |

## 2. Profiling Python

```bash
# cProfile — profiling de CPU
python -m cProfile -o profile.out src/main.py
python -m pstats profile.out

# Visualização gráfica com snakeviz
pip install snakeviz
snakeviz profile.out

# memory_profiler — profiling de memória
pip install memory_profiler
python -m memory_profiler src/main.py

# py-spy — profiling em produção sem parar o processo
pip install py-spy
py-spy top --pid $(pgrep -f "python main.py")
```

## 3. Logging Estruturado

```python
# logging_config.py
import logging
import json
from datetime import datetime

class JsonFormatter(logging.Formatter):
    def format(self, record: logging.LogRecord) -> str:
        # NUNCA logar dados pessoais (LGPD Art. 46)
        return json.dumps({
            "timestamp": datetime.utcnow().isoformat(),
            "level":     record.levelname,
            "logger":    record.name,
            "message":   record.getMessage(),
            "module":    record.module,
            "line":      record.lineno,
        })

def setup_logging(level: str = "INFO") -> None:
    handler = logging.FileHandler("app.log")
    handler.setFormatter(JsonFormatter())

    root = logging.getLogger()
    root.setLevel(level)
    root.addHandler(handler)
```

## 4. Health Check e Métricas

```python
# health.py
from PySide6.QtSql import QSqlQuery

def check_database(db) -> dict:
    query = QSqlQuery(db)
    ok = query.exec("SELECT 1")
    return {"status": "ok" if ok else "fail", "component": "database"}

def check_memory() -> dict:
    import psutil, os
    proc = psutil.Process(os.getpid())
    mb = proc.memory_info().rss / 1024 / 1024
    return {"status": "ok", "memory_mb": round(mb, 1)}
```

## 5. Prompt de Auditoria Performance

```
Faça uma auditoria de performance deste projeto Python/PySide6/MySQL.
Reporte em português.

- Há operações bloqueantes na event loop Qt?
- QApplication.processEvents() como gambiarra?
- data() do QAbstractItemModel executa query por chamada?
- Há N+1 queries (loop com query por iteração)?
- Generators usados para grandes volumes de dados?
- functools.lru_cache para computações repetidas?
- Logging estruturado implementado?
- Crash handler instalado em produção?
```

---

---

# 🚀 Auditoria de DevOps, Build & Empacotamento

## Stack: Python · PySide6 · MySQL

---

## 1. Gerenciamento de Dependências

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `pyproject.toml` como única fonte de verdade | 🔴 CRÍTICO |
| ☐ | Dependências fixadas com versão exata em `requirements.lock` | 🔴 CRÍTICO |
| ☐ | `requirements-dev.txt` separado de produção | 🟠 IMPORTANTE |
| ☐ | Ambiente virtual (`venv` ou `uv`) obrigatório | 🔴 CRÍTICO |
| ☐ | `safety check` no CI para CVEs | 🔴 CRÍTICO |

```toml
# pyproject.toml completo
[project]
name = "my-app"
version = "1.0.0"
requires-python = ">=3.11"
dependencies = [
    "PySide6>=6.7",
    "PyMySQL>=1.1",
    "python-dotenv>=1.0",
    "bcrypt>=4.0",
    "pydantic>=2.0",
    "cryptography>=42.0",
]

[project.optional-dependencies]
dev = [
    "pytest>=8.0",
    "pytest-qt>=4.0",
    "pytest-cov>=5.0",
    "pytest-mock>=3.0",
    "mypy>=1.9",
    "ruff>=0.4",
    "bandit>=1.7",
    "safety>=3.0",
]

[tool.pytest.ini_options]
testpaths = ["tests"]
addopts = "--cov=src --cov-report=html --cov-fail-under=60"

[tool.mypy]
python_version = "3.11"
strict = true

[tool.ruff.lint]
select = ["E","W","F","I","B","S","ANN","UP","N"]
```

## 2. Empacotamento Python/PySide6

| Ferramenta | Gera | Plataformas | Uso |
|-----------|------|------------|-----|
| **PyInstaller** | Executável único ou pasta | Linux, Win, macOS | Mais maduro |
| **Nuitka** | Compilado C — mais rápido | Linux, Win, macOS | Melhor performance |
| **cx_Freeze** | Pasta com executável | Linux, Win, macOS | Simples |
| **AppImage** | AppImage Linux | Linux | Sem instalação |

```bash
# PyInstaller — executável único
pip install pyinstaller

pyinstaller \
  --onefile \
  --windowed \
  --name "MyApp" \
  --icon "assets/icon.ico" \
  --add-data "assets:assets" \
  --add-data ".env.example:.env.example" \
  src/main.py

# Nuitka — compilação para C (melhor performance)
pip install nuitka
python -m nuitka \
  --standalone \
  --onefile \
  --enable-plugin=pyside6 \
  --output-dir=dist \
  src/main.py
```

## 3. CI/CD — GitHub Actions

```yaml
name: CI/CD Python/PySide6

on: [push, pull_request]

jobs:
  quality:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: '3.11'
          cache: 'pip'

      - run: pip install -e ".[dev]"

      - name: ruff lint
        run: ruff check src/ tests/

      - name: mypy
        run: mypy src/

      - name: bandit SAST
        run: bandit -r src/ -ll -ii

      - name: safety CVE check
        run: safety check -r requirements.txt

      - name: pytest + cobertura
        run: |
          pytest tests/ \
            --cov=src \
            --cov-report=xml \
            --cov-fail-under=60
        env:
          QT_QPA_PLATFORM: offscreen   # headless Qt

      - name: Upload cobertura
        uses: codecov/codecov-action@v4

  build:
    runs-on: ubuntu-latest
    needs: quality
    if: startsWith(github.ref, 'refs/tags/v')
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: '3.11'

      - run: pip install pyinstaller PySide6
      - run: |
          pyinstaller --onefile --windowed \
            --name "MyApp" src/main.py

      - name: Publicar release
        uses: softprops/action-gh-release@v2
        with:
          files: dist/MyApp
```

## 4. Prompt de Auditoria DevOps

```
Faça uma auditoria de DevOps e build deste projeto Python/PySide6.
Reporte em português.

- pyproject.toml como fonte de dependências?
- Dependências com versão fixada?
- Ambiente virtual configurado?
- CI/CD automatizado com GitHub Actions?
- ruff + mypy + bandit + safety no CI?
- pytest com gate de cobertura no CI?
- QT_QPA_PLATFORM=offscreen para testes headless?
- PyInstaller/Nuitka configurado?
- Testado em máquina limpa sem Python instalado?
- Versionamento SemVer com tags Git?
```

---

---

# 🧹 Auditoria de Qualidade de Código & Dívida Técnica

## Stack: Python · PySide6 · MySQL

---

## 1. Code Smells Python/PySide6

| # | Sintoma | Limiar | Prioridade | Ferramenta |
|---|---------|--------|-----------|------------|
| ☐ | Módulo com mais de 300 linhas | > 300 | 🔴 CRÍTICO | wc -l / radon |
| ☐ | Função/método com mais de 30 linhas | > 30 | 🔴 CRÍTICO | radon |
| ☐ | Classe com mais de 10 métodos públicos | > 10 | 🟠 IMPORTANTE | radon |
| ☐ | Slot com lógica de negócio > 10 linhas | > 10 | 🔴 CRÍTICO | Revisão manual |
| ☐ | Função com mais de 5 parâmetros | > 5 | 🟠 IMPORTANTE | ruff PLR0913 |
| ☐ | Nível de indentação acima de 4 | > 4 | 🟠 IMPORTANTE | ruff C901 |

## 2. Complexidade Ciclomática

| Complexidade | Classificação | Ação |
|-------------|--------------|------|
| 1–5 | ✅ Simples | Nenhuma |
| 6–10 | ⚠️ Moderado | Monitorar |
| 11–20 | 🟠 Alto | Refatorar |
| > 20 | 🔴 Crítico | Urgente |

```bash
# radon — análise de complexidade Python
pip install radon

# Complexidade ciclomática
radon cc src/ -a -nb     # mostra apenas B, C, D, E, F
radon cc src/ --min C    # falha se alguma função é C ou pior
radon cc src/ --json > complexity.json

# Índice de manutenibilidade
radon mi src/ -s          # mostra score por arquivo
```

## 3. Nomenclatura Python/PySide6

| Entidade | Convenção | Exemplo |
|----------|-----------|---------|
| Classes | PascalCase | `UserRepository`, `MainWindow` |
| Funções/métodos | snake_case | `find_by_id()`, `set_name()` |
| Variáveis privadas | `_` prefix | `self._repository` |
| Constantes de módulo | UPPER_SNAKE | `MAX_RETRIES`, `DEFAULT_TIMEOUT` |
| Signals | snake_case passado | `user_created`, `data_changed` |
| Slots | `on_` ou `_handle_` | `on_save_clicked`, `_handle_error` |
| Arquivos | snake_case | `user_repository.py` |
| Pacotes | snake_case | `domain/value_objects/` |
| Enums | PascalCase + StrEnum/IntEnum | `class Status(StrEnum)` |

## 4. Dead Code

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Funções/métodos nunca chamados | 🟠 IMPORTANTE | vulture |
| ☐ | Variáveis declaradas e nunca usadas | 🟠 IMPORTANTE | ruff F841 |
| ☐ | Imports não utilizados | 🟠 IMPORTANTE | ruff F401 |
| ☐ | Código comentado em bloco > 10 linhas | 🟢 COSMÉTICO | Grep |
| ☐ | `TODO`/`FIXME`/`HACK` catalogados | 🟢 COSMÉTICO | grep -rn |

```bash
# vulture — detecta código morto Python
pip install vulture
vulture src/ --min-confidence 80
```

## 5. Prompt de Auditoria Qualidade

```
Faça uma auditoria de qualidade de código Python/PySide6.
Reporte em português.

- Identifique os 5 maiores módulos (God Modules)
- Identifique os 5 métodos de maior complexidade ciclomática
- Há slots com lógica de negócio?
- Há dicts crus entre camadas em vez de dataclasses?
- Há Magic Strings ou Magic Numbers?
- Convenção snake_case aplicada consistentemente?
- Há imports não utilizados?
- Há código comentado em blocos grandes?
- Quantos TODO/FIXME existem?
- Estime o índice de manutenibilidade (radon MI)
```

---

---

# 📋 Índice Mestre de Auditorias
## Stack: Python · PySide6/PyQt6 · MySQL

---

## Documentos de Auditoria

| # | Documento | Foco | Prioridade |
|---|-----------|------|-----------|
| 1 | `py_qt6_auditoria_seguranca.md` | bandit, SQL Injection, secrets, bcrypt, LGPD | 🔴 Primeiro |
| 2 | `py_qt6_auditoria_arquitetura.md` | 4 camadas, SOLID, DRY, TDD, pytest, ABCs | 🔴 Segundo |
| 3 | `py_qt6_auditoria_python_moderno.md` | PEP 8, type hints, mypy, ruff, enums, async | 🔴 Terceiro |
| 4 | `py_qt6_auditoria_completo.md` (este) | Qt6, MySQL, Performance, DevOps, Qualidade | 🟠 Quarto |

---

## Prompt Mestre — Auditoria Completa para Claude Opus

```
Faça uma auditoria completa e abrangente deste projeto Python/PySide6/MySQL.
Reporte TUDO em português. Seja preciso e técnico.

═══════════════════════════════════════
BLOCO 1 — SEGURANÇA (primeiro)
═══════════════════════════════════════
- Há eval()/exec() com input externo?
- Há SQL sem prepare()/bindValue()?
- Há f-string ou concatenação em SQL?
- Credenciais hardcoded no código?
- Senhas com MD5/SHA1 em vez de bcrypt/argon2?
- pickle.loads() de fontes externas?
- yaml.load() sem SafeLoader?

═══════════════════════════════════════
BLOCO 2 — ARQUITETURA
═══════════════════════════════════════
- Há QSqlQuery dentro de QWidget?
- Slots implementam lógica de negócio?
- Services dependem de ABCs ou concretos (DIP)?
- Domain layer tem dependência de PySide6?
- Dataclasses ou Pydantic para entidades?
- 3 piores violações de SRP
- 3 piores violações de DRY

═══════════════════════════════════════
BLOCO 3 — PYTHON MODERNO
═══════════════════════════════════════
- ruff roda sem erros?
- mypy roda sem erros com strict=true?
- Há bare except ou except: pass?
- Há mutable default arguments?
- Dict/List de typing em vez de nativos?
- Strings mágicas em vez de StrEnum?
- Dicts crus entre camadas?

═══════════════════════════════════════
BLOCO 4 — QT6/PYSIDE6
═══════════════════════════════════════
- Há setFixedSize() ou coordenadas absolutas?
- QAbstractItemModel para listas e tabelas?
- beginInsertRows/endInsertRows emitidos?
- data() do model sem query?
- Signals com QueuedConnection cross-thread?
- Strings em tr()?
- QLocale pt_BR configurado?
- QApplication.processEvents() como gambiarra?

═══════════════════════════════════════
BLOCO 5 — BANCO DE DADOS
═══════════════════════════════════════
- Charset utf8mb4 em todas as tabelas?
- DECIMAL para dinheiro (não FLOAT)?
- N+1 queries (loop com query)?
- EXPLAIN em queries frequentes?
- Migrations versionadas?
- Backup automático?

═══════════════════════════════════════
BLOCO 6 — DEVOPS E BUILD
═══════════════════════════════════════
- pyproject.toml como fonte de dependências?
- CI/CD com GitHub Actions?
- QT_QPA_PLATFORM=offscreen para testes headless?
- PyInstaller/Nuitka configurado?
- safety + bandit no CI?
- Gate de cobertura ≥ 60%?

═══════════════════════════════════════
DIAGNÓSTICO FINAL
═══════════════════════════════════════
1. Todos os problemas por prioridade:
   CRÍTICO / IMPORTANTE / COSMÉTICO

2. Top 10 ações de maior impacto (por ROI)

3. Estimativa de esforço:
   - CRÍTICO: X horas/dias
   - IMPORTANTE: X horas/dias
   - COSMÉTICO: X horas/dias

4. Roadmap de sprints (2 semanas cada)
```

---

## Ferramentas por Domínio

| Domínio | Ferramenta | Instalação | Uso |
|---------|-----------|-----------|-----|
| Lint + estilo | ruff | `pip install ruff` | `ruff check src/` |
| Type checking | mypy | `pip install mypy` | `mypy src/` |
| SAST segurança | bandit | `pip install bandit` | `bandit -r src/` |
| CVEs deps | safety | `pip install safety` | `safety check` |
| Secrets no git | trufflehog | `pip install trufflehog` | `trufflehog git file://.` |
| Testes | pytest | `pip install pytest` | `pytest tests/` |
| Testes Qt | pytest-qt | `pip install pytest-qt` | automático com fixture `qtbot` |
| Cobertura | pytest-cov | `pip install pytest-cov` | `--cov=src --cov-fail-under=60` |
| Mocks | pytest-mock | `pip install pytest-mock` | fixture `mocker` |
| Complexidade | radon | `pip install radon` | `radon cc src/ --min C` |
| Dead code | vulture | `pip install vulture` | `vulture src/` |
| Profiling CPU | cProfile | built-in | `python -m cProfile -o out.prof src/main.py` |
| Profiling memória | memory_profiler | `pip install memory_profiler` | `@profile` decorator |
| Profiling produção | py-spy | `pip install py-spy` | `py-spy top --pid PID` |
| Empacotamento | PyInstaller | `pip install pyinstaller` | `pyinstaller --onefile src/main.py` |
| Empacotamento fast | Nuitka | `pip install nuitka` | `python -m nuitka --onefile src/main.py` |
| Migrations | Alembic | `pip install alembic` | `alembic upgrade head` |
| XML seguro | defusedxml | `pip install defusedxml` | `import defusedxml.ElementTree` |
| Criptografia | cryptography | `pip install cryptography` | `Fernet.generate_key()` |
| Hash seguro | bcrypt | `pip install bcrypt` | `bcrypt.hashpw(...)` |

---

## Roadmap Consolidado — 10 Sprints

| Sprint | Semana | Objetivo | Critério de done |
|--------|--------|----------|-----------------|
| 🔴 1 | 1-2 | SQL Injection zero | prepare()/bindValue() em 100%; grep sem f-string SQL |
| 🔴 2 | 3-4 | Secrets + senhas seguras | trufflehog sem hits; bcrypt em produção |
| 🔴 3 | 5-6 | Separar camadas | Zero QSqlQuery em QWidget; Services com ABCs |
| 🟠 4 | 7-8 | ruff + mypy zero erros | CI verde com strict mypy |
| 🟠 5 | 9-10 | TDD nos Services | Cobertura ≥ 60% com mocks |
| 🟠 6 | 11-12 | CI/CD completo | bandit + safety + pytest headless no pipeline |
| 🟠 7 | 13-14 | Qt6 correto | QAbstractItemModel; signals sem duplicação; QLocale pt_BR |
| 🟢 8 | 15-16 | Python moderno | Dataclasses; StrEnum; X|None; match/case |
| 🟢 9 | 17-18 | Empacotamento | PyInstaller testado em máquina limpa |
| 🟢 10 | 19-20 | Observabilidade | Logging JSON; crash handler; métricas de memória |

---

*Índice mestre — Claude Opus — uso interno confidencial*

---

## II2 — Arquitetura Python — 4 Camadas, SOLID, DRY, TDD

# 🏛️ Auditoria de Arquitetura — 4 Camadas + SOLID + DRY + TDD
## Stack: Python · PySide6/PyQt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Python / PySide6 / MySQL |

---

## 1. Arquitetura em 4 Camadas

```
┌─────────────────────────────────────────────────────────┐
│  LAYER 1 — PRESENTATION   PySide6 Widgets / QML         │
│  Views, ViewModels, Delegates, UI state                  │
├─────────────────────────────────────────────────────────┤
│  LAYER 2 — APPLICATION    Services / Use Cases           │
│  Regras de negócio, orquestração, DTOs                   │
├─────────────────────────────────────────────────────────┤
│  LAYER 3 — DOMAIN         Entities / Repository ABCs    │
│  Modelos de domínio, interfaces, value objects           │
├─────────────────────────────────────────────────────────┤
│  LAYER 4 — INFRASTRUCTURE MySQL / QSql / SQLAlchemy      │
│  Implementações concretas, I/O, migrations               │
└─────────────────────────────────────────────────────────┘
```

### Regra de dependência obrigatória

```
PRESENTATION → APPLICATION (via interfaces / signals)
APPLICATION  → DOMAIN      (via ABCs / Protocols)
DOMAIN       → nada        (zero dependências externas)
INFRASTRUCTURE → DOMAIN    (implementa ABCs do domain)
```

> ⚠️ **Violação típica de IA fraca:** `QSqlQuery` ou `pymysql` diretamente dentro de slots de `QWidget` — as 4 camadas colapsadas em um único arquivo `.py`.

---

## 2. Checklist por Camada

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

### 2.1 Layer 1 — Presentation (PySide6 Widgets)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Widgets não contêm lógica de negócio — apenas UI e input | 🔴 CRÍTICO | SRP |
| ☐ | Sem acesso a banco de dados dentro de qualquer `QWidget` | 🔴 CRÍTICO | Camadas |
| ☐ | ViewModels/Presenters separados das Views | 🔴 CRÍTICO | SRP / MVP |
| ☐ | Slots de UI delegam para Services — não implementam lógica | 🔴 CRÍTICO | SRP |
| ☐ | `QAbstractItemModel` subclassificado para listas e tabelas | 🟠 IMPORTANTE | SRP / OCP |
| ☐ | Sem `QApplication.processEvents()` em loops de negócio | 🔴 CRÍTICO | Performance |

**Sintaxe correta de connect em PySide6:**

```python
# ✅ CORRETO — connect com método bound
self.btn_save.clicked.connect(self._on_save_clicked)

# ✅ CORRETO — lambda com slot explícito
self.btn_delete.clicked.connect(
    lambda: self._on_delete_clicked(item_id)
)

# ❌ ERRADO — lógica de negócio diretamente no slot
def _on_save_clicked(self):
    # Isso não deveria estar aqui:
    query = QSqlQuery()
    query.prepare("INSERT INTO users ...")
    query.exec()
```

### 2.2 Layer 2 — Application (Services / Use Cases)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Classes de Service com responsabilidade única | 🔴 CRÍTICO | SRP |
| ☐ | Services dependem de ABCs/Protocols de Repository | 🔴 CRÍTICO | DIP |
| ☐ | Services injetados via construtor — sem instanciação interna | 🔴 CRÍTICO | DIP |
| ☐ | Services sem referência a widgets ou UI | 🔴 CRÍTICO | SRP / Camadas |
| ☐ | Dataclasses ou Pydantic para DTOs | 🟠 IMPORTANTE | SRP |
| ☐ | Operações longas em `QThread` ou `QRunnable` | 🔴 CRÍTICO | Performance |

### 2.3 Layer 3 — Domain (Entities / ABCs)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | ABCs (`abc.ABC`) ou `Protocol` para todos os Repositories | 🔴 CRÍTICO | DIP / ISP |
| ☐ | Entidades de domínio sem dependência de PySide6 ou MySQL | 🟠 IMPORTANTE | SRP |
| ☐ | Value Objects para conceitos do domínio (Email, CPF, Money) | 🟠 IMPORTANTE | DRY |
| ☐ | Dataclasses ou Pydantic para entidades — não dicts crus | 🔴 CRÍTICO | SRP |
| ☐ | Domínio testável sem banco ou UI | 🔴 CRÍTICO | TDD |

### 2.4 Layer 4 — Infrastructure (MySQL / QSql)

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Implementações de Repository isoladas na camada de infraestrutura | 🔴 CRÍTICO | DIP |
| ☐ | Toda query SQL dentro dos Repositories | 🔴 CRÍTICO | SRP |
| ☐ | `QSqlQuery.prepare()` + `bindValue()` em 100% das queries | 🔴 CRÍTICO | Segurança |
| ☐ | Transações explícitas para operações compostas | 🟠 IMPORTANTE | Consistência |
| ☐ | Erros de query verificados e propagados com exceções próprias | 🔴 CRÍTICO | Robustez |

**Estrutura de Repository correta em Python:**

```python
# domain/interfaces/i_user_repository.py — Layer 3
from abc import ABC, abstractmethod
from typing import Optional
from domain.entities.user import User

class IUserRepository(ABC):
    @abstractmethod
    def find_by_id(self, user_id: int) -> Optional[User]: ...

    @abstractmethod
    def find_all(self) -> list[User]: ...

    @abstractmethod
    def save(self, user: User) -> User: ...

    @abstractmethod
    def delete(self, user_id: int) -> None: ...


# infrastructure/repositories/mysql_user_repository.py — Layer 4
from PySide6.QtSql import QSqlQuery
from domain.interfaces.i_user_repository import IUserRepository
from domain.entities.user import User

class MySqlUserRepository(IUserRepository):
    def __init__(self, db):
        self._db = db

    def find_by_id(self, user_id: int) -> Optional[User]:
        query = QSqlQuery(self._db)
        query.prepare("SELECT id, name, email FROM users WHERE id = :id")
        query.bindValue(":id", user_id)

        if not query.exec():
            raise RepositoryError(query.lastError().text())

        if not query.next():
            return None

        return self._map_row(query)

    def _map_row(self, query: QSqlQuery) -> User:
        return User(
            id=query.value("id"),
            name=query.value("name"),
            email=query.value("email"),
        )
```

---

## 3. SOLID — Checklist Detalhado

### 3.1 S — Single Responsibility Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `MainWindow` com 1000+ linhas fazendo tudo | Extrair em ViewModel + Services | Presentation |
| ☐ | Slot que salva no banco, valida e atualiza UI | Separar em Service + Repository + slot | Application |
| ☐ | Módulo `.py` com mais de 300 linhas | Decompor em classes menores | Todas |
| ☐ | Função que faz mais de uma coisa | Decompor | Todas |
| ☐ | Classe que busca dados E renderiza tabela | `QAbstractItemModel` + Repository | Pres / Infra |

**Teste rápido de SRP:** consegue descrever a classe em uma frase sem usar "e"?

### 3.2 O — Open/Closed Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `if tipo == "pdf": ... elif tipo == "csv":` em exportação | ABC `IExporter` + implementações | Application |
| ☐ | Novo relatório exige editar classe existente | Herança ou composição | Application |
| ☐ | `if` crescente para tipos de desconto | `DiscountRule` ABC + Strategy | Application |

### 3.3 L — Liskov Substitution Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | Subclasse que lança `NotImplementedError` em método que deveria funcionar | Segregar ABC (ISP) | Domain |
| ☐ | `MySqlUserRepository` não implementa todos os métodos de `IUserRepository` | Implementar completamente | Infra |
| ☐ | Override que restringe o contrato da classe pai | Revisar hierarquia | Todas |

### 3.4 I — Interface Segregation Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | ABC com 20+ métodos abstratos | Quebrar em `IReadable` + `IWritable` | Domain |
| ☐ | Classe forçada a implementar métodos com `pass` | Segregar ABC | Domain |
| ☐ | `Protocol` com métodos irrelevantes para o consumidor | Criar Protocol específico | Application |

### 3.5 D — Dependency Inversion Principle

| # | Anti-padrão | Correção | Camada |
|---|-------------|---------|--------|
| ☐ | `repo = MySqlUserRepository()` dentro de Service | Injetar via `__init__` | Application |
| ☐ | `MainWindow` instanciando Services diretamente | DI Container ou factory | Presentation |
| ☐ | Type hints em classes concretas em vez de ABCs | `IUserRepository` não `MySqlUserRepository` | Todas |

**Exemplo correto de DIP em Python:**

```python
# ✅ CORRETO — depende de abstração
class UserService:
    def __init__(
        self,
        repository: IUserRepository,   # ABC — não concreto
        mailer: IMailer,               # ABC — não concreto
        logger: logging.Logger,
    ) -> None:
        self._repository = repository
        self._mailer = mailer
        self._logger = logger

# ❌ ERRADO — depende de implementação concreta
class UserService:
    def __init__(self) -> None:
        self._repository = MySqlUserRepository()  # concreto!
        self._mailer = SMTPMailer()               # concreto!
```

---

## 4. DRY — Don't Repeat Yourself

### 4.1 DRY por camada

| Camada | Duplicação comum | Solução |
|--------|-----------------|---------|
| Presentation | Mesmo `QMessageBox.critical()` em 20 slots | Método `_show_error(msg)` na classe base |
| Presentation | Mesma configuração de `QTableView` repetida | Helper `configure_table(view, model)` |
| Application | Mesma validação de e-mail em 3 Services | Value Object `Email` ou `validators.py` |
| Application | Lógica de paginação repetida | Classe `Paginator[T]` genérica |
| Infrastructure | Mapeamento query → dataclass repetido | Método `_map_row()` no Repository base |
| Infrastructure | Abertura de conexão repetida | `DatabaseConnection` centralizado |
| Domain | Mesmas invariantes em múltiplas entidades | Classe base `Entity` ou mixin |

### 4.2 DRY com Dataclasses e Pydantic

```python
# ✅ Entidade com dataclass — sem repetição de boilerplate
from dataclasses import dataclass, field
from datetime import datetime

@dataclass
class User:
    name: str
    email: str
    id: int = field(default=0)
    created_at: datetime = field(default_factory=datetime.now)
    is_active: bool = True

    def __post_init__(self):
        if not self.name.strip():
            raise ValueError("Nome não pode ser vazio")
        if "@" not in self.email:
            raise ValueError("E-mail inválido")

# ✅ Com Pydantic — validação automática e serialização
from pydantic import BaseModel, EmailStr, validator

class UserDTO(BaseModel):
    name: str
    email: EmailStr
    is_active: bool = True

    @validator("name")
    def name_must_not_be_empty(cls, v):
        if not v.strip():
            raise ValueError("Nome não pode ser vazio")
        return v.strip()
```

---

## 5. TDD — Red / Green / Refactor

### 5.1 O ciclo

```
🔴 RED      Escreva o teste antes. Ele falha — correto.
🟢 GREEN    Mínimo de código para passar. Sem otimização.
🔵 REFACTOR Melhore sem quebrar. Aqui entram SOLID e DRY.
```

### 5.2 Frameworks de teste

| Framework | Uso | Instalação |
|-----------|-----|-----------|
| **pytest** | Testes unitários e integração | `pip install pytest` |
| **pytest-qt** | Testes de UI PySide6/PyQt6 | `pip install pytest-qt` |
| **unittest.mock** | Mocks e stubs — built-in | Incluso no Python |
| **pytest-mock** | Mock integrado ao pytest | `pip install pytest-mock` |
| **pytest-cov** | Cobertura de código | `pip install pytest-cov` |

### 5.3 Estrutura de diretórios

```
tests/
├── unit/
│   ├── services/
│   │   ├── test_user_service.py
│   │   └── test_order_service.py
│   ├── domain/
│   │   ├── test_user.py
│   │   └── test_email_value_object.py
│   └── conftest.py
├── integration/
│   ├── repositories/
│   │   └── test_mysql_user_repository.py
│   └── conftest.py        ← fixture de banco de teste
└── ui/
    ├── test_main_window.py ← pytest-qt
    └── conftest.py
```

### 5.4 Checklists de TDD

#### Testes Unitários — Services

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Cada método público de Service tem teste unitário | 🔴 CRÍTICO |
| ☐ | Testes escritos ANTES da implementação (Red primeiro) | 🔴 CRÍTICO |
| ☐ | Dependências substituídas por `MagicMock` ou `pytest-mock` | 🔴 CRÍTICO |
| ☐ | Caminho feliz E casos de erro cobertos | 🔴 CRÍTICO |
| ☐ | Nomes descritivos: `test_register_raises_when_email_exists` | 🟠 IMPORTANTE |
| ☐ | Um único `assert` por comportamento testado | 🟠 IMPORTANTE |
| ☐ | Testes independentes — sem estado compartilhado | 🔴 CRÍTICO |
| ☐ | Sem lógica condicional dentro dos testes | 🟠 IMPORTANTE |

#### Testes de Integração — Repositories

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Repositories testados contra MySQL de teste ou SQLite in-memory | 🔴 CRÍTICO |
| ☐ | Schema criado e destruído por fixture pytest | 🔴 CRÍTICO |
| ☐ | CRUD completo coberto | 🔴 CRÍTICO |
| ☐ | Transações testadas (rollback em falha) | 🟠 IMPORTANTE |

#### Testes de UI — PySide6 (pytest-qt)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Signals verificados com `qtbot.waitSignal()` | 🔴 CRÍTICO |
| ☐ | Clicks simulados com `qtbot.mouseClick()` | 🟠 IMPORTANTE |
| ☐ | `QAbstractItemModel` testado com dados mockados | 🟠 IMPORTANTE |
| ☐ | Testes de UI não dependem de banco real | 🔴 CRÍTICO |

### 5.5 Exemplo completo — ciclo TDD Python

```python
# 🔴 RED — test_user_service.py (escreva ANTES de UserService)
import pytest
from unittest.mock import MagicMock, create_autospec
from application.services.user_service import UserService
from domain.interfaces.i_user_repository import IUserRepository
from domain.entities.user import User

@pytest.fixture
def mock_repo():
    return create_autospec(IUserRepository)

@pytest.fixture
def user_service(mock_repo):
    return UserService(repository=mock_repo)


def test_register_raises_when_email_already_exists(user_service, mock_repo):
    # Arrange
    mock_repo.find_by_email.return_value = User(
        name="Existente", email="used@email.com"
    )

    # Act + Assert
    with pytest.raises(EmailAlreadyExistsError):
        user_service.register("Novo", "used@email.com", "Senha123!")

    mock_repo.find_by_email.assert_called_once_with("used@email.com")
    mock_repo.save.assert_not_called()


def test_register_saves_user_when_email_is_new(user_service, mock_repo):
    # Arrange
    mock_repo.find_by_email.return_value = None
    expected = User(name="João", email="joao@email.com")
    mock_repo.save.return_value = expected

    # Act
    result = user_service.register("João", "joao@email.com", "Senha123!")

    # Assert
    assert result.email == "joao@email.com"
    mock_repo.save.assert_called_once()


# 🟢 GREEN — user_service.py — mínimo para passar
class UserService:
    def __init__(self, repository: IUserRepository) -> None:
        self._repository = repository

    def register(self, name: str, email: str, password: str) -> User:
        if self._repository.find_by_email(email):
            raise EmailAlreadyExistsError(email)
        user = User(name=name, email=email)
        return self._repository.save(user)

# 🔵 REFACTOR — extrair validação de senha para PasswordValidator
# Extrair hash para PasswordHasher, manter testes verdes
```

### 5.6 Métricas de Cobertura

| # | Item | Meta | Prioridade |
|---|------|------|-----------|
| ☐ | Cobertura de linhas nos Services | ≥ 80% | 🔴 CRÍTICO |
| ☐ | Cobertura de branches nos Services | ≥ 70% | 🟠 IMPORTANTE |
| ☐ | Cobertura nas entidades de Domain | ≥ 90% | 🟠 IMPORTANTE |
| ☐ | Gate de cobertura no CI | ≥ 60% total | 🔴 CRÍTICO |

```bash
# Rodar testes com cobertura
pytest tests/ --cov=src --cov-report=html --cov-report=term-missing

# Falhar se cobertura < 60%
pytest tests/ --cov=src --cov-fail-under=60
```

---

## 6. Estrutura de Diretórios Recomendada

```
projeto/
├── pyproject.toml               ← dependências e configurações
├── .env.example
│
├── src/
│   ├── presentation/            ← LAYER 1
│   │   ├── views/               ← QWidget subclasses
│   │   ├── viewmodels/          ← ViewModels / Presenters
│   │   ├── delegates/           ← QAbstractItemDelegate
│   │   └── models/              ← QAbstractItemModel subclasses
│   │
│   ├── application/             ← LAYER 2
│   │   ├── services/            ← Use cases
│   │   ├── dto/                 ← Dataclasses / Pydantic models
│   │   └── exceptions.py        ← Exceções de negócio
│   │
│   ├── domain/                  ← LAYER 3
│   │   ├── entities/            ← User, Order, Product
│   │   ├── interfaces/          ← IUserRepository (ABCs)
│   │   └── value_objects/       ← Email, Money, CPF
│   │
│   └── infrastructure/          ← LAYER 4
│       ├── repositories/        ← MySqlUserRepository
│       ├── database/            ← connection, migrations
│       └── config.py            ← leitura de .env
│
└── tests/
    ├── unit/
    ├── integration/
    └── ui/
```

---

## 7. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria completa de arquitetura em 4 camadas deste projeto
Python/PySide6/MySQL. Reporte em português.

## LAYER 1 — PRESENTATION
- Há QSqlQuery ou pymysql dentro de QWidget?
- Slots de UI implementam lógica de negócio?
- ViewModels separados das Views?
- QAbstractItemModel usado para listas e tabelas?

## LAYER 2 — APPLICATION
- Services têm responsabilidade única (SRP)?
- Services dependem de ABCs ou de implementações concretas (DIP)?
- Operações longas em QThread/QRunnable?
- Services têm referência a widgets?

## LAYER 3 — DOMAIN
- ABCs ou Protocols definidos para Repositories?
- Entidades de domínio livres de PySide6 e MySQL?
- Dataclasses ou Pydantic para entidades?
- Value Objects implementados?

## LAYER 4 — INFRASTRUCTURE
- Toda query SQL nos Repositories?
- QSqlQuery.prepare() em 100% das queries?
- Transações explícitas para operações compostas?

## TDD
- Há testes pytest para Services com mocks?
- pytest-qt usado para testar signals?
- Qual é a cobertura estimada?

## SOLID e DRY
- 3 piores violações de SRP
- 3 piores violações de DRY
- DIP violado (instanciação interna de concretions)?

## DIAGNÓSTICO FINAL
- Mapa de dependências entre camadas
- 5 ações de maior impacto arquitetural
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 8. Roadmap de Refatoração

| Sprint | Camada | Objetivo | Critério |
|--------|--------|----------|----------|
| 🔴 1 | Infra | Mover SQL para Repositories | Zero QSqlQuery fora de Repositories |
| 🔴 2 | Application | Criar Services com ABCs | Services sem referência a QWidget |
| 🔴 3 | Presentation | Separar ViewModels | Slots delegam para Services |
| 🔴 4 | Domain | ABCs e Value Objects | Services dependem de interfaces |
| 🟠 5 | Application | TDD nos Services | Cobertura ≥ 80% com mock |
| 🟠 6 | Infra | TDD nos Repositories | Testes com banco de teste |
| 🟠 7 | Presentation | TDD na UI | pytest-qt em signals críticos |
| 🟢 8 | Todas | DRY e Value Objects | ruff + mypy zero erros |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## II3 — Segurança Python — SQL Injection, Senhas, LGPD

# 🔒 Auditoria de Segurança & Hardening
## Stack: Python · PySide6/PyQt6 · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Python / PySide6 / MySQL / Infraestrutura |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  PYTHON         Injeção, eval, deserialização, secrets   │
├─────────────────────────────────────────────────────────┤
│  PYSIDE6/PYQT6  QSql, rede, plugins, slots              │
├─────────────────────────────────────────────────────────┤
│  MYSQL          Prepared statements, permissões, TLS     │
├─────────────────────────────────────────────────────────┤
│  DEPENDÊNCIAS   Supply chain, CVEs, secrets no código    │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Python — Segurança de Código

### 2.1 Injeção e Execução de Código

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem `eval()` com input do usuário ou dados externos | 🔴 CRÍTICO | bandit |
| ☐ | Sem `exec()` com strings não confiáveis | 🔴 CRÍTICO | bandit |
| ☐ | Sem `os.system()` com input do usuário | 🔴 CRÍTICO | bandit |
| ☐ | Sem `subprocess` com `shell=True` e input não sanitizado | 🔴 CRÍTICO | bandit |
| ☐ | Sem `pickle.loads()` de fontes externas não confiáveis | 🔴 CRÍTICO | bandit |
| ☐ | Sem `yaml.load()` sem `Loader=yaml.SafeLoader` | 🔴 CRÍTICO | bandit |
| ☐ | Sem `__import__()` com string de usuário | 🔴 CRÍTICO | bandit |

```python
# ❌ PERIGOSO — executa código arbitrário
eval(user_input)
exec(user_input)
os.system(f"grep {user_input} /etc/passwd")
subprocess.run(user_input, shell=True)
pickle.loads(external_data)
yaml.load(data)  # sem SafeLoader

# ✅ CORRETO
subprocess.run(["grep", sanitized_term, "/var/log/app.log"])
yaml.safe_load(data)
result = ast.literal_eval(safe_string)  # apenas literais Python
```

### 2.2 Secrets e Credenciais

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem credenciais hardcoded no código | 🔴 CRÍTICO | bandit / trufflehog |
| ☐ | Variáveis de ambiente via `python-dotenv` ou `os.environ` | 🔴 CRÍTICO | Revisão manual |
| ☐ | Arquivo `.env` no `.gitignore` | 🔴 CRÍTICO | git |
| ☐ | `.env.example` versionado sem valores | 🔴 CRÍTICO | git |
| ☐ | Secrets nunca logados | 🔴 CRÍTICO | Revisão manual |
| ☐ | `SECRET_KEY` gerada com `secrets.token_hex(32)` | 🔴 CRÍTICO | Revisão manual |

```python
# ❌ NUNCA
DB_PASSWORD = "senha123"
API_KEY = "sk-abc123xyz"

# ✅ CORRETO
import os
from dotenv import load_dotenv

load_dotenv()
DB_PASSWORD = os.environ["DB_PASSWORD"]  # lança KeyError se ausente
API_KEY = os.environ.get("API_KEY")      # None se ausente

# Gerar secret seguro
import secrets
SECRET_KEY = secrets.token_hex(32)
```

### 2.3 Senhas e Hashing

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Senhas armazenadas com `bcrypt` ou `argon2-cffi` — nunca MD5/SHA1 | 🔴 CRÍTICO | bandit |
| ☐ | Sem `hashlib.md5(password)` ou `hashlib.sha1(password)` | 🔴 CRÍTICO | bandit |
| ☐ | Comparação de hash com `secrets.compare_digest()` — evita timing attack | 🔴 CRÍTICO | Revisão manual |

```python
# ✅ Hash seguro de senha com bcrypt
import bcrypt

def hash_password(password: str) -> str:
    return bcrypt.hashpw(
        password.encode('utf-8'),
        bcrypt.gensalt(rounds=12)
    ).decode('utf-8')

def verify_password(password: str, hashed: str) -> bool:
    return bcrypt.checkpw(
        password.encode('utf-8'),
        hashed.encode('utf-8')
    )
```

### 2.4 Deserialização e Parsing

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | JSON parseado com `json.loads()` — nunca `eval()` | 🔴 CRÍTICO | bandit |
| ☐ | XML parseado com `defusedxml` — evita XXE | 🔴 CRÍTICO | bandit |
| ☐ | Arquivos de configuração validados com `pydantic` ou `cerberus` | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Upload de arquivos com validação de tipo MIME e tamanho | 🔴 CRÍTICO | Revisão manual |

```python
# ❌ XXE vulnerability
import xml.etree.ElementTree as ET
tree = ET.parse(user_file)  # vulnerável a XXE

# ✅ Seguro
import defusedxml.ElementTree as ET
tree = ET.parse(user_file)  # XXE desabilitado
```

---

## 3. PySide6/PyQt6 — Segurança Específica

### 3.1 QSql — SQL Injection

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Toda query SQL via `QSqlQuery.prepare()` + `bindValue()` | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sem f-string ou concatenação em queries SQL | 🔴 CRÍTICO | grep / bandit |
| ☐ | Credenciais de banco não hardcoded | 🔴 CRÍTICO | bandit |
| ☐ | Conexão MySQL com SSL/TLS habilitado | 🔴 CRÍTICO | Configuração |

```python
# ❌ SQL Injection
email = line_edit.text()
query = QSqlQuery(f"SELECT * FROM users WHERE email = '{email}'")

# ✅ CORRETO — prepared statement
query = QSqlQuery()
query.prepare("SELECT id, name FROM users WHERE email = :email")
query.bindValue(":email", line_edit.text())
query.exec()
```

### 3.2 Rede e QNetworkAccessManager

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Verificação de certificado SSL nunca desabilitada | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sem `QSslConfiguration` com `setPeerVerifyMode(QSslSocket.VerifyNone)` | 🔴 CRÍTICO | Revisão manual |
| ☐ | Timeout configurado em todas as requisições de rede | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Dados sensíveis não logados em requests/responses | 🔴 CRÍTICO | Revisão manual |

### 3.3 Armazenamento Seguro

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `QSettings` não usado para dados sensíveis (armazena plain text) | 🔴 CRÍTICO | Revisão manual |
| ☐ | Dados sensíveis em memória limpos após uso | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Arquivos temporários com dados sensíveis removidos com `os.unlink()` | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Criptografia via `cryptography` (Fernet) — nunca implementação própria | 🔴 CRÍTICO | Revisão manual |

```python
# ✅ Criptografia com cryptography (Fernet)
from cryptography.fernet import Fernet

class SecureStorage:
    def __init__(self, key: bytes):
        self._fernet = Fernet(key)

    def encrypt(self, data: str) -> bytes:
        return self._fernet.encrypt(data.encode())

    def decrypt(self, token: bytes) -> str:
        return self._fernet.decrypt(token).decode()

# Gerar e guardar chave de forma segura
key = Fernet.generate_key()
```

---

## 4. MySQL — Segurança

### 4.1 Acesso e Permissões

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Usuário de aplicação com permissões mínimas | 🔴 CRÍTICO | MySQL audit |
| ☐ | Sem uso de root pela aplicação | 🔴 CRÍTICO | Configuração |
| ☐ | Acesso apenas de hosts específicos — sem `%` | 🔴 CRÍTICO | MySQL audit |
| ☐ | Conexão com SSL/TLS obrigatório | 🔴 CRÍTICO | Configuração |
| ☐ | Porta 3306 não exposta publicamente | 🔴 CRÍTICO | Firewall |

**Conexão MySQL com SSL via PyMySQL:**

```python
import pymysql
import os

connection = pymysql.connect(
    host=os.environ["DB_HOST"],
    user=os.environ["DB_USER"],
    password=os.environ["DB_PASSWORD"],
    database=os.environ["DB_NAME"],
    ssl={
        "ca": "/etc/ssl/mysql/ca-cert.pem",
        "cert": "/etc/ssl/mysql/client-cert.pem",
        "key": "/etc/ssl/mysql/client-key.pem",
    },
    ssl_verify_cert=True,  # NUNCA False em produção
    cursorclass=pymysql.cursors.DictCursor
)
```

**Conexão via QSql com TLS:**

```python
from PySide6.QtSql import QSqlDatabase

db = QSqlDatabase.addDatabase("QMYSQL")
db.setHostName(os.environ["DB_HOST"])
db.setDatabaseName(os.environ["DB_NAME"])
db.setUserName(os.environ["DB_USER"])
db.setPassword(os.environ["DB_PASSWORD"])
db.setConnectOptions(
    "SSL_CA=/etc/ssl/mysql/ca-cert.pem;"
    "SSL_CERT=/etc/ssl/mysql/client-cert.pem;"
    "SSL_KEY=/etc/ssl/mysql/client-key.pem;"
    "MYSQL_OPT_SSL_MODE=SSL_MODE_REQUIRED"
)
db.open()
```

---

## 5. Análise Estática de Segurança

### 5.1 Bandit — SAST para Python

```bash
# Instalação
pip install bandit

# Análise completa
bandit -r src/ -ll -ii  # low level, low confidence mínimo

# Relatório HTML
bandit -r src/ -f html -o bandit_report.html

# Ignorar falsos positivos específicos
bandit -r src/ --skip B101,B311  # B101=assert, B311=random
```

### 5.2 Safety — Dependências com CVE

```bash
pip install safety
safety check --full-report
safety check -r requirements.txt
```

### 5.3 GitHub Actions com segurança

```yaml
  security:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Bandit SAST
        run: |
          pip install bandit
          bandit -r src/ -ll -ii --exit-zero -f json \
            -o bandit-report.json
          # Falha se há issues HIGH severity
          bandit -r src/ -lll --exit-zero || exit 1

      - name: Safety — CVEs em dependências
        run: |
          pip install safety
          safety check -r requirements.txt

      - name: TruffleHog — secrets no git
        uses: trufflesecurity/trufflehog@main
        with:
          path: ./
          base: ${{ github.event.repository.default_branch }}
```

---

## 6. LGPD — Checklist Python/PySide6

| # | Item de Verificação | Artigo LGPD | Prioridade |
|---|---------------------|------------|-----------|
| ☐ | Dados pessoais não logados em arquivos de log | Art. 46 | 🔴 CRÍTICO |
| ☐ | Senhas e tokens nunca em logs | Art. 46 | 🔴 CRÍTICO |
| ☐ | Dados pessoais criptografados em repouso no MySQL | Art. 46 | 🔴 CRÍTICO |
| ☐ | Exportação de dados do titular implementada | Art. 18, V | 🟠 IMPORTANTE |
| ☐ | Exclusão completa de dados do titular implementada | Art. 18, VI | 🔴 CRÍTICO |
| ☐ | Consentimento registrado com timestamp | Art. 7, I | 🟠 IMPORTANTE |
| ☐ | Política de retenção definida e automatizada | Art. 15/16 | 🔴 CRÍTICO |
| ☐ | Plano de resposta a incidentes (72h para ANPD) | Art. 48 | 🔴 CRÍTICO |

---

## 7. Roadmap de Segurança

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | SQL Injection zero | QSqlQuery.prepare() em 100% das queries | grep sem f-string SQL |
| 🔴 2 | Secrets fora do código | python-dotenv + .env no .gitignore | trufflehog sem hits |
| 🔴 3 | Senhas seguras | bcrypt/argon2 em vez de MD5/SHA1 | bandit sem B303/B324 |
| 🔴 4 | Bandit no CI | SAST automático em todo PR | Zero HIGH severity |
| 🟠 5 | Safety no CI | CVEs em dependências verificados | Zero CVE crítico |
| 🟠 6 | TLS no MySQL | Conexão com SSL obrigatório | Wireshark confirma |
| 🟠 7 | Remover eval/exec | Substituir por alternativas seguras | bandit sem B307 |
| 🟢 8 | LGPD | Exportação e exclusão de dados | Checklist completo |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## II4 — Python Moderno — Type Hints, Async, Dataclasses

# 🐍 Auditoria de Python Moderno
## PEP 8 · Type Hints · Async · Padrões · Python 3.11+

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Python 3.11+ |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  MODERNIDADE     Python 3.10/11/12 features              │
├─────────────────────────────────────────────────────────┤
│  TYPE HINTS      Anotações, mypy, Pydantic               │
├─────────────────────────────────────────────────────────┤
│  QUALIDADE       PEP 8, docstrings, complexidade         │
├─────────────────────────────────────────────────────────┤
│  ASYNC           asyncio, QThread, QtConcurrent          │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Modernidade — Python 3.10/11/12

### 2.1 Sintaxe Moderna

| # | Anti-padrão legado | Substituição moderna | Versão | Prioridade |
|---|-------------------|---------------------|--------|-----------|
| ☐ | `if type(x) == int:` | `isinstance(x, int)` | 3.x | 🔴 CRÍTICO |
| ☐ | `dict.get(k, None)` | `dict.get(k)` (None é default) | 3.x | 🟢 COSMÉTICO |
| ☐ | `lambda x: x.name` em sort | `operator.attrgetter("name")` | 3.x | 🟢 COSMÉTICO |
| ☐ | `"Hello " + name` | `f"Hello {name}"` | 3.6 | 🟠 IMPORTANTE |
| ☐ | `%s` ou `.format()` | f-strings | 3.6 | 🟠 IMPORTANTE |
| ☐ | `if x is not None and x > 0:` em encadeamentos | Walrus `:=` onde adequado | 3.8 | 🟢 COSMÉTICO |
| ☐ | `Dict`, `List`, `Tuple` de `typing` | `dict`, `list`, `tuple` nativos | 3.9 | 🟠 IMPORTANTE |
| ☐ | `Optional[X]` | `X \| None` | 3.10 | 🟠 IMPORTANTE |
| ☐ | `Union[X, Y]` | `X \| Y` | 3.10 | 🟠 IMPORTANTE |
| ☐ | `if/elif` em cascata para tipos | `match/case` (structural pattern matching) | 3.10 | 🟠 IMPORTANTE |

```python
# ❌ Legado
from typing import Dict, List, Optional, Union

def process(
    users: List[Dict[str, str]],
    config: Optional[Dict] = None,
) -> Union[str, int]:
    if config is None:
        config = {}
    ...

# ✅ Python 3.10+
def process(
    users: list[dict[str, str]],
    config: dict | None = None,
) -> str | int:
    if config is None:
        config = {}
    ...

# ✅ match/case — Python 3.10
def handle_status(status: str) -> str:
    match status:
        case "active":   return "Usuário ativo"
        case "inactive": return "Usuário inativo"
        case "pending":  return "Aguardando aprovação"
        case _:          return "Status desconhecido"
```

### 2.2 Dataclasses e NamedTuples

| # | Anti-padrão | Substituição | Prioridade |
|---|-------------|-------------|-----------|
| ☐ | Dicts crus passados entre camadas como "objetos" | `@dataclass` ou Pydantic `BaseModel` | 🔴 CRÍTICO |
| ☐ | Tuplas sem nome para agrupar dados | `NamedTuple` ou `@dataclass` | 🟠 IMPORTANTE |
| ☐ | Classes com só `__init__` sem lógica | `@dataclass` | 🟠 IMPORTANTE |
| ☐ | `@dataclass` mutável onde deveria ser imutável | `@dataclass(frozen=True)` | 🟠 IMPORTANTE |

```python
# ❌ Dict cru — sem tipo, sem validação
user = {"id": 1, "name": "João", "email": "joao@email.com"}

# ✅ Dataclass tipada
from dataclasses import dataclass, field

@dataclass
class User:
    name: str
    email: str
    id: int = 0
    is_active: bool = True

    def __post_init__(self):
        if not self.name.strip():
            raise ValueError("Nome vazio")

# ✅ Imutável (Value Object)
@dataclass(frozen=True)
class Money:
    amount_cents: int
    currency: str = "BRL"

    def __add__(self, other: "Money") -> "Money":
        assert self.currency == other.currency
        return Money(self.amount_cents + other.amount_cents, self.currency)
```

### 2.3 Enums

| # | Anti-padrão | Substituição | Prioridade |
|---|-------------|-------------|-----------|
| ☐ | Constantes mágicas: `STATUS_ACTIVE = 1` | `class Status(IntEnum): Active = 1` | 🔴 CRÍTICO |
| ☐ | Strings mágicas: `"active"`, `"inactive"` | `class Status(StrEnum): Active = "active"` | 🔴 CRÍTICO |
| ☐ | `if status == "active":` espalhado em 20 lugares | `if status == Status.Active:` | 🔴 CRÍTICO |

```python
from enum import StrEnum, IntEnum, auto

class UserStatus(StrEnum):
    Active   = "active"
    Inactive = "inactive"
    Pending  = "pending"

class OrderStatus(IntEnum):
    Draft     = auto()
    Submitted = auto()
    Approved  = auto()
    Cancelled = auto()
```

---

## 3. Type Hints e Mypy

### 3.1 Checklist de Type Hints

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Todo método público com type hints em parâmetros e retorno | 🔴 CRÍTICO | mypy |
| ☐ | ABCs e Protocols anotados corretamente | 🔴 CRÍTICO | mypy |
| ☐ | Sem `Any` sem justificativa documentada | 🟠 IMPORTANTE | mypy |
| ☐ | `Optional[X]` / `X \| None` onde retorno pode ser None | 🔴 CRÍTICO | mypy |
| ☐ | `TypeVar` para funções/classes genéricas | 🟠 IMPORTANTE | mypy |
| ☐ | `TypedDict` para dicts com estrutura conhecida | 🟠 IMPORTANTE | mypy |
| ☐ | `Protocol` para duck typing sem herança | 🟠 IMPORTANTE | mypy |
| ☐ | `Final` para constantes imutáveis | 🟢 COSMÉTICO | mypy |
| ☐ | `ClassVar` para variáveis de classe | 🟢 COSMÉTICO | mypy |

```python
# ✅ Type hints completos — Python 3.11+
from typing import Protocol, TypeVar, Final
from collections.abc import Sequence

T = TypeVar("T")

# Constante imutável
MAX_RETRIES: Final = 3

# Protocol para duck typing (sem herança obrigatória)
class Saveable(Protocol):
    def save(self) -> bool: ...
    def validate(self) -> list[str]: ...

# Função genérica tipada
def first_or_none(items: Sequence[T]) -> T | None:
    return items[0] if items else None

# TypedDict para dicts estruturados
from typing import TypedDict

class UserDict(TypedDict):
    id: int
    name: str
    email: str
    is_active: bool
```

### 3.2 Configuração mypy

```ini
# mypy.ini ou pyproject.toml [tool.mypy]
[mypy]
python_version = 3.11
strict = true
warn_return_any = true
warn_unused_configs = true
disallow_untyped_defs = true
disallow_any_generics = true
check_untyped_defs = true
no_implicit_optional = true
warn_redundant_casts = true
warn_unused_ignores = true

# Ignorar stubs ausentes de bibliotecas Qt
[[tool.mypy.overrides]]
module = ["PySide6.*", "PyQt6.*"]
ignore_missing_imports = true
```

---

## 4. PEP 8 e Qualidade de Código

### 4.1 Estilo e Formatação

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Formatação automática com `black` ou `ruff format` | 🔴 CRÍTICO | ruff / black |
| ☐ | Imports organizados com `isort` ou `ruff --select I` | 🟠 IMPORTANTE | ruff |
| ☐ | Sem linhas com mais de 88–120 caracteres | 🟠 IMPORTANTE | ruff |
| ☐ | `ruff` sem erros na totalidade do projeto | 🔴 CRÍTICO | ruff |
| ☐ | Sem `import *` | 🔴 CRÍTICO | ruff F401 |
| ☐ | Sem imports não utilizados | 🟠 IMPORTANTE | ruff F401 |
| ☐ | Ordem de imports: stdlib → third-party → local | 🟠 IMPORTANTE | ruff I |

### 4.2 Docstrings

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Docstring em todas as classes públicas | 🟠 IMPORTANTE |
| ☐ | Docstring em todos os métodos públicos de Services e Repositories | 🟠 IMPORTANTE |
| ☐ | Formato consistente: Google style ou NumPy style | 🟢 COSMÉTICO |
| ☐ | `Args`, `Returns`, `Raises` documentados em métodos complexos | 🟢 COSMÉTICO |

```python
def register_user(self, name: str, email: str, password: str) -> User:
    """Registra um novo usuário no sistema.

    Args:
        name: Nome completo do usuário.
        email: Endereço de e-mail único.
        password: Senha em texto plano (será hasheada).

    Returns:
        User: Entidade do usuário criado com ID preenchido.

    Raises:
        EmailAlreadyExistsError: Se o e-mail já está cadastrado.
        WeakPasswordError: Se a senha não atende aos requisitos.
    """
```

### 4.3 Configuração ruff

```toml
# pyproject.toml
[tool.ruff]
target-version = "py311"
line-length = 88

[tool.ruff.lint]
select = [
    "E",    # pycodestyle errors
    "W",    # pycodestyle warnings
    "F",    # pyflakes
    "I",    # isort
    "B",    # flake8-bugbear
    "C4",   # flake8-comprehensions
    "UP",   # pyupgrade
    "S",    # flake8-bandit (segurança)
    "ANN",  # flake8-annotations (type hints)
    "N",    # pep8-naming
    "SIM",  # flake8-simplify
]

ignore = [
    "ANN101",  # self sem annotation
    "ANN102",  # cls sem annotation
]

[tool.ruff.lint.per-file-ignores]
"tests/**" = ["S101"]   # assert em testes é ok
```

---

## 5. Tratamento de Erros

### 5.1 Exceções

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | `except:` sem tipo (bare except) | `except Exception as e:` com log | 🔴 CRÍTICO |
| ☐ | `except Exception: pass` — engole erros | Logar ou re-raise | 🔴 CRÍTICO |
| ☐ | Exceções genéricas `raise Exception("erro")` | Exceções de domínio específicas | 🟠 IMPORTANTE |
| ☐ | Stack trace nunca logado em erros inesperados | `logger.exception("msg")` | 🔴 CRÍTICO |
| ☐ | `try/except` muito amplos (cobrindo código demais) | Reduzir escopo do try | 🟠 IMPORTANTE |

```python
# ❌ Anti-padrões
try:
    result = do_everything()
except:
    pass

# ✅ Hierarquia de exceções de domínio
class AppError(Exception):
    """Base para todas as exceções da aplicação."""

class RepositoryError(AppError):
    """Erros de acesso a dados."""

class EmailAlreadyExistsError(AppError):
    def __init__(self, email: str) -> None:
        super().__init__(f"E-mail já cadastrado: {email}")
        self.email = email

# ✅ Tratamento correto
import logging
logger = logging.getLogger(__name__)

try:
    user = self._repository.find_by_id(user_id)
except RepositoryError as e:
    logger.exception("Falha ao buscar usuário %d", user_id)
    raise  # re-raise para camada superior tratar
```

### 5.2 Context Managers

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | `f = open(...)` sem `with` | `with open(...) as f:` | 🔴 CRÍTICO |
| ☐ | Conexão de banco sem context manager | `with db.transaction():` | 🔴 CRÍTICO |
| ☐ | Lock sem `with` | `with self._lock:` | 🔴 CRÍTICO |
| ☐ | Recursos externos sem `contextlib` | `@contextmanager` | 🟠 IMPORTANTE |

```python
# ✅ Context manager para transação MySQL
from contextlib import contextmanager
from PySide6.QtSql import QSqlDatabase

@contextmanager
def sql_transaction(db: QSqlDatabase):
    db.transaction()
    try:
        yield db
        db.commit()
    except Exception:
        db.rollback()
        raise
```

---

## 6. Concorrência — QThread e asyncio

### 6.1 QThread Pattern correto

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Operações longas em `QThread` ou `QRunnable` — não na event loop | 🔴 CRÍTICO |
| ☐ | Worker movido para thread com `moveToThread` — não subclasse de QThread | 🟠 IMPORTANTE |
| ☐ | Sem acesso a widgets de thread secundária | 🔴 CRÍTICO |
| ☐ | Cross-thread signals com `Qt.QueuedConnection` | 🔴 CRÍTICO |

```python
# ✅ Worker com moveToThread — padrão correto Python/PySide6
from PySide6.QtCore import QObject, QThread, Signal

class DataWorker(QObject):
    result_ready = Signal(list)
    error_occurred = Signal(str)

    def __init__(self, service):
        super().__init__()
        self._service = service

    def process(self):
        try:
            data = self._service.fetch_all()  # operação longa
            self.result_ready.emit(data)
        except Exception as e:
            self.error_occurred.emit(str(e))


class MainWindow(QWidget):
    def _start_loading(self):
        self._thread = QThread(self)
        self._worker = DataWorker(self._service)
        self._worker.moveToThread(self._thread)

        self._thread.started.connect(self._worker.process)
        self._worker.result_ready.connect(self._on_data_ready)
        self._worker.error_occurred.connect(self._on_error)
        self._thread.finished.connect(self._thread.deleteLater)

        self._thread.start()

    def _on_data_ready(self, data: list):
        # Seguro — roda na thread principal via QueuedConnection automático
        self._model.update_data(data)
```

### 6.2 asyncio com Qt (qasync)

```python
# Integração asyncio com PySide6 via qasync
import asyncio
import qasync

@qasync.asyncSlot()
async def on_load_clicked(self):
    self.btn_load.setEnabled(False)
    try:
        users = await asyncio.to_thread(self._service.fetch_all)
        self._model.update_data(users)
    except Exception as e:
        self._show_error(str(e))
    finally:
        self.btn_load.setEnabled(True)
```

---

## 7. Mutabilidade e Argumentos Padrão

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | `def f(items=[]):` — mutable default argument | `def f(items=None): items = items or []` | 🔴 CRÍTICO |
| ☐ | `def f(config={}):` | `def f(config=None): config = config or {}` | 🔴 CRÍTICO |
| ☐ | Modificar lista/dict recebido como parâmetro sem intenção clara | Copiar antes de modificar: `items = list(items)` | 🟠 IMPORTANTE |

```python
# ❌ Bug clássico Python — mutable default argument
def add_user(user, users=[]):   # [] é criado UMA vez, compartilhado!
    users.append(user)
    return users

# ✅ Correto
def add_user(user, users=None):
    if users is None:
        users = []
    users.append(user)
    return users

# ✅ Com dataclass field
from dataclasses import dataclass, field

@dataclass
class OrderService:
    _cache: list = field(default_factory=list)  # nova lista por instância
```

---

## 8. Generators e Lazy Evaluation

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Processamento de listas grandes usando generators — não carregando tudo em memória | 🟠 IMPORTANTE |
| ☐ | `yield from` para delegar generators | 🟢 COSMÉTICO |
| ☐ | List comprehension em vez de `map`/`filter` com `lambda` | 🟢 COSMÉTICO |
| ☐ | Generator expression em vez de list quando não precisa de indexação | 🟠 IMPORTANTE |

```python
# ❌ Carrega todos os registros em memória
def get_all_users(self) -> list[User]:
    return [self._map_row(row) for row in self._fetch_all_rows()]

# ✅ Generator — lazy, memória constante para grandes volumes
def iter_all_users(self) -> Generator[User, None, None]:
    for row in self._fetch_all_rows():
        yield self._map_row(row)

# ✅ Uso
for user in repo.iter_all_users():
    process(user)  # nunca tem tudo em memória
```

---

## 9. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria de qualidade de código Python moderno neste projeto.
Reporte em português.

## MODERNIDADE PYTHON
- Há uso de Dict/List/Optional de typing em vez dos nativos?
- Union[X, Y] em vez de X | Y?
- Dicts crus passados entre camadas em vez de dataclasses?
- Strings mágicas em vez de StrEnum/IntEnum?
- f-strings usadas em vez de % ou .format()?
- match/case disponível onde caberia?

## TYPE HINTS
- Todos os métodos públicos têm anotações de tipo?
- mypy roda sem erros com strict=true?
- Há Any sem justificativa?
- Protocols usados para duck typing?

## QUALIDADE
- ruff roda sem erros?
- Há bare except ou except Exception: pass?
- Há mutable default arguments?
- Arquivos abertos sem with?
- Docstrings em classes e métodos públicos?

## CONCORRÊNCIA
- Operações longas na event loop do Qt?
- QThread usado corretamente (moveToThread)?
- Sem acesso a widgets de thread secundária?

## DIAGNÓSTICO FINAL
- 5 piores violações de Python moderno
- 3 maiores riscos de bugs sutis
- Prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 10. Roadmap Python Moderno

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | ruff zero erros | Configurar ruff + corrigir todos os erros | CI verde com ruff |
| 🔴 2 | Eliminar bare except | Exceções de domínio + logging correto | grep sem `except:` |
| 🔴 3 | Type hints completos | Anotar todos os métodos públicos | mypy sem erros |
| 🟠 4 | Dataclasses | Substituir dicts crus por dataclasses | grep sem `dict()` entre camadas |
| 🟠 5 | Enums | Substituir strings/ints mágicos | grep sem magic strings |
| 🟠 6 | Thread safety | Operações longas em QThread correto | Event loop nunca trava |
| 🟢 7 | Python 3.10+ | X\|None, match/case, tipos nativos | mypy target 3.11 |
| 🟢 8 | Generators | Lazy evaluation para grandes volumes | Heaptrack sem spikes |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

# Parte III — Web (PHP / JavaScript / HTML / MySQL)


---

## III1 — Arquitetura 4 Camadas Web — SOLID, DRY, TDD com PHPUnit

# 🏛️ Auditoria de Arquitetura — 4 Camadas
## Front / Mid / Back / Foundation · TDD · SOLID · DRY
### Stack: HTML · JavaScript · PHP · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | HTML / JS / PHP / MySQL |

---

## 1. Visão Geral da Arquitetura em 4 Camadas

```
┌─────────────────────────────────────────────────────────┐
│  LAYER 1 — FRONT          HTML + CSS + JavaScript        │
│  Apresentação, UX, validação client-side, SPA/MPA        │
├─────────────────────────────────────────────────────────┤
│  LAYER 2 — MID            PHP (Controllers + Services)   │
│  Orquestração, regras de negócio, autenticação, API      │
├─────────────────────────────────────────────────────────┤
│  LAYER 3 — BACK           PHP (Repositories + Models)    │
│  Acesso a dados, mapeamento objeto-relacional, queries   │
├─────────────────────────────────────────────────────────┤
│  LAYER 4 — FOUNDATION     MySQL + Infraestrutura         │
│  Schema, índices, constraints, backup, configuração      │
└─────────────────────────────────────────────────────────┘
```

### Regra de dependência obrigatória

```
FRONT  →  só fala com MID  (via API REST ou form POST)
MID    →  só fala com BACK (nunca acessa banco diretamente)
BACK   →  só fala com FOUNDATION (queries isoladas no Repository)
FOUNDATION  →  não conhece as camadas superiores
```

> ⚠️ **Violação mais comum gerada por IA fraca:** SQL diretamente em Controllers ou em arquivos de view — colapso total das camadas 2, 3 e 4 em um único arquivo.

---

## 2. Checklist de Auditoria por Camada

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

### 2.1 Layer 1 — Front (HTML / CSS / JavaScript)

#### Estrutura e Separação

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | HTML sem lógica de negócio embutida (sem PHP misturado) | 🔴 CRÍTICO | SRP / Camadas |
| ☐ | JavaScript em arquivos `.js` separados — sem `<script>` inline com lógica | 🟠 IMPORTANTE | SRP |
| ☐ | CSS em arquivos externos — sem `style=""` com regras complexas | 🟠 IMPORTANTE | SRP |
| ☐ | Componentes JS reutilizáveis (funções, módulos) — sem duplicação de código | 🔴 CRÍTICO | DRY |
| ☐ | Eventos JS via `addEventListener` — nunca `onclick=""` inline no HTML | 🟠 IMPORTANTE | SRP |
| ☐ | Templates HTML com estrutura semântica (`header`, `main`, `nav`, `section`) | 🟠 IMPORTANTE | SRP |

#### Validação e Comunicação com MID

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Validação client-side presente mas não exclusiva (duplicada no servidor) | 🔴 CRÍTICO | Defense in depth |
| ☐ | Toda comunicação assíncrona via `fetch`/`XMLHttpRequest` para endpoints MID | 🔴 CRÍTICO | Camadas |
| ☐ | Nenhuma query SQL ou lógica de banco visível no front | 🔴 CRÍTICO | Camadas |
| ☐ | Mensagens de erro tratadas no JS — sem `alert()` com stack trace | 🟠 IMPORTANTE | SRP |
| ☐ | Estado da UI gerenciado de forma centralizada (sem variáveis globais espalhadas) | 🟠 IMPORTANTE | DRY / SRP |
| ☐ | Funções JS com responsabilidade única — máximo ~20 linhas por função | 🟠 IMPORTANTE | SRP |

#### DRY no Front

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Funções utilitárias em arquivo `utils.js` compartilhado | 🟠 IMPORTANTE | DRY |
| ☐ | Templates HTML reutilizados (includes, partials ou Web Components) | 🟠 IMPORTANTE | DRY |
| ☐ | Constantes de URL de API centralizadas em `config.js` ou equivalente | 🟠 IMPORTANTE | DRY |
| ☐ | Estilos CSS usando variáveis (`--custom-properties`) para cores e espaçamentos | 🟢 COSMÉTICO | DRY |

---

### 2.2 Layer 2 — Mid (PHP Controllers + Services)

#### Controllers — Porta de Entrada

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Controllers recebem request, delegam para Service, retornam response — nada mais | 🔴 CRÍTICO | SRP |
| ☐ | Nenhuma query SQL dentro de Controllers | 🔴 CRÍTICO | Camadas / SRP |
| ☐ | Nenhum HTML gerado dentro de Controllers | 🔴 CRÍTICO | SRP |
| ☐ | Controller por recurso/entidade — sem "GodController" com 1000 linhas | 🔴 CRÍTICO | SRP |
| ☐ | Validação de input no Controller antes de passar ao Service | 🟠 IMPORTANTE | SRP |
| ☐ | Autenticação/autorização verificada no Controller (middleware ou guard) | 🔴 CRÍTICO | SRP |

#### Services — Regras de Negócio

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Toda regra de negócio vive em classes Service — não em Controllers nem em Views | 🔴 CRÍTICO | SRP |
| ☐ | Services dependem de interfaces de Repository — não de implementações concretas | 🔴 CRÍTICO | DIP |
| ☐ | Services injetados via construtor — sem `new Service()` dentro de Controllers | 🔴 CRÍTICO | DIP / DRY |
| ☐ | Services não conhecem detalhes de HTTP (sem `$_GET`, `$_POST` diretamente) | 🔴 CRÍTICO | SRP / Camadas |
| ☐ | Lógica de negócio sem duplicação entre Services | 🔴 CRÍTICO | DRY |
| ☐ | Services retornam objetos de domínio ou DTOs — não arrays crus | 🟠 IMPORTANTE | OCP / SRP |

#### Open/Closed e Extensibilidade

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Novos comportamentos adicionados via extensão — não editando classes existentes | 🟠 IMPORTANTE | OCP |
| ☐ | Uso de interfaces para comportamentos variáveis (ex: múltiplos métodos de pagamento) | 🟠 IMPORTANTE | OCP / DIP |
| ☐ | Configurações e estratégias injetadas — sem `if/elseif` em cascata para comportamentos | 🟠 IMPORTANTE | OCP |

---

### 2.3 Layer 3 — Back (PHP Repositories + Models)

#### Repository Pattern

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Interface `RepositoryInterface` definida para cada entidade | 🔴 CRÍTICO | DIP / ISP |
| ☐ | Implementação concreta `MySQLUserRepository` separada da interface | 🔴 CRÍTICO | DIP |
| ☐ | Toda query SQL confinada dentro do Repository correspondente | 🔴 CRÍTICO | SRP / Camadas |
| ☐ | Repository não contém regra de negócio — apenas operações CRUD e queries | 🔴 CRÍTICO | SRP |
| ☐ | Queries complexas encapsuladas em métodos descritivos (`findActiveByRole`, etc.) | 🟠 IMPORTANTE | SRP / DRY |
| ☐ | Sem duplicação de queries entre Repositories diferentes | 🔴 CRÍTICO | DRY |

#### Models / Entidades

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Model representa a entidade de domínio — sem lógica de apresentação | 🟠 IMPORTANTE | SRP |
| ☐ | Propriedades tipadas explicitamente (PHP 7.4+ typed properties) | 🟠 IMPORTANTE | SRP |
| ☐ | Construtores validam invariantes da entidade | 🟠 IMPORTANTE | SRP |
| ☐ | Sem herança profunda de Models (máximo 2 níveis) | 🟠 IMPORTANTE | LSP |

#### Liskov Substitution

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Subclasses não quebram contratos da classe pai (mesmas pré/pós condições) | 🟠 IMPORTANTE | LSP |
| ☐ | Repositórios concretos implementam completamente suas interfaces | 🔴 CRÍTICO | LSP / ISP |
| ☐ | Nenhum método de interface implementado com `throw new NotImplementedException` | 🔴 CRÍTICO | LSP |

#### Interface Segregation

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Interfaces pequenas e coesas — sem "GodInterface" com 30 métodos | 🟠 IMPORTANTE | ISP |
| ☐ | Classes não forçadas a implementar métodos que não usam | 🟠 IMPORTANTE | ISP |
| ☐ | Interfaces segregadas por contexto de uso (leitura vs escrita, por exemplo) | 🟢 COSMÉTICO | ISP |

---

### 2.4 Layer 4 — Foundation (MySQL + Infraestrutura)

#### Schema e Constraints

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Todas as tabelas com chave primária definida | 🔴 CRÍTICO | Foundation |
| ☐ | Chaves estrangeiras com `ON DELETE` e `ON UPDATE` explícitos | 🟠 IMPORTANTE | Foundation |
| ☐ | Constraints `NOT NULL` onde o campo é obrigatório | 🟠 IMPORTANTE | Foundation |
| ☐ | Constraints `UNIQUE` onde o negócio exige unicidade | 🟠 IMPORTANTE | DRY / Foundation |
| ☐ | Tipos de dados adequados — sem `VARCHAR` para datas, `INT` para booleano | 🟠 IMPORTANTE | Foundation |
| ☐ | Charset `utf8mb4` e collation `utf8mb4_unicode_ci` em todas as tabelas | 🟠 IMPORTANTE | Foundation |

#### Performance e Índices

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Índices em todas as colunas usadas em `WHERE`, `JOIN` e `ORDER BY` | 🔴 CRÍTICO | Foundation |
| ☐ | `EXPLAIN` executado nas queries mais frequentes para validar uso de índices | 🟠 IMPORTANTE | Foundation |
| ☐ | Sem `SELECT *` — colunas explícitas em todas as queries | 🟠 IMPORTANTE | DRY / Foundation |
| ☐ | Sem N+1 queries — uso de `JOIN` ou carregamento em lote | 🔴 CRÍTICO | Foundation |
| ☐ | Paginação em todas as listagens (sem `SELECT` sem `LIMIT`) | 🟠 IMPORTANTE | Foundation |

#### Migrations e Versionamento de Schema

| # | Item de Verificação | Prioridade | Princípio |
|---|---------------------|-----------|-----------|
| ☐ | Alterações de schema via migrations versionadas — nunca edição manual | 🔴 CRÍTICO | Foundation / DRY |
| ☐ | Migrations com `up()` e `down()` (rollback possível) | 🟠 IMPORTANTE | Foundation |
| ☐ | Migrations em controle de versão junto ao código da aplicação | 🔴 CRÍTICO | Foundation |
| ☐ | Schema inicial gerado por migration — sem dump SQL avulso | 🟠 IMPORTANTE | DRY |

---

## 3. TDD — Red / Green / Refactor

### 3.1 O ciclo obrigatório

```
┌──────────────────────────────────────────────────────────┐
│                                                          │
│   🔴 RED      Escreva o teste ANTES do código.           │
│               O teste deve falhar — isso é correto.      │
│                                                          │
│   🟢 GREEN    Escreva o mínimo de código para            │
│               o teste passar. Sem otimização ainda.      │
│                                                          │
│   🔵 REFACTOR Melhore o código sem quebrar os testes.    │
│               Aqui entram SOLID e DRY.                   │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

> ⚠️ **Violação mais comum:** escrever código e depois escrever testes que confirmam o que já foi feito. Isso não é TDD — é teste de regressão tardio.

---

### 3.2 Checklist de TDD por camada

#### Testes de Unidade — Layer MID (Services)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Cada método público de Service tem ao menos um teste unitário | 🔴 CRÍTICO |
| ☐ | Testes escritos ANTES da implementação (Red primeiro) | 🔴 CRÍTICO |
| ☐ | Dependências (Repositories) substituídas por mocks/stubs nos testes de unidade | 🔴 CRÍTICO |
| ☐ | Testes cobrem caminho feliz E casos de erro (exceções, dados inválidos) | 🔴 CRÍTICO |
| ☐ | Nomes de teste descrevem comportamento: `testShouldThrowWhenEmailIsInvalid()` | 🟠 IMPORTANTE |
| ☐ | Cada teste verifica UMA única coisa — sem asserts múltiplos não relacionados | 🟠 IMPORTANTE |
| ☐ | Testes independentes — sem dependência de ordem de execução | 🔴 CRÍTICO |
| ☐ | Sem lógica condicional dentro dos testes (`if` em teste = teste mal escrito) | 🟠 IMPORTANTE |

#### Testes de Integração — Layer BACK (Repositories)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Repositories testados contra banco de dados de teste (SQLite in-memory ou MySQL de teste) | 🔴 CRÍTICO |
| ☐ | Cada operação CRUD tem teste de integração | 🔴 CRÍTICO |
| ☐ | Banco de teste resetado entre testes (transactions com rollback ou fixtures) | 🔴 CRÍTICO |
| ☐ | Testes de integração separados dos unitários (pastas distintas, suites distintas) | 🟠 IMPORTANTE |
| ☐ | Queries complexas (`findActiveByRole`, relatórios) cobertas por testes de integração | 🟠 IMPORTANTE |

#### Testes E2E — Layer FRONT (JavaScript)

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Fluxos críticos cobertos por testes E2E (login, cadastro, checkout, etc.) | 🔴 CRÍTICO |
| ☐ | Funções JS utilitárias testadas com Jest ou equivalente | 🟠 IMPORTANTE |
| ☐ | Validações client-side testadas de forma isolada | 🟠 IMPORTANTE |
| ☐ | Testes E2E executados em pipeline CI antes do deploy | 🔴 CRÍTICO |

#### Métricas de Cobertura

| # | Item de Verificação | Meta | Prioridade |
|---|---------------------|------|-----------|
| ☐ | Cobertura de linhas nos Services | ≥ 80% | 🔴 CRÍTICO |
| ☐ | Cobertura de branches nos Services | ≥ 70% | 🟠 IMPORTANTE |
| ☐ | Cobertura de linhas nos Repositories | ≥ 70% | 🟠 IMPORTANTE |
| ☐ | Cobertura total do projeto | ≥ 60% | 🟠 IMPORTANTE |
| ☐ | Relatório de cobertura gerado no CI e publicado como artefato | — | 🟠 IMPORTANTE |

---

### 3.3 Configuração PHPUnit para TDD

**Instalação:**

```bash
composer require --dev phpunit/phpunit
composer require --dev phpunit/php-code-coverage
```

**Arquivo `phpunit.xml`:**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<phpunit bootstrap="vendor/autoload.php"
         colors="true"
         stopOnFailure="false">

  <testsuites>
    <testsuite name="Unit">
      <directory>tests/Unit</directory>
    </testsuite>
    <testsuite name="Integration">
      <directory>tests/Integration</directory>
    </testsuite>
  </testsuites>

  <coverage>
    <include>
      <directory suffix=".php">src</directory>
    </include>
    <report>
      <html outputDirectory="coverage"/>
      <clover outputFile="coverage.xml"/>
    </report>
  </coverage>

  <php>
    <env name="APP_ENV" value="testing"/>
    <env name="DB_DATABASE" value=":memory:"/>
  </php>

</phpunit>
```

**Estrutura de diretórios obrigatória:**

```
tests/
├── Unit/
│   ├── Services/
│   │   ├── UserServiceTest.php
│   │   └── OrderServiceTest.php
│   └── Models/
│       └── UserTest.php
├── Integration/
│   └── Repositories/
│       ├── UserRepositoryTest.php
│       └── OrderRepositoryTest.php
└── E2E/
    └── Auth/
        └── LoginFlowTest.php
```

**Exemplo de teste unitário em ciclo Red/Green/Refactor:**

```php
// 🔴 RED — escreva este teste ANTES de criar UserService
class UserServiceTest extends TestCase
{
    public function testShouldThrowExceptionWhenEmailIsAlreadyRegistered(): void
    {
        // Arrange
        $repository = $this->createMock(UserRepositoryInterface::class);
        $repository->method('findByEmail')->willReturn(new User('existing@email.com'));

        $service = new UserService($repository);

        // Assert (antes do Act — estilo TDD)
        $this->expectException(EmailAlreadyRegisteredException::class);

        // Act
        $service->register('existing@email.com', 'SenhaSegura123!');
    }
}

// 🟢 GREEN — implemente o mínimo para passar
class UserService
{
    public function __construct(private UserRepositoryInterface $repository) {}

    public function register(string $email, string $password): User
    {
        if ($this->repository->findByEmail($email)) {
            throw new EmailAlreadyRegisteredException();
        }
        // ... resto da implementação
    }
}

// 🔵 REFACTOR — agora melhore sem quebrar o teste
// Extraia validação de senha para PasswordValidator::validate($password)
// Aplique SRP: UserService orquestra, PasswordValidator valida
```

---

## 4. Princípios SOLID — Checklist Detalhado

### 4.1 S — Single Responsibility Principle (SRP)

> *"Uma classe deve ter apenas um motivo para mudar."*

| # | Anti-padrão (o que procurar) | Correção | Camada |
|---|------------------------------|---------|--------|
| ☐ | Controller com 500+ linhas fazendo tudo | Extrair lógica para Service | MID |
| ☐ | Função JS de 100+ linhas com múltiplas responsabilidades | Decompor em funções menores | FRONT |
| ☐ | Repository com validação de negócio embutida | Mover validação para Service | BACK |
| ☐ | Model com métodos de formatação de apresentação | Mover para ViewModel/Presenter | BACK |
| ☐ | Arquivo PHP com HTML, SQL e lógica na mesma classe | Separar em Controller/View/Repository | TODAS |
| ☐ | Função que tanto salva no banco quanto envia e-mail | Separar em Service + Mailer | MID |

**Teste rápido de SRP:** consegue descrever a responsabilidade da classe em uma frase sem usar "e"? Se não, violou SRP.

---

### 4.2 O — Open/Closed Principle (OCP)

> *"Aberto para extensão, fechado para modificação."*

| # | Anti-padrão (o que procurar) | Correção | Camada |
|---|------------------------------|---------|--------|
| ☐ | `if ($tipo === 'pix') ... elseif ($tipo === 'cartao') ...` em Service | Interface `PaymentGatewayInterface` + implementações | MID |
| ☐ | Switch/case crescente para comportamentos variáveis | Strategy Pattern via interface | MID |
| ☐ | Adicionar novo relatório exige editar classe de relatório existente | Abstract `ReportGenerator` + subclasses | BACK |
| ☐ | Novas regras de desconto editam função `calcularDesconto()` existente | `DiscountRuleInterface` + injeção | MID |

**Sinal de alerta:** se adicionar um novo tipo de comportamento exige editar classes já testadas, OCP foi violado.

---

### 4.3 L — Liskov Substitution Principle (LSP)

> *"Subclasses devem poder substituir a classe pai sem alterar o comportamento esperado."*

| # | Anti-padrão (o que procurar) | Correção | Camada |
|---|------------------------------|---------|--------|
| ☐ | Método de interface implementado com `throw new \Exception('not implemented')` | Segregar interface (ISP) | BACK |
| ☐ | Subclasse que restringe o contrato da pai (lança exceção onde pai não lançaria) | Revisar hierarquia | BACK |
| ☐ | `MySQLUserRepository` não implementa todos os métodos de `UserRepositoryInterface` | Implementar completamente | BACK |
| ☐ | Subclasse que ignora parâmetros obrigatórios do método pai | Refatorar contrato | BACK |

---

### 4.4 I — Interface Segregation Principle (ISP)

> *"Clientes não devem ser forçados a depender de interfaces que não usam."*

| # | Anti-padrão (o que procurar) | Correção | Camada |
|---|------------------------------|---------|--------|
| ☐ | Interface com 20+ métodos (GodInterface) | Quebrar em interfaces menores por contexto | BACK/MID |
| ☐ | Repository de leitura forçado a implementar métodos de escrita | `ReadableRepositoryInterface` separada | BACK |
| ☐ | Service que implementa interface de módulo externo com métodos irrelevantes | Criar adapter específico | MID |
| ☐ | Classes com métodos implementados como no-op só para satisfazer interface | Segregar interface | BACK/MID |

---

### 4.5 D — Dependency Inversion Principle (DIP)

> *"Dependa de abstrações, não de implementações concretas."*

| # | Anti-padrão (o que procurar) | Correção | Camada |
|---|------------------------------|---------|--------|
| ☐ | `$repo = new MySQLUserRepository()` dentro de Service | Injetar via construtor | MID |
| ☐ | `$mailer = new SMTPMailer()` instanciado dentro de classe | Injetar `MailerInterface` | MID |
| ☐ | Controller instanciando Service diretamente com `new` | Usar container de injeção de dependência | MID |
| ☐ | Dependência de classe concreta em vez de interface nos type hints | Tipar com interface | MID/BACK |
| ☐ | Ausência de container IoC (Dependency Injection Container) | Implementar DI Container simples | MID |

**Exemplo correto de DIP em PHP:**

```php
// ✅ CORRETO — depende de abstração
class UserService
{
    public function __construct(
        private UserRepositoryInterface $repository,  // interface
        private MailerInterface $mailer,              // interface
        private LoggerInterface $logger               // interface
    ) {}
}

// ❌ ERRADO — depende de implementação concreta
class UserService
{
    public function __construct()
    {
        $this->repository = new MySQLUserRepository(); // concreto
        $this->mailer     = new SMTPMailer();           // concreto
    }
}
```

---

## 5. Princípio DRY — Don't Repeat Yourself

> *"Cada pedaço de conhecimento deve ter uma representação única, inequívoca e autoritativa dentro de um sistema."*

### 5.1 DRY por camada

#### FRONT — JavaScript / HTML

| # | Duplicação comum | Solução DRY |
|---|-----------------|-------------|
| ☐ | Mesma validação de e-mail em 3 formulários diferentes | Função `validateEmail(email)` em `utils.js` |
| ☐ | URL da API repetida em múltiplos arquivos JS | Constante `API_BASE_URL` em `config.js` |
| ☐ | Mesmo bloco `<nav>` copiado em 10 páginas HTML | PHP `include('partials/nav.php')` ou template engine |
| ☐ | Mesma lógica de exibição de erro repetida em vários handlers | Função `showError(message, element)` |
| ☐ | Estilos de botão primário redefinidos em múltiplos arquivos CSS | Classe CSS `.btn-primary` centralizada |

#### MID — PHP Services / Controllers

| # | Duplicação comum | Solução DRY |
|---|-----------------|-------------|
| ☐ | Mesma query de busca por e-mail em UserService e AuthService | Método no Repository `findByEmail()` |
| ☐ | Lógica de paginação repetida em múltiplos Controllers | Trait `Paginatable` ou classe `Paginator` |
| ☐ | Formatação de resposta JSON repetida em 15 Controllers | Método `jsonResponse()` em BaseController |
| ☐ | Verificação de autenticação copiada em cada Controller | Middleware `AuthMiddleware` aplicado globalmente |
| ☐ | Regra de negócio duplicada em Service e em teste | Extrair para método privado ou Value Object |

#### BACK — PHP Repositories

| # | Duplicação comum | Solução DRY |
|---|-----------------|-------------|
| ☐ | Lógica de conexão PDO repetida em cada Repository | BaseRepository com conexão injetada |
| ☐ | Clausulas WHERE comuns repetidas entre queries | Query Builder interno ou Specification Pattern |
| ☐ | Mapeamento array → objeto duplicado em múltiplos métodos | Método `hydrate(array $row): Entity` |
| ☐ | Queries de listagem com paginação repetidas | Método `paginate(string $query, int $page)` em BaseRepository |

#### FOUNDATION — MySQL

| # | Duplicação comum | Solução DRY |
|---|-----------------|-------------|
| ☐ | Colunas `created_at`/`updated_at` com definições inconsistentes entre tabelas | Padrão único documentado e aplicado via migration base |
| ☐ | Mesma lógica de soft-delete replicada em múltiplas tabelas | Coluna `deleted_at` padronizada + Stored Procedure ou filtro no Repository |
| ☐ | Índices idênticos criados manualmente em tabelas diferentes | Migration compartilhada ou script de padronização |

---

### 5.2 Armadilha do DRY prematuro

> ⚠️ Nem toda duplicação deve ser eliminada imediatamente. A regra prática é: **duplique uma vez, na segunda duplicação avalie; na terceira, elimine.**

| Situação | Ação recomendada |
|----------|-----------------|
| Código parecido em 2 lugares com contextos diferentes | Aguardar — pode ser coincidência |
| Código idêntico em 3+ lugares | Extrair imediatamente |
| Regra de negócio duplicada mesmo que em 2 lugares | Extrair imediatamente (conhecimento, não código) |
| Infraestrutura duplicada (conexão, log, resposta HTTP) | Extrair imediatamente |

---

## 6. Prompt de Auditoria para Claude Opus

Cole no Claude Code na raiz do projeto:

```
Faça uma auditoria completa de arquitetura em 4 camadas deste projeto PHP/MySQL/JS.
Reporte em português.

## LAYER 1 — FRONT (HTML/CSS/JS)
- Existe separação real entre HTML, CSS e JS?
- Há lógica de negócio ou SQL embutidos no front?
- Funções JS têm responsabilidade única (SRP)?
- Há duplicação de código JS entre arquivos (DRY)?
- Validações estão duplicadas corretamente entre client e server?

## LAYER 2 — MID (PHP Controllers + Services)
- Controllers orquestram sem implementar regra de negócio?
- Services existem como camada intermediária?
- Há GodController ou GodService (viloação de SRP)?
- Services dependem de interfaces ou de classes concretas (DIP)?
- Existe lógica de negócio duplicada entre Services (DRY)?

## LAYER 3 — BACK (PHP Repositories + Models)
- Existe Repository Pattern com interface e implementação separadas?
- Toda query SQL está confinada nos Repositories?
- Repositories implementam completamente suas interfaces (LSP)?
- Interfaces são coesas ou há GodInterface (ISP)?
- Há queries duplicadas entre Repositories (DRY)?

## LAYER 4 — FOUNDATION (MySQL)
- Schema tem constraints adequadas (FK, NOT NULL, UNIQUE)?
- Há índices nas colunas usadas em WHERE/JOIN/ORDER BY?
- Alterações de schema são versionadas via migrations?
- Há N+1 queries identificáveis?

## TDD
- Existe cobertura de testes para os Services?
- Testes parecem ter sido escritos antes ou depois do código?
- Há mocks/stubs para isolar dependências nos testes unitários?
- Qual é a cobertura estimada por camada?

## SOLID e DRY
- Identifique as 3 piores violações de SRP no projeto
- Identifique as 3 piores violações de DRY no projeto
- Existe algum uso de OCP (interfaces para comportamento variável)?
- Dependências são injetadas ou instanciadas internamente (DIP)?

## DIAGNÓSTICO FINAL
- Liste violações por prioridade: CRÍTICO / IMPORTANTE / COSMÉTICO
- Mapa de dependências entre camadas (quem chama quem)
- Estime cobertura de testes atual (%)
- Priorize as 5 ações de maior impacto arquitetural
```

---

## 7. Roadmap de Refatoração Arquitetural

| Sprint | Camada foco | Objetivo | Ações | Critério de conclusão |
|--------|------------|---------|-------|----------------------|
| 🔴 1 | TODAS | Separar camadas colapsadas | Mover SQL para Repositories, lógica para Services, sem PHP em HTML | Zero SQL fora de Repositories |
| 🔴 2 | MID / BACK | Implementar DIP | Criar interfaces para Repositories, injetar via construtor | Services sem `new ConcreteClass()` |
| 🟠 3 | MID | TDD nos Services | Escrever testes unitários com mocks para todos os Services | Cobertura ≥ 80% em Services |
| 🟠 4 | BACK | TDD nos Repositories | Testes de integração com banco de teste | Cobertura ≥ 70% em Repositories |
| 🟠 5 | FRONT | DRY e modularização JS | Centralizar utilitários, extrair partials HTML, config de API | Zero duplicação de funções JS |
| 🟠 6 | FOUNDATION | Migrations e schema | Versionar schema, adicionar constraints e índices faltantes | Schema 100% versionado |
| 🟢 7 | TODAS | ISP e OCP | Segregar interfaces grandes, substituir if/elseif por Strategy | PHPStan nível 8 sem erros |
| 🟢 8 | TODAS | Cobertura e CI | Pipeline TDD no GitHub Actions com gate de cobertura | CI bloqueia PR com cobertura < 60% |

---

## 8. Estrutura de Diretórios Recomendada

```
projeto/
├── public/                    ← LAYER 1 — webroot (único diretório acessível)
│   ├── index.php              ← Front controller
│   ├── assets/
│   │   ├── js/
│   │   │   ├── config.js      ← Constantes de URL (DRY)
│   │   │   ├── utils.js       ← Funções utilitárias (DRY)
│   │   │   └── modules/       ← Módulos JS por feature
│   │   └── css/
│   └── views/                 ← Templates HTML (sem lógica)
│       └── partials/          ← Includes reutilizáveis (DRY)
│
├── src/                       ← LAYER 2 e 3
│   ├── Controllers/           ← LAYER 2 — entrada HTTP
│   ├── Services/              ← LAYER 2 — regras de negócio
│   ├── Repositories/
│   │   ├── Interfaces/        ← LAYER 3 — contratos (DIP)
│   │   └── MySQL/             ← LAYER 3 — implementações
│   ├── Models/                ← LAYER 3 — entidades de domínio
│   └── Infrastructure/        ← DI Container, Middleware, etc.
│
├── database/                  ← LAYER 4
│   └── migrations/            ← Versionamento de schema (DRY)
│
└── tests/                     ← TDD
    ├── Unit/
    │   └── Services/
    ├── Integration/
    │   └── Repositories/
    └── E2E/
```

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III2 — Qualidade e Dívida Técnica PHP/JS

# 🧹 Auditoria de Qualidade de Código & Dívida Técnica
## Stack: HTML · JavaScript · PHP · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | PHP / JS / HTML |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  CODE SMELLS       Sintomas de código problemático       │
├─────────────────────────────────────────────────────────┤
│  COMPLEXIDADE      Ciclomática, cognitive, acoplamento   │
├─────────────────────────────────────────────────────────┤
│  DEAD CODE         Código morto, dependências orfãs      │
├─────────────────────────────────────────────────────────┤
│  MÉTRICAS          PHPMetrics, ESLint, ferramentas       │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Code Smells — PHP

### 2.1 God Class / God Function

| # | Sintoma | Limiar de alerta | Prioridade | Ferramenta |
|---|---------|-----------------|-----------|------------|
| ☐ | Classe com mais de 300 linhas | > 300 linhas | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Método/função com mais de 30 linhas | > 30 linhas | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Classe com mais de 10 métodos públicos | > 10 métodos | 🟠 IMPORTANTE | PHPMetrics |
| ☐ | Arquivo PHP com mais de 500 linhas | > 500 linhas | 🔴 CRÍTICO | wc -l |
| ☐ | Função com mais de 5 parâmetros | > 5 params | 🟠 IMPORTANTE | PHPStan |
| ☐ | Nível de indentação acima de 4 | > 4 níveis | 🟠 IMPORTANTE | PHP_CodeSniffer |

### 2.2 Feature Envy

| # | Sintoma | Prioridade |
|---|---------|-----------|
| ☐ | Método que usa mais dados de outra classe do que da própria | 🟠 IMPORTANTE |
| ☐ | Service que acessa diretamente propriedades de múltiplas entidades | 🟠 IMPORTANTE |
| ☐ | Controller que manipula internamente objetos retornados pelo Service | 🟠 IMPORTANTE |

### 2.3 Shotgun Surgery

| # | Sintoma | Prioridade |
|---|---------|-----------|
| ☐ | Adicionar um campo novo exige editar 10+ arquivos diferentes | 🔴 CRÍTICO |
| ☐ | Mudar regra de negócio requer alterações espalhadas em múltiplas classes | 🔴 CRÍTICO |
| ☐ | Sem ponto central de verdade para configurações de entidade | 🟠 IMPORTANTE |

### 2.4 Primitive Obsession

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | CPF representado como `string` pura sem validação | Value Object `CPF` | 🟠 IMPORTANTE |
| ☐ | Dinheiro como `float` (perde precisão) | `int` em centavos ou `Money` VO | 🔴 CRÍTICO |
| ☐ | Status como string mágica: `"ativo"`, `"pendente"` | Enum PHP 8.1 | 🟠 IMPORTANTE |
| ☐ | Array associativo passado entre camadas como "objeto" | DTO ou classe tipada | 🟠 IMPORTANTE |

### 2.5 Long Parameter List

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | Função com 6+ parâmetros | Agrupar em DTO/objeto de configuração | 🟠 IMPORTANTE |
| ☐ | Construtor com 8+ dependências (possível SRP violado) | Extrair responsabilidades | 🔴 CRÍTICO |

### 2.6 Magic Numbers e Magic Strings

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | `if ($status === 2)` sem constante nomeada | `const STATUS_ACTIVE = 2` | 🟠 IMPORTANTE |
| ☐ | `sleep(300)` sem comentário explicativo | `const CACHE_TTL_SECONDS = 300` | 🟠 IMPORTANTE |
| ☐ | `$role === 'admin'` repetido em 20 lugares | Enum ou constante centralizada | 🔴 CRÍTICO |

---

## 3. Code Smells — JavaScript

| # | Sintoma | Correção | Prioridade |
|---|---------|---------|-----------|
| ☐ | `var` em vez de `let`/`const` | Substituir por `const` por padrão | 🔴 CRÍTICO |
| ☐ | Callback hell (aninhamento > 3 níveis) | Refatorar para `async/await` | 🔴 CRÍTICO |
| ☐ | `console.log` em código de produção | Remover ou usar logger condicional | 🟠 IMPORTANTE |
| ☐ | Funções anônimas gigantes em event listeners | Extrair para funções nomeadas | 🟠 IMPORTANTE |
| ☐ | Variáveis globais em `window.*` | Módulos ES6 com `import/export` | 🟠 IMPORTANTE |
| ☐ | `==` em vez de `===` (comparação sem tipo) | Substituir por `===` em todo o código | 🔴 CRÍTICO |
| ☐ | `innerHTML` com dados do usuário (XSS) | `textContent` ou sanitização | 🔴 CRÍTICO |
| ☐ | Sem tratamento de erro em `fetch` (sem `.catch`) | `try/catch` em todo `async/await` | 🔴 CRÍTICO |

---

## 4. Complexidade Ciclomática

### O que é

> Complexidade ciclomática mede o número de caminhos independentes em um trecho de código. Cada `if`, `else`, `for`, `while`, `case`, `catch` e `&&`/`||` adiciona +1.

### Limites recomendados

| Complexidade | Classificação | Ação recomendada |
|-------------|--------------|-----------------|
| 1–5 | ✅ Simples | Nenhuma ação necessária |
| 6–10 | ⚠️ Moderado | Monitorar, considerar refatoração |
| 11–20 | 🟠 Alto | Refatorar — dividir em funções menores |
| > 20 | 🔴 Crítico | Refatoração urgente — código não testável |

### Checklist

| # | Item de Verificação | Meta | Prioridade | Ferramenta |
|---|---------------------|------|-----------|------------|
| ☐ | Complexidade ciclomática média do projeto | ≤ 5 | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Nenhum método com complexidade > 15 | ≤ 15 | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Complexidade cognitiva média | ≤ 10 | 🟠 IMPORTANTE | PHPMetrics |
| ☐ | Funções JS com complexidade > 10 identificadas | ≤ 10 | 🟠 IMPORTANTE | ESLint complexity |

**Configurar limite no ESLint:**

```json
{
  "rules": {
    "complexity": ["error", 10],
    "max-lines-per-function": ["warn", { "max": 30 }],
    "max-depth": ["error", 4],
    "max-params": ["warn", 5]
  }
}
```

---

## 5. Acoplamento

### 5.1 Acoplamento Aferente e Eferente

| Métrica | Definição | Ideal |
|---------|----------|-------|
| Ca (Aferente) | Quantas classes dependem desta | Alto = classe estável/importante |
| Ce (Eferente) | De quantas classes esta depende | Baixo = classe independente |
| Instabilidade (I = Ce/Ca+Ce) | 0 = estável, 1 = instável | Camadas base = 0, camadas externas = 1 |

### 5.2 Checklist de Acoplamento

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Classes de domínio (Models/Entities) sem dependência de infraestrutura | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Services não acoplados diretamente a classes de framework | 🟠 IMPORTANTE | PHPMetrics |
| ☐ | Repositórios não conhecem Controllers | 🔴 CRÍTICO | Revisão manual |
| ☐ | Circular dependencies identificadas e eliminadas | 🔴 CRÍTICO | PHPMetrics |
| ☐ | Nenhuma classe com Ce > 20 (depende de mais de 20 outras) | 🟠 IMPORTANTE | PHPMetrics |

---

## 6. Dead Code

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Métodos PHP nunca chamados identificados e removidos | 🟠 IMPORTANTE | PHPStan / Psalm |
| ☐ | Variáveis declaradas e nunca usadas | 🟠 IMPORTANTE | PHPStan |
| ☐ | Imports/use não utilizados | 🟠 IMPORTANTE | PHPStan |
| ☐ | Arquivos PHP órfãos (nunca incluídos/instanciados) | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Funções JS exportadas mas nunca importadas | 🟠 IMPORTANTE | ESLint `no-unused-vars` |
| ☐ | Tabelas MySQL sem referência no código | 🟢 COSMÉTICO | Grep no código |
| ☐ | CSS sem seletores correspondentes no HTML | 🟢 COSMÉTICO | PurgeCSS |
| ☐ | Comentários `// TODO` e `// FIXME` catalogados e priorizados | 🟢 COSMÉTICO | grep -r "TODO\|FIXME" |

---

## 7. Nomenclatura e Legibilidade

### PHP

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | Variáveis com nomes de 1 letra: `$x`, `$d`, `$tmp` | Nomes descritivos: `$userId`, `$discountAmount` | 🟠 IMPORTANTE |
| ☐ | Funções com nome vago: `process()`, `handle()`, `doStuff()` | Nomes que expressam intenção: `processRefund()` | 🔴 CRÍTICO |
| ☐ | Booleanos sem prefixo: `$active`, `$valid` | `$isActive`, `$isValid`, `$hasPermission` | 🟠 IMPORTANTE |
| ☐ | Classes sem sufixo de papel: `User` para service | `UserService`, `UserRepository`, `UserController` | 🟠 IMPORTANTE |
| ☐ | Mistura de idiomas: `$nome`, `$lastName`, `$idade` | Escolher um idioma e manter (preferir inglês) | 🟢 COSMÉTICO |

### JavaScript

| # | Anti-padrão | Correção | Prioridade |
|---|-------------|---------|-----------|
| ☐ | Funções anônimas sem contexto: `() => { ... }` longa | Função nomeada com intenção clara | 🟠 IMPORTANTE |
| ☐ | Variáveis booleanas sem `is`/`has`/`can` | `isLoading`, `hasError`, `canEdit` | 🟠 IMPORTANTE |
| ☐ | Event handlers genéricos: `handleClick`, `onClick` | `handleUserLoginClick`, `handleCartSubmit` | 🟢 COSMÉTICO |

---

## 8. Configuração de Ferramentas

### PHPMetrics

```bash
# Instalação
composer require --dev phpmetrics/phpmetrics

# Gerar relatório HTML completo
./vendor/bin/phpmetrics --report-html=reports/phpmetrics src/

# Abrir no browser
xdg-open reports/phpmetrics/index.html
```

**Métricas prioritárias no relatório:**
- **Cyclomatic Complexity** — identificar métodos > 10
- **Maintainability Index** — abaixo de 65 = código difícil de manter
- **Coupling** — classes com Ce alto
- **Lines of Code por classe** — acima de 300 = candidato a refatoração

### PHP_CodeSniffer (padrão PSR-12)

```bash
composer require --dev squizlabs/php_codesniffer

# Verificar padrão PSR-12
./vendor/bin/phpcs --standard=PSR12 src/

# Corrigir automaticamente o que for possível
./vendor/bin/phpcbf --standard=PSR12 src/
```

### Psalm (análise estática avançada)

```bash
composer require --dev vimeo/psalm
./vendor/bin/psalm --init src/ 4   # nível 4 de rigor
./vendor/bin/psalm
```

---

## 9. Índice de Manutenibilidade

| Faixa | Classificação | Ação |
|-------|--------------|------|
| 85–100 | ✅ Excelente | Manter |
| 65–84 | ⚠️ Aceitável | Monitorar |
| 40–64 | 🟠 Problemático | Priorizar refatoração |
| < 40 | 🔴 Crítico | Refatoração urgente ou reescrita |

---

## 10. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria de qualidade de código e dívida técnica deste projeto PHP/JS.
Reporte em português.

## CODE SMELLS — PHP
- Identifique as 5 maiores classes (God Classes)
- Identifique os 5 métodos mais longos e complexos
- Há Magic Numbers ou Magic Strings espalhados?
- Há Primitive Obsession (dinheiro como float, status como string)?
- Há Shotgun Surgery (mudanças que afetam muitos arquivos)?

## CODE SMELLS — JAVASCRIPT
- Há callback hell (aninhamento > 3 níveis)?
- Uso de var em vez de let/const?
- console.log esquecidos?
- Funções anônimas gigantes em event listeners?

## COMPLEXIDADE
- Quais são os 5 métodos com maior complexidade ciclomática?
- Há métodos com complexidade > 15?
- Qual a complexidade média estimada do projeto?

## DEAD CODE
- Há métodos ou classes claramente sem uso?
- Há arquivos PHP nunca incluídos?
- Há TODOs/FIXMEs catalogados?

## NOMENCLATURA
- Há variáveis com nomes não descritivos?
- Há mistura de idiomas no código?
- Classes têm sufixo de papel claro (Service, Repository, Controller)?

## DIAGNÓSTICO FINAL
- Estime o Índice de Manutenibilidade geral (0-100)
- Liste as 5 maiores dívidas técnicas por impacto
- Priorize: CRÍTICO / IMPORTANTE / COSMÉTICO
- Estime horas de refatoração por grupo
```

---

## 11. Roadmap de Redução de Dívida Técnica

| Sprint | Objetivo | Ações | Métrica de sucesso |
|--------|----------|-------|--------------------|
| 🔴 1 | Eliminar God Classes | Decompor as 3 maiores classes em responsabilidades menores | Nenhuma classe > 300 linhas |
| 🔴 2 | Reduzir complexidade crítica | Refatorar métodos com ciclomática > 15 | Complexidade máxima ≤ 15 |
| 🟠 3 | Eliminar Magic Strings | Substituir por Enums e constantes | PHPStan sem warnings de string mágica |
| 🟠 4 | Value Objects para primitivos | Money, CPF, Email como objetos tipados | Zero `float` para dinheiro |
| 🟠 5 | Dead code | Remover código morto identificado | Zero métodos não referenciados |
| 🟠 6 | JS moderno | Substituir `var`, eliminar callbacks aninhados | ESLint zero erros |
| 🟢 7 | PHPMetrics baseline | Gerar relatório e definir metas de manutenibilidade | MI médio ≥ 65 |
| 🟢 8 | Nomenclatura | Renomear variáveis e funções vagas | Revisão de PR inclui checklist de naming |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III3 — Performance & Observabilidade — Core Web Vitals, OPcache, Logging

# ⚡ Auditoria de Performance & Observabilidade
## Stack: HTML · JavaScript · PHP · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | PHP / MySQL / JS / Infraestrutura |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  FRONTEND       Core Web Vitals, bundle size, cache      │
├─────────────────────────────────────────────────────────┤
│  PHP            Tempo de execução, OPcache, profiling    │
├─────────────────────────────────────────────────────────┤
│  MYSQL          Slow queries, índices, N+1, explain      │
├─────────────────────────────────────────────────────────┤
│  OBSERVABILIDADE  Logging, métricas, alertas, tracing    │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Frontend — Performance de Carregamento

### 2.1 Core Web Vitals

| # | Item de Verificação | Meta | Prioridade | Ferramenta |
|---|---------------------|------|-----------|------------|
| ☐ | LCP (Largest Contentful Paint) | < 2.5s | 🔴 CRÍTICO | Lighthouse / PageSpeed |
| ☐ | FID / INP (Interaction to Next Paint) | < 200ms | 🔴 CRÍTICO | Lighthouse |
| ☐ | CLS (Cumulative Layout Shift) | < 0.1 | 🟠 IMPORTANTE | Lighthouse |
| ☐ | TTFB (Time to First Byte) | < 800ms | 🔴 CRÍTICO | WebPageTest |
| ☐ | FCP (First Contentful Paint) | < 1.8s | 🟠 IMPORTANTE | Lighthouse |

### 2.2 Assets e Carregamento

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | JavaScript minificado em produção | 🔴 CRÍTICO | Webpack / Vite |
| ☐ | CSS minificado em produção | 🔴 CRÍTICO | PostCSS / cssnano |
| ☐ | Imagens comprimidas e em formato moderno (WebP/AVIF) | 🟠 IMPORTANTE | Squoosh / ImageMagick |
| ☐ | Scripts com `defer` ou `async` no `<head>` | 🔴 CRÍTICO | Revisão manual |
| ☐ | CSS crítico inline (above-the-fold) | 🟢 COSMÉTICO | Critical CSS |
| ☐ | Lazy loading em imagens abaixo do fold (`loading="lazy"`) | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Fontes com `font-display: swap` | 🟠 IMPORTANTE | Revisão CSS |
| ☐ | Preload de recursos críticos (`<link rel="preload">`) | 🟢 COSMÉTICO | Lighthouse |

### 2.3 Cache de Assets

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Cache-Control com `max-age` longo para assets estáticos | 🔴 CRÍTICO | `.htaccess` / nginx |
| ☐ | Cache busting via hash no nome do arquivo (`app.a1b2c3.js`) | 🔴 CRÍTICO | Webpack / Vite |
| ☐ | ETag configurado corretamente no servidor | 🟠 IMPORTANTE | Configuração servidor |
| ☐ | Compressão Gzip ou Brotli habilitada no servidor | 🔴 CRÍTICO | `.htaccess` |

**Configuração `.htaccess` para cache e compressão:**

```apache
# Compressão Gzip
<IfModule mod_deflate.c>
    AddOutputFilterByType DEFLATE text/html text/css application/javascript
    AddOutputFilterByType DEFLATE application/json application/xml
</IfModule>

# Cache de longa duração para assets com hash
<IfModule mod_expires.c>
    ExpiresActive On
    ExpiresByType text/css "access plus 1 year"
    ExpiresByType application/javascript "access plus 1 year"
    ExpiresByType image/webp "access plus 1 year"
    ExpiresByType image/png "access plus 6 months"
</IfModule>
```

---

## 3. PHP — Performance de Execução

### 3.1 OPcache

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | OPcache habilitado em produção | 🔴 CRÍTICO | `php -i \| grep opcache` |
| ☐ | `opcache.memory_consumption` ≥ 128MB | 🔴 CRÍTICO | php.ini |
| ☐ | `opcache.max_accelerated_files` ≥ 10000 | 🟠 IMPORTANTE | php.ini |
| ☐ | `opcache.validate_timestamps = 0` em produção | 🟠 IMPORTANTE | php.ini |
| ☐ | `opcache.revalidate_freq = 0` em produção | 🟠 IMPORTANTE | php.ini |

**Configuração php.ini recomendada:**

```ini
opcache.enable=1
opcache.memory_consumption=256
opcache.interned_strings_buffer=16
opcache.max_accelerated_files=20000
opcache.validate_timestamps=0
opcache.revalidate_freq=0
opcache.fast_shutdown=1
```

### 3.2 Código PHP

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem operações bloqueantes no ciclo de request (I/O síncrono pesado) | 🔴 CRÍTICO | Blackfire / Xdebug |
| ☐ | Sem loops fazendo query por iteração (N+1) | 🔴 CRÍTICO | Debugbar / logs |
| ☐ | Autoload PSR-4 com `composer dump-autoload --optimize` em produção | 🟠 IMPORTANTE | Composer |
| ☐ | Uso de generators para processar grandes volumes de dados | 🟠 IMPORTANTE | Revisão manual |
| ☐ | Sem `array_merge()` dentro de loops (use spread operator) | 🟠 IMPORTANTE | PHPStan / revisão |
| ☐ | Conexões PDO reutilizadas — sem reconexão por request | 🔴 CRÍTICO | Revisão manual |
| ☐ | Sessões encerradas cedo (`session_write_close()`) quando não mais necessárias | 🟢 COSMÉTICO | Revisão manual |

### 3.3 Cache de Aplicação

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Cache de dados frequentes com Redis ou Memcached | 🔴 CRÍTICO | Redis / Memcached |
| ☐ | Cache de resultados de queries pesadas | 🔴 CRÍTICO | Redis |
| ☐ | TTL (time-to-live) definido para cada entrada de cache | 🔴 CRÍTICO | Revisão manual |
| ☐ | Estratégia de invalidação de cache definida | 🔴 CRÍTICO | Revisão manual |
| ☐ | Cache de sessão em Redis (não em arquivo) para ambientes com múltiplos servidores | 🟠 IMPORTANTE | php.ini |

**Exemplo de cache com Redis em PHP:**

```php
// Cache-aside pattern
class UserService
{
    public function findById(int $id): User
    {
        $cacheKey = "user:{$id}";

        $cached = $this->cache->get($cacheKey);
        if ($cached !== null) {
            return unserialize($cached);
        }

        $user = $this->repository->findById($id);

        $this->cache->set($cacheKey, serialize($user), ttl: 300); // 5 min

        return $user;
    }
}
```

---

## 4. MySQL — Performance de Banco de Dados

### 4.1 Slow Query Log

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Slow query log habilitado em produção | 🔴 CRÍTICO | my.cnf |
| ☐ | `long_query_time` configurado (recomendado: 1s) | 🔴 CRÍTICO | my.cnf |
| ☐ | Slow queries analisadas semanalmente | 🔴 CRÍTICO | mysqldumpslow / pt-query-digest |
| ☐ | Zero queries sem índice em produção (`log_queries_not_using_indexes`) | 🟠 IMPORTANTE | my.cnf |

**Configuração my.cnf:**

```ini
slow_query_log = 1
slow_query_log_file = /var/log/mysql/slow.log
long_query_time = 1
log_queries_not_using_indexes = 1
```

### 4.2 Índices e EXPLAIN

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `EXPLAIN` executado em todas as queries com mais de 1000 execuções/dia | 🔴 CRÍTICO | MySQL Workbench |
| ☐ | Nenhum `type: ALL` (full table scan) em tabelas com > 1000 registros | 🔴 CRÍTICO | EXPLAIN |
| ☐ | Índices compostos criados para queries com múltiplos campos no WHERE | 🟠 IMPORTANTE | EXPLAIN |
| ☐ | Índices não utilizados identificados e removidos | 🟢 COSMÉTICO | `sys.schema_unused_indexes` |
| ☐ | Colunas de join indexadas em ambas as tabelas | 🔴 CRÍTICO | EXPLAIN |

### 4.3 N+1 Queries

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Sem loop PHP executando query por iteração | 🔴 CRÍTICO | Debugbar / logs |
| ☐ | Listagens com relacionamentos carregadas via JOIN ou IN() | 🔴 CRÍTICO | Revisão manual |
| ☐ | Contagens feitas via `COUNT()` no banco — não via `count($array)` após busca total | 🟠 IMPORTANTE | Revisão manual |

**Exemplo N+1 vs correto:**

```php
// ❌ N+1 — 1 query para lista + 1 por usuário
$users = $db->query("SELECT id FROM users")->fetchAll();
foreach ($users as $user) {
    $orders = $db->query("SELECT * FROM orders WHERE user_id = {$user['id']}")->fetchAll();
}

// ✅ CORRETO — 1 única query com JOIN
$users = $db->query("
    SELECT u.id, u.name, COUNT(o.id) as order_count
    FROM users u
    LEFT JOIN orders o ON o.user_id = u.id
    GROUP BY u.id
")->fetchAll();
```

### 4.4 Configuração MySQL

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `innodb_buffer_pool_size` = 70-80% da RAM disponível | 🔴 CRÍTICO | my.cnf |
| ☐ | `query_cache_type = 0` (query cache depreciado no MySQL 8) | 🟠 IMPORTANTE | my.cnf |
| ☐ | `max_connections` adequado à carga esperada | 🟠 IMPORTANTE | my.cnf |
| ☐ | Connection pooling configurado (PgBouncer equivalente ou PDO persistente) | 🟠 IMPORTANTE | Configuração |

---

## 5. Observabilidade

### 5.1 Logging Estruturado

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Logs em formato estruturado JSON — não strings livres | 🔴 CRÍTICO | Monolog |
| ☐ | Níveis de log corretos: DEBUG, INFO, WARNING, ERROR, CRITICAL | 🔴 CRÍTICO | Monolog |
| ☐ | Logs de erro com stack trace completo | 🔴 CRÍTICO | Monolog |
| ☐ | Request ID único propagado em todos os logs do mesmo request | 🟠 IMPORTANTE | Middleware |
| ☐ | Dados pessoais nunca logados (CPF, senha, token, cartão) — LGPD Art. 46 | 🔴 CRÍTICO | Revisão manual |
| ☐ | Logs rotacionados e com retenção definida | 🟠 IMPORTANTE | logrotate |
| ☐ | Logs centralizados (Elasticsearch / Loki / Papertrail) | 🟠 IMPORTANTE | Stack ELK / Grafana |

**Configuração Monolog:**

```php
use Monolog\Logger;
use Monolog\Handler\StreamHandler;
use Monolog\Formatter\JsonFormatter;
use Monolog\Processor\UidProcessor;

$logger = new Logger('app');

$handler = new StreamHandler('/var/log/app/app.log', Logger::DEBUG);
$handler->setFormatter(new JsonFormatter());

$logger->pushProcessor(new UidProcessor()); // request ID único
$logger->pushHandler($handler);

// ✅ Log estruturado com contexto
$logger->error('Falha ao processar pagamento', [
    'order_id'   => $orderId,
    'gateway'    => 'pagarme',
    'error_code' => $e->getCode(),
    // NUNCA logar: CPF, cartão, senha, token
]);
```

### 5.2 Métricas de Aplicação

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Tempo de resposta de cada endpoint monitorado | 🔴 CRÍTICO | Prometheus / Datadog |
| ☐ | Taxa de erros HTTP (4xx, 5xx) monitorada com alerta | 🔴 CRÍTICO | Prometheus / Grafana |
| ☐ | Uso de memória PHP monitorado (`memory_get_peak_usage()`) | 🟠 IMPORTANTE | APM |
| ☐ | Fila de jobs monitorada (tamanho, tempo de processamento) | 🟠 IMPORTANTE | Horizon / custom |
| ☐ | Dashboard de saúde da aplicação disponível | 🟠 IMPORTANTE | Grafana |

### 5.3 Health Checks

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Endpoint `/health` retornando status da aplicação | 🔴 CRÍTICO | Implementação custom |
| ☐ | Health check verifica conexão com banco de dados | 🔴 CRÍTICO | Implementação custom |
| ☐ | Health check verifica conexão com cache (Redis) | 🟠 IMPORTANTE | Implementação custom |
| ☐ | Uptime monitorado por serviço externo | 🔴 CRÍTICO | UptimeRobot / Better Uptime |

**Exemplo de endpoint `/health`:**

```php
// GET /health
$status = [
    'status'    => 'ok',
    'timestamp' => date('c'),
    'checks'    => [
        'database' => checkDatabase(),   // tenta SELECT 1
        'cache'    => checkRedis(),      // tenta PING
        'disk'     => checkDiskSpace(),  // verifica espaço livre
    ]
];

$allOk = !in_array('fail', array_column($status['checks'], 'status'));
http_response_code($allOk ? 200 : 503);
echo json_encode($status);
```

### 5.4 Alertas

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Alerta para taxa de erro > 1% em janela de 5 minutos | 🔴 CRÍTICO | Grafana / PagerDuty |
| ☐ | Alerta para tempo de resposta p95 > 2s | 🔴 CRÍTICO | Grafana |
| ☐ | Alerta para uso de memória > 85% | 🔴 CRÍTICO | Grafana |
| ☐ | Alerta para slow queries acima de threshold | 🟠 IMPORTANTE | MySQL + Grafana |
| ☐ | Notificação de deploy registrada no canal de alertas | 🟢 COSMÉTICO | Slack / Teams |

---

## 6. Profiling PHP

### Xdebug (desenvolvimento)

```bash
# Instalar
pecl install xdebug

# php.ini para profiling
xdebug.mode=profile
xdebug.output_dir=/tmp/xdebug
xdebug.profiler_output_name=cachegrind.out.%p

# Analisar com KCachegrind (Linux) ou QCachegrind (Mac)
```

### Blackfire (produção / staging)

```bash
# Instalar agent
curl -s https://packagecloud.io/install/repositories/blackfire/stable/script.deb.sh | sudo bash
sudo apt install blackfire

# Profile de um endpoint
blackfire curl https://sua-app.com/endpoint-lento
```

---

## 7. Roadmap de Performance

| Sprint | Objetivo | Ações | Ganho esperado |
|--------|----------|-------|----------------|
| 🔴 1 | Eliminar N+1 queries | Auditar com Debugbar, refatorar para JOINs | -70% queries por request |
| 🔴 2 | Habilitar OPcache e compressão | Configurar php.ini e .htaccess | -40% tempo de execução PHP |
| 🔴 3 | Slow query log + EXPLAIN | Indexar colunas críticas | -60% tempo de queries |
| 🟠 4 | Implementar cache Redis | Cache de queries pesadas e sessões | -50% carga no banco |
| 🟠 5 | Minificar e versionar assets | Webpack/Vite com hash | -60% tamanho de transfer |
| 🟠 6 | Logging estruturado + alertas | Monolog + Grafana | Visibilidade de produção |
| 🟢 7 | Core Web Vitals | Lazy load, preload, fontes otimizadas | LCP < 2.5s |
| 🟢 8 | Profiling com Blackfire | Identificar gargalos residuais | Otimizações cirúrgicas |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III4 — DevOps & Deploy — Docker, CI/CD, Zero-Downtime

# 🚀 Auditoria de DevOps & Deploy
## Stack: PHP · MySQL · HTML · JavaScript

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Infraestrutura / CI/CD / Deploy |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  AMBIENTES       dev / staging / produção separados      │
├─────────────────────────────────────────────────────────┤
│  CI/CD           Build, test, lint, deploy automatizado  │
├─────────────────────────────────────────────────────────┤
│  CONTAINERIZAÇÃO Docker, imagens, compose, orquestração  │
├─────────────────────────────────────────────────────────┤
│  ROLLBACK        Zero-downtime, blue-green, reversão     │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Gestão de Ambientes

### 2.1 Separação de ambientes

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Ambientes distintos: desenvolvimento, staging e produção | 🔴 CRÍTICO |
| ☐ | Banco de dados separado por ambiente | 🔴 CRÍTICO |
| ☐ | Credenciais distintas por ambiente — nunca as mesmas | 🔴 CRÍTICO |
| ☐ | URL de produção nunca apontada em ambiente de dev | 🔴 CRÍTICO |
| ☐ | Staging espelhando produção em configuração e volume de dados | 🟠 IMPORTANTE |

### 2.2 Variáveis de Ambiente

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `.env` para cada ambiente — nunca hardcoded no código | 🔴 CRÍTICO |
| ☐ | `.env` no `.gitignore` — nunca versionado | 🔴 CRÍTICO |
| ☐ | `.env.example` versionado com todas as chaves (sem valores) | 🔴 CRÍTICO |
| ☐ | Variáveis de ambiente validadas na inicialização da aplicação | 🟠 IMPORTANTE |
| ☐ | Segredos de produção em gerenciador de segredos (Vault, AWS Secrets, etc.) | 🟠 IMPORTANTE |

**Arquivo `.env.example` obrigatório:**

```dotenv
# Aplicação
APP_ENV=production
APP_DEBUG=false
APP_URL=https://sua-app.com
APP_KEY=

# Banco de dados
DB_HOST=
DB_PORT=3306
DB_DATABASE=
DB_USERNAME=
DB_PASSWORD=

# Cache
REDIS_HOST=
REDIS_PORT=6379
REDIS_PASSWORD=

# E-mail
MAIL_HOST=
MAIL_PORT=
MAIL_USERNAME=
MAIL_PASSWORD=
MAIL_FROM_ADDRESS=
```

---

## 3. Containerização com Docker

### 3.1 Checklist Docker

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `Dockerfile` para a aplicação PHP | 🔴 CRÍTICO |
| ☐ | `docker-compose.yml` para ambiente de desenvolvimento | 🔴 CRÍTICO |
| ☐ | `docker-compose.prod.yml` para produção | 🟠 IMPORTANTE |
| ☐ | Imagem PHP baseada em versão específica — nunca `php:latest` | 🔴 CRÍTICO |
| ☐ | Imagem com usuário não-root em produção | 🔴 CRÍTICO |
| ☐ | `.dockerignore` configurado (excluir `vendor/`, `node_modules/`, `.git/`) | 🟠 IMPORTANTE |
| ☐ | Volumes para persistência de dados do MySQL | 🔴 CRÍTICO |
| ☐ | Health check definido no `docker-compose.yml` | 🟠 IMPORTANTE |

**Estrutura recomendada:**

```dockerfile
# Dockerfile
FROM php:8.3-fpm-alpine

# Instalar extensões necessárias
RUN docker-php-ext-install pdo pdo_mysql opcache

# Copiar configuração OPcache otimizada
COPY docker/php/opcache.ini /usr/local/etc/php/conf.d/opcache.ini

# Usuário não-root
RUN adduser -D -u 1000 appuser
USER appuser

WORKDIR /var/www/html
COPY --chown=appuser:appuser . .

RUN composer install --no-dev --optimize-autoloader
```

```yaml
# docker-compose.yml (desenvolvimento)
services:
  app:
    build: .
    volumes:
      - .:/var/www/html
    depends_on:
      db:
        condition: service_healthy
    environment:
      - APP_ENV=development

  db:
    image: mysql:8.0
    volumes:
      - db_data:/var/lib/mysql
    environment:
      MYSQL_ROOT_PASSWORD: ${DB_ROOT_PASSWORD}
      MYSQL_DATABASE: ${DB_DATABASE}
    healthcheck:
      test: ["CMD", "mysqladmin", "ping", "-h", "localhost"]
      interval: 10s
      retries: 5

  redis:
    image: redis:7-alpine

  nginx:
    image: nginx:alpine
    ports:
      - "80:80"
    volumes:
      - ./docker/nginx/default.conf:/etc/nginx/conf.d/default.conf

volumes:
  db_data:
```

---

## 4. Pipeline CI/CD Completo

### 4.1 Etapas do pipeline

| Etapa | Quando | Ferramenta | Bloqueia? |
|-------|--------|-----------|-----------|
| Lint PHP | Push / PR | PHPStan + PHPCS | Sim |
| Lint JS | Push / PR | ESLint | Sim |
| Testes unitários | Push / PR | PHPUnit | Sim |
| Testes integração | Push / PR | PHPUnit | Sim |
| Auditoria deps | Push / PR | composer audit + npm audit | Sim |
| Scan de segredos | Push / PR | TruffleHog | Sim |
| Build Docker | Merge main | Docker | Sim |
| Deploy staging | Merge main | Deploy script | Sim |
| Testes E2E | Após staging | Playwright / Cypress | Sim |
| OWASP ZAP | Após staging | ZAP Baseline | Sim |
| Deploy produção | Manual / tag | Deploy script | — |

### 4.2 Checklist CI/CD

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Deploy automatizado para staging a cada merge na branch main | 🔴 CRÍTICO |
| ☐ | Deploy para produção manual ou via tag de versão | 🔴 CRÍTICO |
| ☐ | Nenhum deploy manual via FTP ou SSH direto | 🔴 CRÍTICO |
| ☐ | Pipeline falha rápido (lint antes de testes, testes antes de build) | 🟠 IMPORTANTE |
| ☐ | Artefatos de build versionados e rastreáveis | 🟠 IMPORTANTE |
| ☐ | Notificação de deploy no canal da equipe (Slack/Teams) | 🟢 COSMÉTICO |
| ☐ | Tempo total do pipeline < 10 minutos | 🟠 IMPORTANTE |

**GitHub Actions — pipeline completo:**

```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  quality:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Setup PHP
        uses: shivammathur/setup-php@v2
        with:
          php-version: '8.3'
          extensions: pdo, pdo_mysql, redis
          coverage: xdebug

      - name: Install PHP dependencies
        run: composer install --no-progress --prefer-dist

      - name: PHPStan
        run: ./vendor/bin/phpstan analyse --level=8

      - name: PHPUnit com cobertura
        run: ./vendor/bin/phpunit --coverage-clover coverage.xml

      - name: Verificar cobertura mínima (60%)
        run: |
          COVERAGE=$(php -r "
            \$xml = simplexml_load_file('coverage.xml');
            \$metrics = \$xml->project->metrics;
            echo round((\$metrics['coveredstatements'] / \$metrics['statements']) * 100);
          ")
          if [ "$COVERAGE" -lt 60 ]; then
            echo "Cobertura $COVERAGE% abaixo do mínimo de 60%"
            exit 1
          fi

      - name: Composer audit
        run: composer audit

  lint-js:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-node@v4
        with:
          node-version: '20'
      - run: npm ci
      - run: npx eslint . --ext .js,.ts
      - run: npm audit --audit-level=high

  deploy-staging:
    runs-on: ubuntu-latest
    needs: [quality, lint-js]
    if: github.ref == 'refs/heads/main'
    steps:
      - uses: actions/checkout@v4
      - name: Build e push imagem Docker
        run: |
          docker build -t app:${{ github.sha }} .
          docker tag app:${{ github.sha }} registry/app:staging
          docker push registry/app:staging
      - name: Deploy para staging
        run: ssh deploy@staging "cd /app && docker-compose pull && docker-compose up -d"

  zap-staging:
    runs-on: ubuntu-latest
    needs: [deploy-staging]
    steps:
      - name: ZAP Baseline Scan
        uses: zaproxy/action-baseline@v0.13.0
        with:
          target: 'https://staging.sua-app.com'
```

---

## 5. Estratégia de Deploy

### 5.1 Zero-Downtime Deploy

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Deploy não derruba o serviço durante a atualização | 🔴 CRÍTICO |
| ☐ | Migrations de banco compatíveis com versão anterior (backward compatible) | 🔴 CRÍTICO |
| ☐ | Health check do load balancer remove instância antes de atualizar | 🟠 IMPORTANTE |
| ☐ | Estratégia blue-green ou rolling update implementada | 🟠 IMPORTANTE |

### 5.2 Rollback

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Rollback de aplicação executável em menos de 5 minutos | 🔴 CRÍTICO |
| ☐ | Migrations com `down()` para reverter alterações de schema | 🔴 CRÍTICO |
| ☐ | Versões de imagem Docker mantidas (não sobrescrever `:latest` sem tag versionada) | 🔴 CRÍTICO |
| ☐ | Procedure de rollback documentada e testada | 🟠 IMPORTANTE |
| ☐ | Backup automático do banco antes de cada deploy | 🟠 IMPORTANTE |

**Script de deploy com rollback:**

```bash
#!/bin/bash
# deploy.sh

CURRENT_VERSION=$(cat .current-version)
NEW_VERSION=$1

echo "Fazendo backup do banco..."
mysqldump -u $DB_USER -p$DB_PASS $DB_NAME > backup_pre_deploy_$(date +%Y%m%d_%H%M%S).sql

echo "Deployando versão $NEW_VERSION..."
docker pull registry/app:$NEW_VERSION

if ! docker-compose up -d --no-deps app; then
    echo "FALHA no deploy. Revertendo para $CURRENT_VERSION..."
    docker pull registry/app:$CURRENT_VERSION
    docker-compose up -d --no-deps app
    exit 1
fi

echo $NEW_VERSION > .current-version
echo "Deploy $NEW_VERSION concluído com sucesso."
```

---

## 6. Monitoramento de Infraestrutura

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | Uso de CPU monitorado com alerta > 80% | 🔴 CRÍTICO | Grafana / Datadog |
| ☐ | Uso de memória RAM monitorado com alerta > 85% | 🔴 CRÍTICO | Grafana |
| ☐ | Espaço em disco monitorado com alerta > 80% | 🔴 CRÍTICO | Grafana |
| ☐ | Uptime monitorado externamente | 🔴 CRÍTICO | UptimeRobot |
| ☐ | Certificado SSL monitorado — alerta 30 dias antes do vencimento | 🔴 CRÍTICO | UptimeRobot / Certbot |
| ☐ | Logs do sistema coletados e retidos por 90 dias | 🟠 IMPORTANTE | ELK / Loki |

---

## 7. Roadmap DevOps

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Eliminar deploy manual | Configurar GitHub Actions com deploy para staging | Zero deploy via FTP/SSH manual |
| 🔴 2 | Containerizar com Docker | Dockerfile + docker-compose para dev e prod | Ambiente reproduzível em 1 comando |
| 🔴 3 | Separar ambientes | Staging independente com banco próprio | Staging ≠ produção em tudo |
| 🟠 4 | Pipeline completo | Lint + testes + cobertura + ZAP no CI | PR bloqueado sem passar pipeline |
| 🟠 5 | Zero-downtime | Rolling deploy ou blue-green | Deploy sem interrupção do serviço |
| 🟠 6 | Monitoramento | Grafana + alertas de CPU, memória, uptime | Alertas chegam antes do usuário reclamar |
| 🟢 7 | Rollback automatizado | Script de rollback testado e documentado | Rollback < 5 minutos |
| 🟢 8 | Vault de segredos | Secrets Manager para credenciais de produção | Zero `.env` manual em produção |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III5 — API Design REST — Verbos, Status Codes, Auth, OpenAPI

# 🔌 Auditoria de API Design — REST
## Stack: PHP · MySQL · JavaScript (cliente)

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | PHP API / JS client |

---

## 1. Visão Geral

```
┌─────────────────────────────────────────────────────────┐
│  CONTRATO       Verbos, status codes, versionamento      │
├─────────────────────────────────────────────────────────┤
│  PAYLOAD        Request/Response, serialização, erros    │
├─────────────────────────────────────────────────────────┤
│  SEGURANÇA      Auth, rate limiting, CORS, validação     │
├─────────────────────────────────────────────────────────┤
│  DOCUMENTAÇÃO   OpenAPI/Swagger, contratos, exemplos     │
└─────────────────────────────────────────────────────────┘
```

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

## 2. Contrato REST — Verbos e Rotas

### 2.1 Verbos HTTP

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | `GET` apenas para leitura — nunca altera estado | 🔴 CRÍTICO |
| ☐ | `POST` para criação de recursos | 🔴 CRÍTICO |
| ☐ | `PUT` para substituição total do recurso | 🔴 CRÍTICO |
| ☐ | `PATCH` para atualização parcial do recurso | 🟠 IMPORTANTE |
| ☐ | `DELETE` para remoção — retorna `204 No Content` | 🔴 CRÍTICO |
| ☐ | Sem ações em URLs: `/getUser`, `/deleteOrder`, `/doLogin` | 🔴 CRÍTICO |
| ☐ | Recursos no plural: `/users`, `/orders`, `/products` | 🟠 IMPORTANTE |

**Padrão correto de rotas:**

```
✅ GET    /api/v1/users           → lista usuários
✅ POST   /api/v1/users           → cria usuário
✅ GET    /api/v1/users/{id}      → busca usuário
✅ PUT    /api/v1/users/{id}      → atualiza usuário completo
✅ PATCH  /api/v1/users/{id}      → atualiza campos específicos
✅ DELETE /api/v1/users/{id}      → remove usuário

❌ GET    /api/getUser?id=1
❌ POST   /api/deleteOrder
❌ GET    /api/users/doActivate
```

### 2.2 Status Codes

| # | Situação | Code correto | Anti-padrão comum |
|---|----------|-------------|-------------------|
| ☐ | Recurso criado com sucesso | `201 Created` | Usar `200` em POST |
| ☐ | Operação sem body de retorno | `204 No Content` | Retornar `200` com `{}` |
| ☐ | Recurso não encontrado | `404 Not Found` | Retornar `200` com erro no body |
| ☐ | Erro de validação de input | `422 Unprocessable Entity` | Usar `400` genérico |
| ☐ | Não autenticado | `401 Unauthorized` | Usar `403` |
| ☐ | Autenticado mas sem permissão | `403 Forbidden` | Usar `401` |
| ☐ | Rate limit excedido | `429 Too Many Requests` | Usar `400` |
| ☐ | Erro interno do servidor | `500 Internal Server Error` | Retornar `200` com `success: false` |

### 2.3 Versionamento

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | API versionada desde o início (`/api/v1/`) | 🔴 CRÍTICO |
| ☐ | Versão na URL — não em header (mais simples para o contexto PHP) | 🟠 IMPORTANTE |
| ☐ | Versão antiga mantida por período de deprecação antes de remoção | 🟠 IMPORTANTE |
| ☐ | Header `Deprecation` retornado em endpoints obsoletos | 🟢 COSMÉTICO |

---

## 3. Payload — Request e Response

### 3.1 Formato de Response

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Content-Type `application/json` em todos os responses de API | 🔴 CRÍTICO |
| ☐ | Estrutura de resposta consistente em todos os endpoints | 🔴 CRÍTICO |
| ☐ | Campos em `snake_case` ou `camelCase` — escolher um e manter | 🟠 IMPORTANTE |
| ☐ | Datas em formato ISO 8601 (`2024-01-15T14:30:00Z`) | 🟠 IMPORTANTE |
| ☐ | IDs numéricos enviados como string em JSON (evitar perda de precisão JS) | 🟠 IMPORTANTE |
| ☐ | Sem exposição de campos internos: `password_hash`, `deleted_at`, IDs internos | 🔴 CRÍTICO |

**Estrutura de resposta padronizada:**

```json
// ✅ Sucesso
{
  "data": {
    "id": "123",
    "name": "João Silva",
    "email": "joao@email.com",
    "created_at": "2024-01-15T14:30:00Z"
  },
  "meta": {
    "request_id": "a1b2c3d4"
  }
}

// ✅ Lista paginada
{
  "data": [ {...}, {...} ],
  "meta": {
    "total": 150,
    "per_page": 20,
    "current_page": 1,
    "last_page": 8
  }
}

// ✅ Erro
{
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Os dados enviados são inválidos.",
    "details": {
      "email": ["O e-mail já está em uso."],
      "password": ["A senha deve ter no mínimo 8 caracteres."]
    }
  },
  "meta": {
    "request_id": "a1b2c3d4"
  }
}

// ❌ Padrão inconsistente — nunca misturar
{ "success": false, "msg": "error", "data": null }
```

### 3.2 Paginação

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Toda listagem com paginação — nunca retornar todos os registros | 🔴 CRÍTICO |
| ☐ | Parâmetros `page` e `per_page` (ou `limit`/`offset`) padronizados | 🔴 CRÍTICO |
| ☐ | `per_page` com limite máximo definido (ex: max 100) | 🔴 CRÍTICO |
| ☐ | Metadados de paginação no response (`total`, `last_page`) | 🟠 IMPORTANTE |
| ☐ | Links HATEOAS para próxima/anterior página | 🟢 COSMÉTICO |

### 3.3 Filtros e Ordenação

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Filtros via query string: `GET /users?status=active&role=admin` | 🟠 IMPORTANTE |
| ☐ | Ordenação via `sort` e `order`: `?sort=created_at&order=desc` | 🟠 IMPORTANTE |
| ☐ | Campos permitidos para filtro/ordenação validados no servidor | 🔴 CRÍTICO |
| ☐ | SQL Injection impossível via parâmetros de filtro (prepared statements) | 🔴 CRÍTICO |

---

## 4. Segurança da API

### 4.1 Autenticação

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Endpoints de API autenticados via JWT ou API Key — nunca sessão de cookie | 🔴 CRÍTICO |
| ☐ | JWT com expiração curta (≤ 1h para access token) | 🔴 CRÍTICO |
| ☐ | Refresh token com rotação (invalidar após uso) | 🟠 IMPORTANTE |
| ☐ | API Keys com escopo definido (read-only vs read-write) | 🟠 IMPORTANTE |
| ☐ | Token nunca logado ou exposto em URL (`?token=xxx`) | 🔴 CRÍTICO |
| ☐ | Header `Authorization: Bearer {token}` — nunca query string | 🔴 CRÍTICO |

### 4.2 Rate Limiting

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Rate limiting implementado em todos os endpoints públicos | 🔴 CRÍTICO |
| ☐ | Rate limiting mais restrito em endpoints de autenticação | 🔴 CRÍTICO |
| ☐ | Headers `X-RateLimit-Limit`, `X-RateLimit-Remaining`, `Retry-After` no response | 🟠 IMPORTANTE |
| ☐ | Rate limiting por IP e por usuário autenticado | 🟠 IMPORTANTE |

```php
// Exemplo de rate limiting simples com Redis
class RateLimiter
{
    public function check(string $key, int $maxAttempts, int $decaySeconds): bool
    {
        $attempts = (int) $this->redis->get($key) ?: 0;

        if ($attempts >= $maxAttempts) {
            return false; // bloqueado
        }

        $this->redis->incr($key);
        $this->redis->expire($key, $decaySeconds);

        return true;
    }
}
```

### 4.3 CORS

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | CORS configurado explicitamente — nunca `Access-Control-Allow-Origin: *` em API autenticada | 🔴 CRÍTICO |
| ☐ | Lista de origens permitidas definida e restrita | 🔴 CRÍTICO |
| ☐ | Preflight `OPTIONS` tratado corretamente | 🟠 IMPORTANTE |
| ☐ | `Access-Control-Allow-Credentials: true` apenas quando necessário | 🟠 IMPORTANTE |

---

## 5. Validação de Input na API

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Todo campo do request validado antes de processar | 🔴 CRÍTICO |
| ☐ | Mensagens de erro de validação em português e claras para o cliente | 🟠 IMPORTANTE |
| ☐ | Tipos de dados validados (int, string, email, date) | 🔴 CRÍTICO |
| ☐ | Tamanho máximo de campos de texto validado | 🔴 CRÍTICO |
| ☐ | Campos inesperados ignorados ou rejeitados (no extra fields) | 🟠 IMPORTANTE |
| ☐ | Upload de arquivos com validação de tipo MIME e tamanho na API | 🔴 CRÍTICO |

---

## 6. Documentação OpenAPI / Swagger

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Documentação OpenAPI 3.0 gerada e atualizada | 🔴 CRÍTICO |
| ☐ | Todos os endpoints documentados com exemplos de request e response | 🔴 CRÍTICO |
| ☐ | Códigos de erro documentados por endpoint | 🟠 IMPORTANTE |
| ☐ | Swagger UI disponível em `/api/docs` (apenas em não-produção) | 🟠 IMPORTANTE |
| ☐ | Schemas reutilizáveis definidos em `components/schemas` | 🟢 COSMÉTICO |

**Exemplo de anotação OpenAPI em PHP:**

```php
/**
 * @OA\Post(
 *     path="/api/v1/users",
 *     summary="Cria um novo usuário",
 *     tags={"Usuários"},
 *     @OA\RequestBody(
 *         required=true,
 *         @OA\JsonContent(
 *             required={"name","email","password"},
 *             @OA\Property(property="name", type="string", example="João Silva"),
 *             @OA\Property(property="email", type="string", format="email"),
 *             @OA\Property(property="password", type="string", minLength=8)
 *         )
 *     ),
 *     @OA\Response(response=201, description="Usuário criado"),
 *     @OA\Response(response=422, description="Erro de validação")
 * )
 */
public function store(Request $request): JsonResponse { ... }
```

---

## 7. Prompt de Auditoria para Claude Opus

```
Faça uma auditoria completa do design da API REST deste projeto PHP.
Reporte em português.

## CONTRATO REST
- Os verbos HTTP estão sendo usados corretamente?
- Os status codes estão corretos para cada situação?
- As URLs seguem o padrão REST (substantivos, plural, sem verbos)?
- A API está versionada?

## PAYLOAD
- A estrutura de resposta é consistente entre endpoints?
- Listagens têm paginação com metadados?
- Datas estão em ISO 8601?
- Campos sensíveis estão sendo expostos no response?

## SEGURANÇA
- Como a autenticação está implementada (JWT, API Key, sessão)?
- Existe rate limiting nos endpoints?
- CORS está configurado corretamente?
- Todo input está sendo validado antes de processar?

## DOCUMENTAÇÃO
- Existe documentação OpenAPI/Swagger?
- Os endpoints estão documentados com exemplos?

## DIAGNÓSTICO FINAL
- Liste as 5 piores violações de design REST
- Identifique endpoints sem autenticação que deveriam ter
- Avalie consistência do formato de response
- Priorize: CRÍTICO / IMPORTANTE / COSMÉTICO
```

---

## 8. Roadmap de Adequação de API

| Sprint | Objetivo | Ações | Critério |
|--------|----------|-------|----------|
| 🔴 1 | Status codes corretos | Mapear todos os endpoints e corrigir codes | Zero `200` para erros |
| 🔴 2 | Padronizar response | Estrutura única `{data, meta}` em todos endpoints | Zero respostas inconsistentes |
| 🔴 3 | Rate limiting | Implementar com Redis por IP e usuário | Endpoints de login protegidos |
| 🟠 4 | Versionamento | Prefixar rotas com `/api/v1/` | Deploy sem quebrar clientes |
| 🟠 5 | Paginação universal | Todas as listagens com `page` e `per_page` | Zero listagens sem limit |
| 🟠 6 | Documentação OpenAPI | Gerar e publicar Swagger UI em staging | 100% endpoints documentados |
| 🟢 7 | CORS refinado | Whitelist de origens por ambiente | Zero `Allow-Origin: *` |
| 🟢 8 | HATEOAS parcial | Links de paginação no meta | Clientes navegam sem construir URLs |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III6 — Acessibilidade, SEO, i18n, Dependências, Documentação

# 🌐 Auditoria de Acessibilidade, SEO Técnico, Dependências & Documentação
## Stack: HTML · JavaScript · PHP · MySQL

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Uso interno | Front-end / Infraestrutura / Processo |

---

## 1. Acessibilidade — WCAG 2.1

> Conformidade mínima recomendada: **nível AA**. Obrigatório para contratos com setor público (Lei Brasileira de Inclusão — LBI, Art. 63).

> **Legenda:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

### 1.1 Estrutura Semântica

| # | Item de Verificação | Nível WCAG | Prioridade | Ferramenta |
|---|---------------------|-----------|-----------|------------|
| ☐ | Hierarquia de headings correta (`h1` → `h2` → `h3`) — sem pular níveis | AA 1.3.1 | 🔴 CRÍTICO | axe / WAVE |
| ☐ | Apenas um `<h1>` por página | AA 1.3.1 | 🔴 CRÍTICO | axe |
| ☐ | Landmarks HTML5 presentes: `<header>`, `<nav>`, `<main>`, `<footer>` | AA 1.3.1 | 🟠 IMPORTANTE | axe |
| ☐ | Listas marcadas com `<ul>` ou `<ol>` — nunca com `<div>` + `•` | AA 1.3.1 | 🟠 IMPORTANTE | axe |
| ☐ | Tabelas com `<thead>`, `<th scope>` e `<caption>` | AA 1.3.1 | 🟠 IMPORTANTE | axe |

### 1.2 Imagens e Mídia

| # | Item de Verificação | Nível WCAG | Prioridade | Ferramenta |
|---|---------------------|-----------|-----------|------------|
| ☐ | Todas as imagens informativas com `alt` descritivo | A 1.1.1 | 🔴 CRÍTICO | axe / Lighthouse |
| ☐ | Imagens decorativas com `alt=""` (vazio) | A 1.1.1 | 🔴 CRÍTICO | axe |
| ☐ | Ícones SVG com `aria-label` ou `title` | A 1.1.1 | 🟠 IMPORTANTE | axe |
| ☐ | Vídeos com legendas (closed captions) | AA 1.2.2 | 🟠 IMPORTANTE | Manual |
| ☐ | Sem conteúdo que pisca mais de 3x por segundo | A 2.3.1 | 🔴 CRÍTICO | Manual |

### 1.3 Formulários

| # | Item de Verificação | Nível WCAG | Prioridade | Ferramenta |
|---|---------------------|-----------|-----------|------------|
| ☐ | Todo campo de formulário com `<label>` associado via `for`/`id` | A 1.3.1 | 🔴 CRÍTICO | axe |
| ☐ | Campos obrigatórios marcados com `required` e `aria-required="true"` | AA 3.3.2 | 🔴 CRÍTICO | axe |
| ☐ | Mensagens de erro associadas ao campo via `aria-describedby` | AA 3.3.1 | 🔴 CRÍTICO | axe |
| ☐ | Autocomplete configurado em campos de dados pessoais | AA 1.3.5 | 🟠 IMPORTANTE | axe |
| ☐ | Sem dependência exclusiva de placeholder como label | AA 3.3.2 | 🔴 CRÍTICO | axe |

### 1.4 Navegação por Teclado

| # | Item de Verificação | Nível WCAG | Prioridade | Ferramenta |
|---|---------------------|-----------|-----------|------------|
| ☐ | Todos os elementos interativos acessíveis via Tab | A 2.1.1 | 🔴 CRÍTICO | Teste manual |
| ☐ | Foco visível em todos os elementos interativos (sem `outline: none` sem alternativa) | AA 2.4.7 | 🔴 CRÍTICO | Teste manual |
| ☐ | Ordem de foco lógica e coerente com layout visual | A 2.4.3 | 🟠 IMPORTANTE | Teste manual |
| ☐ | Modais/dialogs com foco preso dentro enquanto abertos | A 2.1.1 | 🔴 CRÍTICO | Teste manual |
| ☐ | Skip link ("Ir para o conteúdo principal") no início da página | A 2.4.1 | 🟠 IMPORTANTE | Inspeção |

### 1.5 Contraste e Visual

| # | Item de Verificação | Meta | Prioridade | Ferramenta |
|---|---------------------|------|-----------|------------|
| ☐ | Contraste de texto normal vs fundo | ≥ 4.5:1 | 🔴 CRÍTICO | Colour Contrast Analyser |
| ☐ | Contraste de texto grande (18pt+) vs fundo | ≥ 3:1 | 🔴 CRÍTICO | Colour Contrast Analyser |
| ☐ | Informação não transmitida apenas por cor | AA 1.4.1 | 🔴 CRÍTICO | Manual |
| ☐ | Texto redimensionável até 200% sem perda de conteúdo | AA 1.4.4 | 🟠 IMPORTANTE | Zoom do browser |

### 1.6 ARIA

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `aria-*` apenas onde HTML semântico não é suficiente | 🟠 IMPORTANTE | axe |
| ☐ | Roles ARIA válidos e nos elementos corretos | 🔴 CRÍTICO | axe |
| ☐ | `aria-live` em regiões com conteúdo dinâmico (alertas, loaders) | 🟠 IMPORTANTE | axe |
| ☐ | Sem ARIA redundante (`<button role="button">`) | 🟢 COSMÉTICO | axe |

**Ferramentas de teste:**

```bash
# axe CLI — auditoria rápida de acessibilidade
npm install -g @axe-core/cli
axe https://sua-app.com --tags wcag2a,wcag2aa

# Lighthouse via CLI
npx lighthouse https://sua-app.com --only-categories=accessibility --output=json
```

---

## 2. SEO Técnico

### 2.1 Meta Tags e Estrutura

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `<title>` único e descritivo em cada página (50–60 chars) | 🔴 CRÍTICO | Screaming Frog |
| ☐ | `<meta name="description">` único por página (150–160 chars) | 🟠 IMPORTANTE | Screaming Frog |
| ☐ | `<meta name="viewport" content="width=device-width, initial-scale=1">` | 🔴 CRÍTICO | Lighthouse |
| ☐ | `<html lang="pt-BR">` declarado | 🔴 CRÍTICO | Inspeção |
| ☐ | Canonical URL definido: `<link rel="canonical" href="...">` | 🟠 IMPORTANTE | Screaming Frog |
| ☐ | Open Graph tags para compartilhamento social | 🟢 COSMÉTICO | Facebook Debugger |
| ☐ | Sem meta robots `noindex` acidental em páginas de conteúdo | 🔴 CRÍTICO | Screaming Frog |

### 2.2 Structured Data (Schema.org)

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | JSON-LD implementado para tipo de conteúdo principal (Organization, Product, Article) | 🟠 IMPORTANTE | Google Rich Results Test |
| ☐ | Breadcrumbs marcados com `BreadcrumbList` | 🟢 COSMÉTICO | Google Rich Results Test |
| ☐ | Sem erros no Google Rich Results Test | 🟠 IMPORTANTE | Google Rich Results Test |

### 2.3 Performance Técnica para SEO

| # | Item de Verificação | Meta | Prioridade | Ferramenta |
|---|---------------------|------|-----------|------------|
| ☐ | Mobile-first — layout responsivo em todos os breakpoints | — | 🔴 CRÍTICO | Lighthouse |
| ☐ | LCP (Largest Contentful Paint) | < 2.5s | 🔴 CRÍTICO | PageSpeed Insights |
| ☐ | HTTPS em todas as páginas — HTTP redireciona para HTTPS | — | 🔴 CRÍTICO | Screaming Frog |
| ☐ | Sem links quebrados (404) | — | 🔴 CRÍTICO | Screaming Frog |
| ☐ | `sitemap.xml` gerado, atualizado e submetido ao Google Search Console | — | 🟠 IMPORTANTE | GSC |
| ☐ | `robots.txt` configurado corretamente — sem bloquear recursos CSS/JS | — | 🔴 CRÍTICO | GSC |

### 2.4 Internacionalização (i18n) — Contexto Brasil

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Strings de interface em português — sem mistura de idiomas no front | 🔴 CRÍTICO |
| ☐ | Fuso horário configurado: `date_default_timezone_set('America/Recife')` | 🔴 CRÍTICO |
| ☐ | Formatação de moeda em Real: `R$ 1.234,56` (ponto = milhar, vírgula = decimal) | 🔴 CRÍTICO |
| ☐ | Formatação de data no padrão brasileiro: `DD/MM/AAAA` | 🔴 CRÍTICO |
| ☐ | Validação de CPF, CNPJ, CEP e telefone BR nos formulários | 🟠 IMPORTANTE |
| ☐ | Máscaras de input para CPF `000.000.000-00` e telefone `(00) 00000-0000` | 🟠 IMPORTANTE |
| ☐ | `number_format()` usando vírgula como separador decimal | 🔴 CRÍTICO |

---

## 3. Gestão de Dependências & Supply Chain

### 3.1 Dependências PHP (Composer)

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `composer.lock` versionado no repositório | 🔴 CRÍTICO | Git |
| ☐ | `composer audit` executado no CI — zero vulnerabilidades HIGH/CRITICAL | 🔴 CRÍTICO | Composer |
| ☐ | Dependências atualizadas — sem versões com mais de 2 anos sem update | 🟠 IMPORTANTE | `composer outdated` |
| ☐ | Dependências de dev separadas das de produção (`require` vs `require-dev`) | 🟠 IMPORTANTE | composer.json |
| ☐ | Sem `"*"` como versão de dependência no composer.json | 🔴 CRÍTICO | Inspeção |
| ☐ | Licenças de todas as dependências verificadas e compatíveis | 🟠 IMPORTANTE | `composer licenses` |

### 3.2 Dependências JavaScript (npm)

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | `package-lock.json` versionado no repositório | 🔴 CRÍTICO | Git |
| ☐ | `npm audit --audit-level=high` no CI — zero vulnerabilidades | 🔴 CRÍTICO | npm |
| ☐ | Dependências de produção mínimas — sem libs usadas apenas no dev em `dependencies` | 🟠 IMPORTANTE | package.json |
| ☐ | Sem pacotes abandonados (último release > 2 anos, zero mantenedores) | 🟠 IMPORTANTE | Inspeção manual |
| ☐ | CDN externo com Subresource Integrity (SRI): `integrity="sha384-..."` | 🟠 IMPORTANTE | srihash.org |

**Verificar licenças JS:**

```bash
npx license-checker --onlyAllow 'MIT;ISC;BSD-2-Clause;BSD-3-Clause;Apache-2.0'
```

### 3.3 SBOM — Software Bill of Materials

| # | Item de Verificação | Prioridade | Ferramenta |
|---|---------------------|-----------|------------|
| ☐ | SBOM gerado e atualizado a cada release | 🟠 IMPORTANTE | CycloneDX |
| ☐ | SBOM incluído como artefato no CI/CD | 🟢 COSMÉTICO | CycloneDX |

```bash
# Gerar SBOM em formato CycloneDX
composer require --dev cyclonedx/cyclonedx-php-composer
composer make-bom --output-file=sbom.xml
```

---

## 4. Documentação Técnica

### 4.1 README

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | README.md presente e atualizado na raiz do projeto | 🔴 CRÍTICO |
| ☐ | Instruções de instalação local funcionais (testadas) | 🔴 CRÍTICO |
| ☐ | Variáveis de ambiente necessárias documentadas | 🔴 CRÍTICO |
| ☐ | Como rodar os testes documentado | 🔴 CRÍTICO |
| ☐ | Arquitetura geral do projeto descrita em 1 parágrafo | 🟠 IMPORTANTE |
| ☐ | Dependências externas (Redis, MySQL, serviços de terceiros) listadas | 🟠 IMPORTANTE |
| ☐ | Processo de deploy documentado | 🟠 IMPORTANTE |

**Estrutura mínima de README.md:**

```markdown
# Nome do Projeto

Breve descrição da aplicação e seu propósito.

## Requisitos
- PHP 8.3+
- MySQL 8.0+
- Redis 7+
- Node.js 20+

## Instalação local

1. Clone o repositório
2. Copie `.env.example` para `.env` e preencha as variáveis
3. `composer install`
4. `npm install`
5. `php migrate.php` (ou comando de migration)
6. `php -S localhost:8000 -t public/`

## Testes

```bash
./vendor/bin/phpunit           # todos os testes
./vendor/bin/phpunit --testsuite Unit       # apenas unitários
./vendor/bin/phpunit --coverage-html coverage/
```

## Arquitetura

Aplicação em 4 camadas: Front (HTML/JS), Mid (Controllers/Services),
Back (Repositories/Models), Foundation (MySQL).
Ver docs/architecture.md para detalhes.

## Deploy

Ver docs/deploy.md para processo completo de deploy.
```

### 4.2 Docblocks PHP

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Todos os métodos públicos de Services com docblock | 🟠 IMPORTANTE |
| ☐ | Todos os métodos de Repositories com docblock e tipos de retorno | 🟠 IMPORTANTE |
| ☐ | Interfaces completamente documentadas | 🟠 IMPORTANTE |
| ☐ | `@throws` documentado em métodos que lançam exceções | 🟢 COSMÉTICO |

### 4.3 ADR — Architecture Decision Records

| # | Item de Verificação | Prioridade |
|---|---------------------|-----------|
| ☐ | Pasta `docs/adr/` com decisões arquiteturais documentadas | 🟢 COSMÉTICO |
| ☐ | Decisões de tecnologia principais registradas (por que MySQL e não PostgreSQL, etc.) | 🟢 COSMÉTICO |
| ☐ | ADRs em formato padronizado (Título, Status, Contexto, Decisão, Consequências) | 🟢 COSMÉTICO |

---

## 5. Prompts de Auditoria para Claude Opus

### Acessibilidade

```
Faça uma auditoria de acessibilidade WCAG 2.1 nível AA deste projeto HTML/JS.
Reporte em português.

- Há hierarquia correta de headings (h1 → h2 → h3)?
- Todos os campos de formulário têm <label> associado?
- Imagens têm atributo alt adequado?
- A navegação por teclado funciona em todos os elementos interativos?
- Há foco visível nos elementos?
- Há aria-live em regiões de conteúdo dinâmico?
- Há dependência exclusiva de cor para transmitir informação?
- Liste as 5 violações mais graves com artigo WCAG correspondente.
```

### SEO Técnico

```
Faça uma auditoria de SEO técnico deste projeto HTML/PHP.
Reporte em português.

- Cada página tem <title> e <meta description> únicos?
- <html lang="pt-BR"> está declarado?
- Há canonical URL definido?
- robots.txt está correto?
- sitemap.xml está presente?
- Há structured data (JSON-LD)?
- HTTPS está forçado?
- Formatação de datas e moedas está no padrão brasileiro?
- Liste as 5 maiores oportunidades de melhoria.
```

### Dependências

```
Analise as dependências deste projeto PHP/JS.
Reporte em português.

- Há pacotes com vulnerabilidades conhecidas (composer audit / npm audit)?
- Há dependências desatualizadas há mais de 2 anos?
- composer.lock e package-lock.json estão versionados?
- As licenças das dependências são compatíveis com uso comercial?
- Há dependências de dev em produção?
- CDN externo usa SRI (Subresource Integrity)?
```

---

## 6. Roadmap Combinado

| Sprint | Área | Objetivo | Critério |
|--------|------|----------|----------|
| 🔴 1 | Acessibilidade | Corrigir labels de formulário e alt de imagens | axe zero erros críticos |
| 🔴 2 | SEO | Title, description, lang e canonical em todas as páginas | Screaming Frog zero erros |
| 🔴 3 | i18n Brasil | Timezone, moeda, data e validações BR | Zero string hardcoded em inglês |
| 🟠 4 | Dependências | Audit no CI + atualizar pacotes críticos | Zero HIGH/CRITICAL no audit |
| 🟠 5 | Documentação | README funcional + docblocks em Services | Onboarding em < 30 min |
| 🟠 6 | Contraste e foco | Corrigir contraste < 4.5:1 e foco visível | Lighthouse acessibilidade ≥ 90 |
| 🟢 7 | Structured Data | JSON-LD para tipo principal de conteúdo | Google Rich Results sem erros |
| 🟢 8 | SBOM + ADR | Gerar SBOM e documentar decisões arquiteturais | SBOM no CI como artefato |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

## III7 — Hardening PHP + LGPD — Pipeline OWASP ZAP, Incidentes

# 🔒 Hardening de Segurança PHP
## Pipeline de Validação Contínua com PHPStan + ESLint + OWASP ZAP
### Conformidade com a LGPD — Lei 13.709/2018

| Versão | Classificação | Escopo |
|--------|--------------|--------|
| 1.0 | Confidencial | PHP / MySQL / JS |

---

## 1. Introdução e Contexto LGPD

Este documento consolida o checklist de hardening de segurança para aplicações PHP/MySQL/JS e o pipeline de validação contínua com PHPStan, ESLint e OWASP ZAP. Todos os controles foram mapeados aos artigos relevantes da Lei Geral de Proteção de Dados (LGPD — Lei 13.709/2018).

### Princípios LGPD aplicáveis a aplicações web

| Artigo | Princípio | Implicação para o desenvolvedor |
|--------|-----------|--------------------------------|
| Art. 6º, I | Finalidade | Coletar apenas dados necessários para a funcionalidade declarada |
| Art. 6º, III | Necessidade | Minimizar campos de formulário e dados armazenados em banco |
| Art. 6º, VII | Segurança | Proteger dados contra acesso não autorizado, vazamento e ataque |
| Art. 6º, VIII | Prevenção | Adotar medidas técnicas preventivas antes da ocorrência de incidentes |
| Art. 46 | Medidas de segurança | Implementar medidas técnicas e administrativas para proteger dados pessoais |
| Art. 48 | Comunicação de incidentes | Notificar ANPD e titulares em até 72h após conhecimento de vazamento |

---

## 2. Checklist de Hardening — Segurança Técnica

> **Legenda de prioridade:** 🔴 CRÍTICO — 🟠 IMPORTANTE — 🟢 COSMÉTICO

---

### 2.1 Injeção e Validação de Entrada

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | Usar PDO ou MySQLi com prepared statements em todas as queries | 🔴 CRÍTICO | Art. 46 | PHPStan + grep manual |
| ☐ | Nunca concatenar variáveis `$_GET`/`$_POST` diretamente em SQL | 🔴 CRÍTICO | Art. 46 | PHPStan / Semgrep |
| ☐ | Validar e sanitizar toda entrada antes de usar (`filter_input`) | 🔴 CRÍTICO | Art. 46 | PHPStan |
| ☐ | Usar `htmlspecialchars()` em todo output dinâmico no HTML | 🔴 CRÍTICO | Art. 46 | OWASP ZAP / ESLint |
| ☐ | Nunca usar `eval()` ou `exec()` com dados do usuário | 🔴 CRÍTICO | Art. 46 | PHPStan |
| ☐ | Validar uploads: tipo MIME real, extensão, tamanho máximo | 🔴 CRÍTICO | Art. 46 | PHPStan |
| ☐ | Mover arquivos de upload para fora do webroot | 🟠 IMPORTANTE | Art. 46 | Revisão manual |

---

### 2.2 Autenticação e Sessões

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | Senhas armazenadas com `password_hash()` (bcrypt/argon2) | 🔴 CRÍTICO | Art. 46 | Grep / PHPStan |
| ☐ | Usar `password_verify()` — nunca comparação direta | 🔴 CRÍTICO | Art. 46 | PHPStan |
| ☐ | Regenerar ID de sessão após login (`session_regenerate_id`) | 🔴 CRÍTICO | Art. 46 | PHPStan |
| ☐ | Configurar `session.cookie_httponly = 1` e `session.cookie_secure = 1` | 🔴 CRÍTICO | Art. 46 | Configuração PHP |
| ☐ | Implementar logout que destrói sessão completamente | 🟠 IMPORTANTE | Art. 46 | Revisão manual |
| ☐ | Limitar tentativas de login (rate limiting / lockout) | 🟠 IMPORTANTE | Art. 6, VII | Revisão manual |
| ☐ | Implementar autenticação multifator (2FA) para áreas críticas | 🟠 IMPORTANTE | Art. 46 | Revisão manual |
| ☐ | Tokens de API com expiração definida e revogação possível | 🟠 IMPORTANTE | Art. 46 | Revisão manual |

---

### 2.3 CSRF e Headers de Segurança

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | Adicionar token CSRF em todos os formulários POST | 🔴 CRÍTICO | Art. 46 | OWASP ZAP |
| ☐ | Validar token CSRF no servidor antes de processar formulário | 🔴 CRÍTICO | Art. 46 | PHPStan / manual |
| ☐ | Configurar header `Content-Security-Policy` (CSP) | 🟠 IMPORTANTE | Art. 46 | OWASP ZAP |
| ☐ | Configurar header `X-Frame-Options: DENY` ou `SAMEORIGIN` | 🟠 IMPORTANTE | Art. 46 | OWASP ZAP |
| ☐ | Configurar header `X-Content-Type-Options: nosniff` | 🟠 IMPORTANTE | Art. 46 | OWASP ZAP |
| ☐ | Configurar `Strict-Transport-Security` (HSTS) | 🟠 IMPORTANTE | Art. 46 | OWASP ZAP |
| ☐ | Remover header `X-Powered-By` e `Server` do response | 🟢 COSMÉTICO | Art. 46 | OWASP ZAP |
| ☐ | Configurar `Referrer-Policy` adequado | 🟢 COSMÉTICO | Art. 6, I | OWASP ZAP |

---

### 2.4 Configuração do Servidor e PHP

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | `display_errors = Off` em produção (usar log) | 🔴 CRÍTICO | Art. 46 | php.ini / PHPStan |
| ☐ | `expose_php = Off` no php.ini | 🟠 IMPORTANTE | Art. 46 | Configuração |
| ☐ | Desabilitar funções perigosas: `disable_functions = exec,passthru,...` | 🟠 IMPORTANTE | Art. 46 | Configuração |
| ☐ | Forçar HTTPS em toda a aplicação (`.htaccess`) | 🔴 CRÍTICO | Art. 46 | OWASP ZAP |
| ☐ | Proteger diretórios `vendor/`, `.git/`, `config/` com `.htaccess` | 🔴 CRÍTICO | Art. 46 | OWASP ZAP |
| ☐ | Definir `open_basedir` para limitar acesso ao filesystem | 🟠 IMPORTANTE | Art. 46 | Configuração |
| ☐ | Manter PHP atualizado (versão com suporte ativo) | 🟠 IMPORTANTE | Art. 46 | Renovação automática |
| ☐ | Configurar `error_log` em local seguro fora do webroot | 🟠 IMPORTANTE | Art. 46 | Configuração |

---

### 2.5 Dados Pessoais e LGPD

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | Credenciais e chaves de API apenas em variáveis de ambiente (`.env`) | 🔴 CRÍTICO | Art. 46 | Grep / PHPStan |
| ☐ | Arquivo `.env` fora do webroot e no `.gitignore` | 🔴 CRÍTICO | Art. 46 | Git audit |
| ☐ | Criptografar dados sensíveis em repouso (CPF, e-mail, telefone) | 🔴 CRÍTICO | Art. 46 | Revisão manual |
| ☐ | Implementar política de retenção: deletar dados após prazo legal | 🔴 CRÍTICO | Art. 15/16 | Revisão manual |
| ☐ | Registrar log de acesso a dados pessoais (trilha de auditoria) | 🟠 IMPORTANTE | Art. 37 | Revisão manual |
| ☐ | Implementar mecanismo de exclusão de conta (direito ao esquecimento) | 🔴 CRÍTICO | Art. 18, VI | Revisão manual |
| ☐ | Implementar exportação de dados do titular (portabilidade) | 🟠 IMPORTANTE | Art. 18, V | Revisão manual |
| ☐ | Exibir aviso de cookies e obter consentimento explícito | 🟠 IMPORTANTE | Art. 7, I | OWASP ZAP / Manual |
| ☐ | Documentar o fluxo de dados pessoais (mapeamento ROPA) | 🟠 IMPORTANTE | Art. 37 | Processo interno |
| ☐ | Pseudonimizar dados pessoais onde possível | 🟠 IMPORTANTE | Art. 13 | Revisão manual |

---

### 2.6 Banco de Dados MySQL

| # | Item de Verificação | Prioridade | Artigo LGPD | Ferramenta |
|---|---------------------|-----------|-------------|------------|
| ☐ | Usuário de banco com permissões mínimas (sem `GRANT ALL`) | 🔴 CRÍTICO | Art. 46 | MySQL audit |
| ☐ | Senha forte para usuário do banco — nunca `root` em produção | 🔴 CRÍTICO | Art. 46 | Configuração |
| ☐ | Charset `utf8mb4` e collation `utf8mb4_unicode_ci` em todas as tabelas | 🟠 IMPORTANTE | Art. 46 | SQL review |
| ☐ | Definir FK, NOT NULL, UNIQUE constraints adequadas | 🟠 IMPORTANTE | Art. 46 | SQL review |
| ☐ | Índices em todas as colunas usadas em WHERE/JOIN/ORDER BY | 🟠 IMPORTANTE | — | EXPLAIN / Percona |
| ☐ | Criptografar backups e armazenar em local seguro | 🔴 CRÍTICO | Art. 46 | Processo |
| ☐ | Testar restauração de backup periodicamente | 🟠 IMPORTANTE | Art. 46/48 | Processo |
| ☐ | Habilitar binary log para auditoria de alterações | 🟠 IMPORTANTE | Art. 37 | Configuração MySQL |

---

## 3. Pipeline de Validação Contínua

O pipeline abaixo deve ser executado a cada commit (pre-commit hook), em Pull Requests (CI/CD) e semanalmente de forma completa com o OWASP ZAP em modo spider completo.

### 3.1 Visão geral do fluxo

| Etapa | Ferramenta | Comando | Falha = ? |
|-------|-----------|---------|-----------|
| 1. Análise estática PHP | PHPStan (nível 8) | `composer phpstan analyse --level=8` | Bloqueia PR |
| 2. Análise estática JS | ESLint + plugin security | `npx eslint . --ext .js,.ts` | Bloqueia PR |
| 3. Detecção de segredos | trufflehog | `trufflehog git file://.` | Bloqueia PR |
| 4. Dependências PHP | composer audit | `composer audit --format=json` | Bloqueia PR |
| 5. Dependências JS | npm audit | `npm audit --audit-level=high` | Bloqueia PR |
| 6. DAST — passivo | OWASP ZAP Baseline | `zap-baseline.py -t $APP_URL` | Bloqueia deploy |
| 7. DAST — ativo (semanal) | OWASP ZAP Full Scan | `zap-full-scan.py -t $APP_URL` | Gera relatório |
| 8. LGPD — cookies/consent | OWASP ZAP + manual | Verificar aviso e consentimento | Gera relatório |

---

### 3.2 Configuração — PHPStan

**Instalação via Composer:**

```bash
composer require --dev phpstan/phpstan phpstan/phpstan-strict-rules
composer require --dev phpstan/extension-installer
```

**Arquivo `phpstan.neon`:**

```neon
parameters:
    level: 8
    paths:
        - src
        - app
    treatPhpDocTypesAsCertain: false
    checkMissingIterableValueType: true
```

---

### 3.3 Configuração — ESLint com plugin de segurança

**Instalação:**

```bash
npm install --save-dev eslint eslint-plugin-security
npm install --save-dev eslint-plugin-no-unsanitized
```

**Arquivo `.eslintrc.json`:**

```json
{
  "plugins": ["security", "no-unsanitized"],
  "extends": ["plugin:security/recommended"],
  "rules": {
    "no-unsanitized/method": "error",
    "no-unsanitized/property": "error",
    "no-eval": "error"
  }
}
```

---

### 3.4 Configuração — OWASP ZAP

**Scan passivo — rápido, ideal para cada PR:**

```bash
docker run -t ghcr.io/zaproxy/zaproxy:stable \
  zap-baseline.py -t https://sua-app.com \
  -r zap-report.html --auto
```

**Scan ativo completo — semanal ou pré-release:**

```bash
docker run -t ghcr.io/zaproxy/zaproxy:stable \
  zap-full-scan.py -t https://sua-app.com \
  -r zap-full-report.html -x zap-full-report.xml
```

**Regras ZAP prioritárias para LGPD:**

| Regra | Descrição | Prioridade | Artigo LGPD | Tipo |
|-------|-----------|-----------|-------------|------|
| 10010 | Cookie sem flag HttpOnly | 🔴 CRÍTICO | Art. 46 | ZAP automático |
| 10011 | Cookie sem flag Secure | 🔴 CRÍTICO | Art. 46 | ZAP automático |
| 10038 | Content-Security-Policy ausente | 🟠 IMPORTANTE | Art. 46 | ZAP automático |
| 10015 | Incomplete or No Cache-Control Header | 🟠 IMPORTANTE | Art. 46 | ZAP automático |
| 40012 | Cross-Site Scripting (Reflected) | 🔴 CRÍTICO | Art. 46 | ZAP ativo |
| 40014 | Cross-Site Scripting (Persistent) | 🔴 CRÍTICO | Art. 46 | ZAP ativo |
| 40018 | SQL Injection | 🔴 CRÍTICO | Art. 46 | ZAP ativo |
| 40019 | SQL Injection (MySQL) | 🔴 CRÍTICO | Art. 46 | ZAP ativo |
| 10054 | Cookie sem atributo SameSite (proteção CSRF) | 🟠 IMPORTANTE | Art. 46 | ZAP automático |

---

## 4. Workflow GitHub Actions

Cole em `.github/workflows/security.yml`:

```yaml
name: Security Pipeline

on: [push, pull_request]

jobs:
  phpstan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: composer install --no-progress
      - run: composer phpstan analyse --level=8
      - run: composer audit

  eslint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: npm ci
      - run: npx eslint . --ext .js,.ts
      - run: npm audit --audit-level=high

  secrets-scan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0
      - name: TruffleHog OSS
        uses: trufflesecurity/trufflehog@main
        with:
          path: ./
          base: ${{ github.event.repository.default_branch }}

  zap-scan:
    runs-on: ubuntu-latest
    needs: [phpstan, eslint]
    steps:
      - uses: actions/checkout@v4
      - name: ZAP Baseline Scan
        uses: zaproxy/action-baseline@v0.13.0
        with:
          target: 'https://staging.sua-app.com'
```

---

## 5. Plano de Resposta a Incidentes — LGPD Art. 48

Em caso de vazamento ou acesso não autorizado a dados pessoais, a LGPD exige notificação à ANPD e aos titulares afetados. O prazo recomendado pela ANPD é de **até 72 horas** após o conhecimento do incidente.

### 5.1 Fluxo de resposta

| Fase | Ação | Responsável / Ferramenta | Prazo |
|------|------|--------------------------|-------|
| 🔴 1 | Identificar e conter o incidente | Equipe técnica — revogar tokens, bloquear IP, derrubar endpoint | **Imediato** |
| 🟠 2 | Avaliar escopo e dados afetados | DPO + equipe técnica — logs de acesso, tabelas afetadas | < 12 horas |
| 🟠 3 | Notificar internamente a diretoria | DPO — relatório preliminar do incidente | < 24 horas |
| 🟢 4 | Notificar ANPD (portal.anpd.gov.br) | DPO — formulário ANPD com detalhes do incidente | **< 72 horas** |
| 🟢 5 | Notificar titulares afetados | DPO + equipe — e-mail claro e objetivo | **< 72 horas** |
| ⚪ 6 | Documentar lições aprendidas | Equipe técnica — post-mortem + ajustes no pipeline | < 7 dias |

### 5.2 Informações mínimas para notificação à ANPD

- Natureza dos dados pessoais afetados
- Quantidade de titulares afetados (estimativa)
- Medidas técnicas adotadas para contenção
- Possíveis consequências do incidente
- Medidas adotadas ou propostas para mitigar os efeitos
- Dados de contato do DPO (Encarregado de Dados)

---

## 6. Roadmap de Implementação

Ordem recomendada de execução para projetos legados com débito técnico de segurança:

| Sprint | Objetivo | Ações | Resultado esperado |
|--------|----------|-------|--------------------|
| 🔴 1 | Eliminar vulnerabilidades críticas | PDO/prepared statements, `password_hash`, CSRF token, `.env` | Zero SQL Injection / XSS crítico |
| 🟠 2 | Configurar pipeline CI/CD | PHPStan + ESLint + GitHub Actions + ZAP Baseline | Pipeline funcional bloqueando PRs inseguros |
| 🟠 3 | Headers e configuração de servidor | CSP, HSTS, X-Frame-Options, php.ini hardening | OWASP ZAP Baseline com zero alertas HIGH |
| 🟢 4 | Adequação LGPD | Criptografia de dados, consentimento, exclusão de conta, ROPA | Conformidade Art. 46 e Art. 18 LGPD |
| ⚪ 5 | Maturidade e monitoramento | ZAP Full Scan semanal, alertas automáticos, DPO definido | Pipeline de segurança contínua e sustentável |

---

*Documento gerado com Claude Opus — uso interno confidencial*

---

*Consolidado em: 2026-04-29 · Fonte: cppqtsql-stack.zip · py_qt_sq.zip · audits_site.tar.gz*
