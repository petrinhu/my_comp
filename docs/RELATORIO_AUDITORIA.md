# Relatório final de auditoria (QA-08 + QA-16)

**Data:** 2026-11-26 · **Base:** branch `claude/funny-allen-lpxe8k`, rumo à `main` em um PR por correção · **Ambiente da medição:** container Ubuntu 24.04, GCC 13.3, clang 18, Qt 6.4.

Este relatório consolida o §A10 do [`TESTES.md`](TESTES.md) e o diagnóstico categorizado pedido no QA-16. As metas numéricas genéricas do `TESTES.md` não valem aqui: a [MC-07](../GODS_LAWS.md#mc-07) pede gates verdes e cobertura **medida**, sem meta.

## 1. Gates de qualidade

| Gate | Ferramenta | Resultado | Como reproduzir |
| :--- | :--- | :--- | :--- |
| T1 Unit | GoogleTest | 466 passam, 1 pulado (SMBIOS legível como root no container) | `cd build && ctest --output-on-failure` |
| T1 Aceitação | GoogleTest + GMock | 36 passam, 2 pulados (sem DMI/hardware no container) | idem |
| T1 QML | qmltestrunner | 4 passam (IconButton e KillProcessDialog) | idem |
| T2 Estática | clang-tidy (bugprone, cert, security) | 0 achados em `src/` | `scripts/ci.sh` |
| T2 Estática | cppcheck (error/warning) | 0 achados | `scripts/ci.sh` |
| T4 Memória | valgrind memcheck | 0 erros, 0 leaks definidos | `scripts/ci.sh` |
| T3 Fuzzing | libFuzzer + ASan (+ UBSan na VRAM) | 5 harnesses; o da VRAM achou 1 estouro, corrigido, e rodou limpo depois | ver bloco BT-02 do `CMakeLists.txt` |
| Concorrência | TSan (QA-15) | verde; 26 avisos triados como falso positivo de Qt e Pulse não instrumentados | ver AUDIT-28 |
| A3 Acessibilidade | `scripts/check_contrast.py` | todos os pares de cor da paleta com contraste de pelo menos 4,5:1 | `scripts/ci.sh` |
| Texto puro no QML | `scripts/check_qml_plaintext.py` | todo `Text`/`Label` com dado dinâmico declara `textFormat` | `scripts/ci.sh` |
| A6 Cobertura | gcovr | 80,5% das linhas (detalhe na seção 2) | `scripts/coverage.sh` |
| T8 Segredos | gitleaks (141 commits) | 0 achados; 2 exemplos didáticos em docs listados em `.gitleaksignore` | `gitleaks detect --source .` |
| Compilação | GCC 13.3 com `-Wall -Wextra -Wpedantic` e afins | 0 warnings no build limpo | `cmake -B build -G Ninja && cmake --build build` |

Não medidos aqui: T12 (CVEs de dependências, falta `trivy` no container) e o `-Werror` com o GCC do Fedora 43 (AUDIT-02).

## 2. Cobertura por camada

| Camada | Linhas |
| :--- | :--- |
| domain | 98,7% |
| application | 96,3% |
| infrastructure | 83,9% |
| presentation | 72,0% |
| composition | 35,9% |
| **Total** | **80,5%** |

Os menores números têm explicação: `composition/` é o código de montagem que só o `main` executa; `nvml_adapter_dl.cpp` precisa de GPU NVIDIA; `dbus_firmware_provider.cpp` precisa do fwupd no barramento. O `DiagnosticViewModel` (2,4%) e o `LogEntryModel` (3,4%) eram lacuna de teste de verdade e foram cobertos no QA-19.

## 3. Achados desta rodada e correções

| Achado | Gravidade | Correção |
| :--- | :--- | :--- |
| `vram_feedback.bin` com `id` 2147483647 estourava `int` e o próximo id apagava a entrada carregada (achado do fuzzer) | IMPORTANTE | `ID_MAXIMO` na carga e no `inserir` (AUDIT-33) |
| Template de busca online aceitava `file://` e `javascript:` antes do `QDesktopServices::openUrl` | IMPORTANTE | só http(s) com `%1` (QA-19, MC-10) |
| Exceção no worker da auditoria VRAM deixava a tela presa em "ocupado" | IMPORTANTE | try/catch no worker e PID inválido descartado no NVML (QA-13) |
| `std::stoi`/`std::stoull` lançavam com /proc malformado | IMPORTANTE | `parse_integer` sem exceção (QA-13) |
| 5 achados do gate T2 entraram nas ondas 10 e 11 | IMPORTANTE | corrigidos ou justificados com NOLINT (PR do gate T2) |
| QML desenhava marcação vinda de fora: `Text`/`Label` usam `AutoText`, e um `cmdline`, uma mensagem do journal ou o nome de um dispositivo Bluetooth vizinho com `<img>` ou `<font>` virava rich text; o diálogo de matar processo usava `StyledText` com o nome | IMPORTANTE | `Text.PlainText` em 90 elementos e gate `check_qml_plaintext.py` (AUDIT-36) |
| Matar processo podia atingir um PID reciclado entre a lista e o "Sim" | IMPORTANTE | `pidfd` + conferência do `starttime` (AUDIT-37) |
| `logLimit` negativo virava 0, que o provedor lê como "journal inteiro" | IMPORTANTE | negativo recusado |
| `cmdline` de vários MB (JVM) lido a cada atualização | COSMÉTICO | leitura limitada a 4 KiB |
| Nome do processo cortado em 15 caracteres | COSMÉTICO | lido de `/proc/<pid>/cmdline` (AUDIT-30) |
| Camada de aplicação chamava `QDir::homePath()` | COSMÉTICO | home injetado pela composição (AUDIT-34, MC-02) |
| `CpuArchitecture` era o único modelo de domínio com `std::string` | COSMÉTICO | `QString` (QA-12) |

As ondas anteriores (PRs 1 a 20) estão registradas linha a linha no [`TODO.md`](../TODO.md) e no `CHANGELOG.md`.

## 4. Diagnóstico do que falta (QA-16)

Esforço estimado para uma pessoa com o ambiente pronto.

### CRÍTICO: bloqueia a Beta

| Item | O que falta | Onde roda | Esforço |
| :--- | :--- | :--- | :--- |
| AUDIT-29 | Smoke manual de todas as abas no Fedora 43 + Plasma 6 Wayland, tema claro e escuro, `pkexec`, Orca | máquina do líder | 1 h |
| AUDIT-26 | Conferir o layout `nvmlProcessInfo_v2` numa GPU NVIDIA real | máquina com NVIDIA | 30 min |
| AUDIT-02 | Medir warnings com o GCC do Fedora 43 e ligar `-Werror` | máquina do líder | 30 min a 2 h, conforme os warnings novos |
| BT-04 | Release Beta (CHANGELOG, tag, push) depois dos três acima | `./deploy.sh` | 15 min |

### IMPORTANTE: antes ou logo depois da Beta

| Item | O que falta | Esforço |
| :--- | :--- | :--- |
| UX-05 | Auditar I/O e parsing na thread da UI | 1 dia |
| AUDIT-28 | Teste de concorrência determinístico para o snapshot do `ExportViewModel` | meio dia |
| AUDIT-03 | `qsTr()`/`tr()` nas strings que ainda faltam | 1 a 2 dias |
| AUDIT-01 | Tema Breeze embarcado no `.qrc` | 1 dia |
| AUDIT-35 | Chave do feedback VRAM no Secret Service em vez de arquivo 0600 | meio dia |
| AUDIT-31 | Utilização de compute por processo (precisa de GPU real) | 1 dia |
| QA-11 | IWYU completo e forward declarations | meio dia, quando o IWYU estiver no Fedora |

### COSMÉTICO: melhoria contínua

| Item | O que falta | Esforço |
| :--- | :--- | :--- |
| UX-03 | Layouts QML responsivos sem tamanho fixo | 1 dia |
| AUDIT-32 | UI de feedback da VRAM: listar, remover e escrever o motivo | meio dia |
| JR-02, DT-01, PE-01 | Funcionalidades parciais ou novas do journal, dev tools e impressoras | 1 a 3 dias cada |
| Itens 🎨 | 11 itens que esperam decisão de design | depende da decisão |
