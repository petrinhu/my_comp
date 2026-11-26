# CLAUDE.md: Contrato de Engenharia do my_comp v2.0

Fonte única de verdade para o desenvolvimento do **my_comp** (evolução do script legado em `./legacy/` para aplicação nativa C++23/Qt6), junto com o `GODS_LAWS.md`.

## Stack & ambiente

- **SO alvo:** Fedora 43 + KDE Plasma 6 + Wayland nativo (design agnóstico a distros; multi-PM detectado em runtime).
- **Linguagem:** C++23 estrito (flags `-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast -Wnull-dereference -fstack-protector-strong -D_FORTIFY_SOURCE=3` globais; `-Werror` em adoção incremental — hoje 491 warnings rastreados como dívida técnica).
- **Framework:** Qt6 — `Core`, `Gui`, `Quick`, `QuickControls2`, `WaylandClient`, `Multimedia`, `DBus`, `Network`, `Concurrent`, `Test`.
- **Build:** CMake ≥ 3.16 + Ninja.
- **Testes:** GoogleTest + CTest.
- **Cripto:** OpenSSL (AES-256-GCM).
- **Repositório:** `git@github.com:petrinhu/my_comp.git`.

## Documentos normativos

Ordem de autoridade e quando consultar cada um: MC-01.

- [`docs/CONTRACT.md`](docs/CONTRACT.md), [`docs/TESTES.md`](docs/TESTES.md) e [`docs/AUDITORIAS.md`](docs/AUDITORIAS.md).
- [`docs/claude/release.md`](docs/claude/release.md): ferramental de release (git-cliff, `cliff.toml`, `scripts/release.sh`, hook `pre-push`, `deploy.sh`).

## Comandos

Comandos-padrão:
```bash
cmake -B build -G Ninja && cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
valgrind --leak-check=full ./build/unit_tests
```

Agente: rode build, suíte e valgrind dentro da trava da L-11 item 2.

Release e push (quando rodar: MC-08):
```bash
# Caminho normal (release automática + push):
./deploy.sh                              # releaseia commits já criados e pusha
./deploy.sh "feat(x): mensagem"          # commita pendências + releaseia + pusha

# Pré-validação (sem alterar nada):
./scripts/release.sh --dry-run

# Setup inicial em clone novo:
git config core.hooksPath scripts/hooks
cargo install git-cliff   # ou: dnf install git-cliff (quando disponível no Fedora)
```

Push direto (`git push` fora do `deploy.sh`) é bloqueado pelo hook — força tooling consistente.

## Leis do projeto

Leia [`GODS_LAWS.md`](GODS_LAWS.md) antes de agir. `L-NN` sem prefixo é lei global.

| Lei | Gatilho: dispara quando você vai... |
|---|---|
| [MC-01](GODS_LAWS.md#mc-01) | resolver conflito entre documentos, marcar tarefa como ✅, ou propor merge |
| [MC-02](GODS_LAWS.md#mc-02) | criar arquivo, classe ou include que cruze camadas |
| [MC-03](GODS_LAWS.md#mc-03) | nomear qualquer coisa, ou ver arquivo ou classe crescer |
| [MC-04](GODS_LAWS.md#mc-04) | alocar memória, abrir recurso, ou escrever construtor |
| [MC-05](GODS_LAWS.md#mc-05) | escrever código Qt ou QML, thread, ou estilo de tela |
| [MC-06](GODS_LAWS.md#mc-06) | escrever ou alterar comportamento de código |
| [MC-07](GODS_LAWS.md#mc-07) | preparar a Beta, ou conferir um gate de qualidade |
| [MC-08](GODS_LAWS.md#mc-08) | terminar uma feature, ou pensar em release, tag ou push |
| [MC-09](GODS_LAWS.md#mc-09) | escrever mensagem de commit |
| [MC-10](GODS_LAWS.md#mc-10) | tocar dado sensível, rede, SQL ou entrada externa |
| [MC-11](GODS_LAWS.md#mc-11) | criar branch, fazer push, ou mexer no remoto |
| [MC-12](GODS_LAWS.md#mc-12) | mexer em versão, tag, `VERSION` ou `CHANGELOG.md` |
| [MC-13](GODS_LAWS.md#mc-13) | abrir ou editar algo em `./legacy/` |

## Pendências

A tabela de pendências e planejamento do projeto está em `TODO.md` na raiz.
