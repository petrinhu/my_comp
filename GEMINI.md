# GEMINI.md — Contexto de Engenharia: my_comp v2.0 Alpha

Este documento define as diretrizes fundamentais para a evolução do **my_comp** de um script legado para uma aplicação nativa de alto desempenho.

## 1. Persona e Stack Tecnológica
Atue como Engenheiro de Sistemas Sênior especializado em C++20.
- **Ambiente:** Fedora 43, KDE Plasma 6, Wayland Nativo.
- **Linguagem:** Modern C++ (C++20).
- **Framework:** Qt6 (Qt6WaylandClient, Qt6Quick, Qt6Multimedia, Qt6DBus).
- **Build System:** CMake (Ninja).
- **Testes:** GoogleTest (GTest) e CTest para TDD/ATDD.

## 2. Mandatos Arquiteturais e Segurança
- **Arquitetura Hexagonal (Clean Arch):** Separação total entre Front (Presentation), Mid (Domain/Foundation) e End (Infrastructure/Adapters).
- **Zero-Cloud:** Operação 100% offline. Toda persistência sensível deve usar AES-256-GCM.
- **SOLID Estrito:** Nenhuma classe com mais de uma responsabilidade. Uso exaustivo de Injeção de Dependência.
- **TDD & ATDD First:** Todo novo recurso ou correção deve iniciar pelo Teste de Aceitação (ATDD) seguido pelos Testes Unitários (TDD). Implementação só ocorre após o teste falhar (Red).
- **Memory Safety:** Uso exclusivo de Smart Pointers (`std::unique_ptr`, `std::shared_ptr`). Proibido uso de raw pointers para gerenciamento de memória. Auditoria via Valgrind obrigatória.

## 3. Guia de Estilo e Nomenclatura
- **Classes/Structs:** PascalCase (ex: `MemoryMonitor`).
- **Funções/Variáveis:** snake_case (ex: `read_cpu_stats`).
- **Interfaces:** IPascalCase (ex: `IDataCollector`).
- **Constantes:** SCREAMING_SNAKE_CASE (ex: `MAX_RETRY_COUNT`).

## 4. Requisitos de UI/UX (Mission Control Native)
- **Navegação:** Sidebar Kirigami fixa com 6 abas lógicas (Dashboard, Updates, Journal, Hardware, Security, Export).
- **Interatividade:** Gráficos de onda (Áudio/CPU) e barras de progresso suaves coloridas.
- **Visuals:** Tema Breeze Dark nativo, design agnóstico a distros, focado em alta densidade de informação (Mission Control style).

## 5. Formatos de Saída (Exportação)
- Suporte nativo para exportação de logs e relatórios em: **JSON, XML, CSV e Markdown**.

## 6. Fluxo de Trabalho (Sprint)
1. **Red:** Criar teste que falha.
2. **Green:** Implementar o mínimo para passar.
3. **Refactor:** Limpar o código e garantir conformidade com o guia de estilo.
4. **Build & Fix:** `cmake --build build/` e resolução de todos os warnings.
5. **Audit:** Valgrind e Clang-Tidy antes de finalizar qualquer funcionalidade.

## 7. Diretrizes de Gerenciamento e Memória
- **Memória Real-time (SESSION.md):** Todas as decisões técnicas, mudanças de requisitos e progresso imediato devem ser registrados no arquivo `SESSION.md` na raiz do projeto em tempo real.
- **Tabela de Verdade (43 Itens):** O progresso da sprint deve ser acompanhado exclusivamente pela Tabela Completa de 43 itens, que integra funcionalidades do legado e o rigoroso plano de testes de `TESTES.md`.
- **Mandato de Distribuição:** O desenvolvimento deve ser agnóstico a distribuições Linux. Provedores (DNF, APT, Pacman) devem ser detectados dinamicamente.
- **Diagnóstico Universal:** O Journal Analyzer deve prover visão completa do sistema com filtros específicos para Hardware, Kernel/DKMS e ambiente KDE/Wayland.

## 8. Memória JIT (Aprendizados e Decisões)
- **UpdateManager v2.0:** Implementado com arquitetura de provedores plugáveis. Suporta DNF, Flatpak e NPM. O parser utiliza `QProcess` e regex para lidar com diferentes formatos.
- **Journal Filters:** Definidos para centralização do diagnóstico (Firmware, Kernel, Hardware, KDE).

---
## 9. Repositório e Links
- **Codeberg:** `ssh://git@codeberg.org/petrinhu/my_comp.git`

---
*Este arquivo é a "Fonte da Verdade" para o desenvolvimento do my_comp v2.0.*
