# Guia Completo de Testes, Qualidade e Auditoria

> Documento instrucional para qualquer agente de IA ou engenheiro executar a suíte  
> completa de verificação em **qualquer projeto C++/Qt/CMake**.  
> Adapte os caminhos, nomes de módulos, executáveis e APIs conforme o projeto alvo.

---

## Índice

1. [Pré-requisitos e Instalação](#1-pré-requisitos-e-instalação)
2. [T1 — Testes Unitários](#t1--testes-unitários)
3. [T2 — Análise Estática](#t2--análise-estática)
4. [T3 — Fuzzing de Inputs](#t3--fuzzing-de-inputs)
5. [T4 — Análise Dinâmica de Memória](#t4--análise-dinâmica-de-memória)
6. [T5 — Scanning de Dependências](#t5--scanning-de-dependências)
7. [T6 — Teste de APIs](#t6--teste-de-apis)
8. [T7 — Scanning de Binário](#t7--scanning-de-binário)
9. [T8 — Verificação de Secrets](#t8--verificação-de-secrets)
10. [T9 — Teste de Rede](#t9--teste-de-rede)
11. [T10 — SQL Injection](#t10--sql-injection)
12. [T11 — Fuzzing de Protocolos de Rede](#t11--fuzzing-de-protocolos-de-rede)
13. [T12 — Busca de CVEs nas Dependências](#t12--busca-de-cves-nas-dependências)
14. [A1 — Descoberta e Modelagem](#a1--descoberta-e-modelagem)
15. [A2 — Auditoria de Arquitetura e Camadas](#a2--auditoria-de-arquitetura-e-camadas)
16. [A3 — UI/UX e Acessibilidade](#a3--uiux-e-acessibilidade)
17. [A4 — QA Geral C++](#a4--qa-geral-c)
18. [A5 — Análise Estática e Dinâmica C/C++](#a5--análise-estática-e-dinâmica-cc)
19. [A6 — Cobertura de Testes](#a6--cobertura-de-testes)
20. [A7 — Dependências e Acoplamento](#a7--dependências-e-acoplamento)
21. [A8 — Consistência .h vs .cpp](#a8--consistência-h-vs-cpp)
22. [A9 — Análise Arquitetural Geral](#a9--análise-arquitetural-geral)
23. [A10 — Relatório Final de Auditoria](#a10--relatório-final-de-auditoria)
24. [Classificação de Problemas](#classificação-de-problemas)
25. [Formato de Patch](#formato-de-patch)

---

## 🛠 Ecossistema de Testes & QA (Mandatos TDD/ATDD)

Para **qualquer projeto C++/Qt/CMake**, utilizaremos as seguintes ferramentas integradas ao ciclo de desenvolvimento:

### 1. TDD (Test-Driven Development) - Unidade e Lógica
*   **QtTest Framework**: Testes de classes Qt (QString, QSignalSpy) e integração com o Event Loop.
*   **GoogleTest (GTest)**: Mocking complexo para motores de IA e abstrações de hardware.
*   **LCOV / Gcovr**: Geração de relatórios de cobertura de código (Alvo: 70%+).

### 2. ATDD (Acceptance Test-Driven Development) - Requisitos e Usuário
*   **Cucumber-cpp / Gherkin**: Definição de cenários de aceitação em linguagem natural.
*   **Scripts Python (PySide6)**: Automação de interface e simulação de fluxos.
*   **DBus Spy / KWin FakeInput**: Validação de injeção de texto nativa em ambiente Wayland.

### 3. QA & Análise Estática (Segurança e Qualidade)
*   **Clang-Tidy & CppCheck**: Verificação de conformidade com Modern C++20 e detecção de bugs latentes.
*   **Gitleaks**: Prevenção de exposição de segredos e chaves no histórico Git.
*   **Trivy / Grype**: Auditoria de vulnerabilidades (CVEs) em bibliotecas do projeto.

### 4. Análise Dinâmica e Estresse
*   **ASan / UBSan / TSan**: Detecção de vazamentos de memória, comportamento indefinido e *data races*.
*   **AFL++ (Fuzzing)**: Testes de estresse com inputs malformados.
*   **Valgrind / Perf**: Baseline de performance e telemetria de hardware.

---

## 1. Pré-requisitos e Instalação

Instale todas as ferramentas antes de executar qualquer etapa.  
Comandos equivalentes para as três distribuições principais:

```bash
# Fedora (dnf)
sudo dnf install -y cppcheck clang-tools-extra gcc g++ cmake \
    afl++ libFuzzer valgrind gdb lcov gcovr \
    trivy grype gitleaks nmap sqlmap \
    hurl ffuf radare2 python3-pip

# Debian/Ubuntu (apt)
sudo apt install -y cppcheck clang clang-tidy g++ cmake \
    afl++ valgrind lcov gcovr \
    nmap sqlmap python3-pip
# trivy, grype, gitleaks, hurl, ffuf via binário ou snap

# Arch (pacman)
sudo pacman -S --noconfirm cppcheck clang gcc cmake \
    afl++ valgrind lcov gcovr nmap sqlmap
# trivy, grype, gitleaks via AUR: yay -S trivy-bin grype-bin gitleaks
```

Crie o diretório de relatórios antes de qualquer execução:

```bash
mkdir -p reports
```

---

## T1 — Testes Unitários

**Objetivo:** verificar que cada módulo se comporta conforme especificado de forma isolada.

**Ferramenta:** QtTest (embutido no Qt6) ou Google Test.

```bash
# Configurar build com testes habilitados
cmake -B build_test -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build_test -j$(nproc)

# Executar todos os testes
cd build_test && ctest --output-on-failure -j$(nproc)

# Executar módulo específico
./build_test/tests/test_[seu_modulo]
```

**Critério de aprovação:** 0 falhas. Cobertura mínima de 70% nos módulos críticos.

**Por que é crítico:** testes unitários detectam regressões antes de chegarem à integração.

---

## T2 — Análise Estática

**Objetivo:** detectar bugs, má práticas e problemas de segurança sem executar o código.

**Ferramentas:** `cppcheck` + `clang-tidy`.

```bash
# cppcheck — análise completa com saída XML
cppcheck --enable=all \
         --enable=security \
         --inconclusive \
         --suppress=missingIncludeSystem \
         --xml \
         --xml-version=2 \
         src/ 2> reports/cppcheck.xml

# Converter XML para HTML legível
cppcheck-htmlreport --file=reports/cppcheck.xml \
                    --report-dir=reports/cppcheck_html \
                    --source-dir=.

# clang-tidy — checks de modernização, performance e segurança
find src -name "*.cpp" | xargs clang-tidy \
    --checks="modernize-*,performance-*,readability-*,bugprone-*,security-*" \
    --warnings-as-errors="bugprone-*,security-*" \
    -- -std=c++20 -I src \
    2>&1 | tee reports/clang_tidy.txt
```

**Critério de aprovação:** zero erros `bugprone-*` e `security-*`. Warnings documentados.

**Por que é crítico:** detecta buffer overflows, use-after-free e injeções antes do runtime.

---

## T3 — Fuzzing de Inputs

**Objetivo:** encontrar crashes e comportamentos inesperados ao receber entradas malformadas.

**Ferramenta:** `afl++` ou `libFuzzer`.

```bash
# Compilar alvo para fuzzing com afl++
AFL_USE_ASAN=1 afl-g++ -std=c++20 -o fuzz_target \
    src/[seu_modulo_para_fuzz].cpp \
    -I src -lQt6Core -lQt6Network

# Criar corpus inicial
mkdir -p fuzz/corpus fuzz/findings
echo '{"campo":"valor_exemplo"}' > fuzz/corpus/seed1.json
echo '{"campo":""}' > fuzz/corpus/seed2.json
echo '{"campo":"A"}' > fuzz/corpus/seed3.json

# Executar fuzzer (Ctrl+C para parar)
afl-fuzz -i fuzz/corpus -o fuzz/findings -- ./fuzz_target @@

# Verificar crashes encontrados
ls fuzz/findings/crashes/
```

**Com libFuzzer** (alternativa):

```bash
clang++ -std=c++20 -fsanitize=fuzzer,address \
    -o fuzz_libfuzzer fuzz_target.cpp -I src
./fuzz_libfuzzer -max_total_time=300 fuzz/corpus/
```

**Critério de aprovação:** sem crashes após 5 minutos de execução com corpus variado.

**Por que é crítico:** inputs externos (APIs, usuário) são vetores primários de ataque.

---

## T4 — Análise Dinâmica de Memória

**Objetivo:** detectar vazamentos de memória, acessos inválidos e comportamento indefinido em runtime.

**Ferramentas:** AddressSanitizer (ASan) + UndefinedBehaviorSanitizer (UBSan).

```bash
# Build com sanitizers
cmake -B build_asan \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build_asan -j$(nproc)

# Executar com variáveis de ambiente dos sanitizers
ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:log_path=reports/asan" \
UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0:log_path=reports/ubsan" \
./build_asan/[SEU_EXECUTÁVEL] 2>&1 | tee reports/sanitizers.txt

# Verificar relatórios gerados
ls reports/asan.* reports/ubsan.*
```

**Com Valgrind** (alternativa mais lenta, sem recompilação):

```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --xml=yes \
         --xml-file=reports/valgrind.xml \
         ./build/[SEU_EXECUTÁVEL]
```

**Critério de aprovação:** zero `ERROR SUMMARY` do ASan. Zero `definitely lost` no Valgrind.

**Por que é crítico:** vazamentos de memória causam degradação e crashes em produção.

---

## T5 — Scanning de Dependências

**Objetivo:** identificar dependências vulneráveis ou desatualizadas no projeto.

**Ferramentas:** `trivy` + `dependency-check`.

```bash
# trivy — scan do sistema de arquivos e imagem
trivy fs . \
    --severity HIGH,CRITICAL \
    --format json \
    --output reports/trivy_fs.json

trivy fs . \
    --severity HIGH,CRITICAL \
    --format table \
    2>&1 | tee reports/trivy_table.txt

# dependency-check (OWASP)
dependency-check \
    --project "[NOME_DO_PROJETO]" \
    --scan . \
    --format HTML \
    --out reports/dependency_check/
```

**Critério de aprovação:** zero vulnerabilidades CRITICAL. HIGH documentadas e justificadas.

**Por que é crítico:** Qt6, OpenSSL e SQLite têm histórico de CVEs exploráveis remotamente.

---

## T6 — Teste de APIs

**Objetivo:** verificar que as APIs externas respondem corretamente e que o cliente as consome sem erros.

**Ferramentas:** `hurl` + `ffuf`.

```bash
# hurl — teste declarativo de endpoints
# Criar arquivo de teste
cat > tests/api_externa.hurl << 'EOF'
GET https://sua-api-externa.com/endpoint?param=exemplo
HTTP 200
[Asserts]
header "Content-Type" contains "application/json"
jsonpath "$[0].campo" exists
jsonpath "$[0].id" exists
EOF

hurl --test tests/api_externa.hurl \
     --report-html reports/hurl_html/ \
     --report-json reports/hurl.json

# ffuf — fuzzing de endpoints da API (parâmetros inválidos)
ffuf -u "https://sua-api-externa.com/endpoint?param=FUZZ" \
     -w /usr/share/wordlists/dirb/common.txt \
     -mc 200,400,500 \
     -o reports/ffuf_api.json \
     -of json
```

**Critério de aprovação:** endpoints retornam 200 com JSON válido para entradas normais.  
Entradas malformadas não devem retornar 500 nem expor stack traces.

**Por que é crítico:** falhas de API causam crash em cascata se o cliente não trata os erros.

---

## T7 — Scanning de Binário

**Objetivo:** verificar o binário compilado em busca de funções perigosas, strings sensíveis e proteções ausentes.

**Ferramentas:** `radare2` + `ghidra` (headless).

```bash
# radare2 — análise rápida do binário
r2 -A -q -c "afl~sym; iz~password; iz~token; iz~key; iI" \
    build/[SEU_EXECUTÁVEL] 2>&1 | tee reports/r2_analysis.txt

# Verificar proteções do binário (NX, PIE, RELRO, stack canary)
r2 -q -c "iI" build/[SEU_EXECUTÁVEL] | grep -E "nx|pic|relro|canary"

# checksec (alternativa rápida)
checksec --file=build/[SEU_EXECUTÁVEL] 2>/dev/null || \
    python3 -c "
import subprocess, json
r = subprocess.run(['readelf','-l','build/[SEU_EXECUTÁVEL]'], capture_output=True, text=True)
print('GNU_STACK' in r.stdout and 'RWE' not in r.stdout and 'NX: habilitado' or 'NX: DESABILITADO')
"

# ghidra headless (requer ghidra instalado)
ghidra_headless /tmp/ghidra_proj [SEU_EXECUTÁVEL] \
    -import build/[SEU_EXECUTÁVEL] \
    -postScript HeadlessAnalyzer.java \
    -scriptLog reports/ghidra.log 2>/dev/null || \
    echo "ghidra não instalado — use r2 como fallback"
```

**Critério de aprovação:** binário com NX habilitado, PIE ativo, RELRO completo, sem strings de senha/token em texto claro.

**Por que é crítico:** proteções do binário são a última linha de defesa contra exploits.

---

## T8 — Verificação de Secrets

**Objetivo:** garantir que nenhuma credencial, token ou chave privada foi commitada no repositório.

**Ferramentas:** `gitleaks` + `trufflehog`.

```bash
# gitleaks — escaneia todo o histórico git
gitleaks detect \
    --source . \
    --report-format json \
    --report-path reports/gitleaks.json \
    --verbose 2>&1 | tee reports/gitleaks_output.txt

# trufflehog — varredura com verificação de validade das credenciais
trufflehog filesystem . \
    --json 2>&1 | tee reports/trufflehog.json

# Verificar resultados
python3 -c "
import json, sys
with open('reports/gitleaks.json') as f:
    data = json.load(f)
if data:
    print(f'ALERTA: {len(data)} secrets encontrados!')
    sys.exit(1)
else:
    print('OK: nenhum secret detectado')
" 2>/dev/null || echo "Verifique reports/gitleaks.json manualmente"
```

**Critério de aprovação:** zero secrets detectados em qualquer commit do histórico.

**Por que é crítico:** tokens commitados são explorados em minutos por bots de varredura de repositórios.

---

## T9 — Teste de Rede

**Objetivo:** verificar a superfície de ataque de rede da aplicação em execução.

**Ferramenta:** `nmap`.

```bash
# Verificar portas abertas pelo processo da aplicação
APP_PID=$(pgrep -f [SEU_EXECUTÁVEL]) && echo "PID: $APP_PID"

# Listar conexões de rede abertas pelo processo
ss -tupn | grep "$APP_PID" | tee reports/conexoes_rede.txt

# nmap no localhost (verificar se a app abre algum servidor)
nmap -sV -p 1-65535 localhost \
    --open \
    -oN reports/nmap_localhost.txt

# Verificar hosts externos que a app conecta
strace -f -e trace=network -p "$APP_PID" 2>&1 | \
    grep -E "connect|sendto" | tee reports/strace_rede.txt
```

**Critério de aprovação:** aplicação não deve abrir portas de escuta. Conexões apenas para APIs conhecidas do projeto.

**Por que é crítico:** portas abertas inesperadas indicam backdoor ou configuração incorreta.

---

## T10 — SQL Injection

**Objetivo:** verificar que as queries SQLite são seguras contra injeção de SQL.

**Ferramenta:** `sqlmap` + revisão manual de prepared statements.

```bash
# Identificar o arquivo do banco de dados SQLite
find . -name "*.db" -o -name "*.sqlite" 2>/dev/null

# sqlmap contra banco local (se a app expuser alguma interface HTTP)
# Adapte a URL e parâmetro conforme o projeto
sqlmap -u "http://localhost:PORT/api?query=exemplo" \
       --dbms=sqlite \
       --level=5 \
       --risk=3 \
       --batch \
       --output-dir=reports/sqlmap/ 2>/dev/null || \
    echo "Sem interface HTTP — verificar manualmente"

# Revisão manual: buscar queries sem prepared statements
grep -rn "exec\|query\|prepare" src/ | \
    grep -v "bindValue\|addBindValue\|:param\|?" | \
    grep -v "^Binary" | tee reports/sql_review.txt

# Verificar uso correto de QSqlQuery com bind
echo "--- Prepared statements encontrados ---"
grep -rn "bindValue\|addBindValue" src/ | wc -l
echo "--- Queries com concatenação de string (risco) ---"
grep -rn "\.exec(.*+\|query.*+\|QString.*sql" src/ | tee -a reports/sql_review.txt
```

**Critério de aprovação:** todas as queries usam prepared statements (`bindValue`). Zero concatenação de input do usuário em SQL.

**Por que é crítico:** SQL injection em SQLite permite leitura/escrita arbitrária do banco de dados local.

---

## T11 — Fuzzing de Protocolos de Rede

**Objetivo:** verificar robustez do cliente HTTP ao receber respostas malformadas ou inesperadas das APIs.

**Ferramentas:** `afl++` ou `libFuzzer` adaptado para respostas HTTP.

```bash
# Criar servidor mock que retorna respostas malformadas
cat > /tmp/mock_server.py << 'EOF'
#!/usr/bin/env python3
"""Servidor mock que responde com payloads malformados para testar robustez do cliente."""
import http.server, random, string

PAYLOADS = [
    b'',                          # resposta vazia
    b'null',                      # JSON nulo
    b'{"campo": ' + b'A'*100000,  # JSON truncado gigante
    b'<html>erro 500</html>',     # HTML onde se espera JSON
    b'\x00\xFF\xFE' * 100,       # bytes inválidos
    b'[]',                        # array vazio
    b'[{"id": null, "campo": null}]',  # campos nulos
]

class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(random.choice(PAYLOADS))
    def log_message(self, *args): pass

http.server.HTTPServer(('localhost', 18080), Handler).serve_forever()
EOF
python3 /tmp/mock_server.py &
MOCK_PID=$!

# Executar aplicação apontando para o mock (ajuste variável de ambiente ou config)
# Observar se crasha ou trata o erro graciosamente
timeout 30 ./build/[SEU_EXECUTÁVEL] --api-url=http://localhost:18080 2>&1 | \
    tee reports/fuzzing_rede.txt || true

kill $MOCK_PID 2>/dev/null
```

**Critério de aprovação:** aplicação não crasha. Exibe mensagem de erro ao usuário para qualquer resposta inválida.

**Por que é crítico:** APIs externas podem retornar respostas inesperadas por falha, manutenção ou ataque MITM.

---

## T12 — Busca de CVEs nas Dependências

**Objetivo:** identificar vulnerabilidades conhecidas (CVE) nas bibliotecas usadas pelo projeto.

**Ferramentas:** `trivy` + `grype` + consulta a NVD/OSV.

```bash
# trivy — varredura de CVEs no sistema de arquivos
trivy fs . \
    --severity CRITICAL,HIGH,MEDIUM \
    --vuln-type library \
    --format table \
    2>&1 | tee reports/cve_trivy.txt

trivy fs . \
    --severity CRITICAL,HIGH,MEDIUM \
    --format json \
    --output reports/cve_trivy.json

# grype — varredura alternativa com base de dados Grype
grype dir:. \
    --scope all-layers \
    -o table 2>&1 | tee reports/cve_grype.txt

grype dir:. \
    -o json > reports/cve_grype.json

# Verificar versões das dependências principais manualmente
echo "=== Versões instaladas ===" | tee reports/versoes_deps.txt
pkg-config --modversion Qt6Core 2>/dev/null   | xargs echo "Qt6Core:"     | tee -a reports/versoes_deps.txt
openssl version                                                              | tee -a reports/versoes_deps.txt
sqlite3 --version                                                            | tee -a reports/versoes_deps.txt

# Consultar OSV (Open Source Vulnerabilities) via API
python3 << 'EOF'
import urllib.request, json

deps = [
    {"package": {"name": "openssl",  "ecosystem": "Linux"}},
    {"package": {"name": "sqlite",   "ecosystem": "Linux"}},
]
for dep in deps:
    req = urllib.request.Request(
        "https://api.osv.dev/v1/query",
        data=json.dumps(dep).encode(),
        headers={"Content-Type": "application/json"},
        method="POST"
    )
    try:
        with urllib.request.urlopen(req, timeout=10) as r:
            data = json.load(r)
            vulns = data.get("vulns", [])
            name = dep["package"]["name"]
            print(f"{name}: {len(vulns)} vuln(s) encontrada(s)")
    except Exception as e:
        print(f"Erro consultando OSV para {dep['package']['name']}: {e}")
EOF
```

**Critério de aprovação:** zero CVEs CRITICAL sem patch disponível. HIGH documentadas com prazo de atualização.

**Por que é crítico:** Qt6 e OpenSSL têm CVEs históricos com CVSS ≥ 9.0 (execução remota de código).

---

## A1 — Descoberta e Modelagem

**Objetivo:** construir mapa mental completo do projeto antes de qualquer análise.

```bash
# Estrutura de diretórios
find src -type f | sort | tee reports/estrutura_arquivos.txt

# Contagem de linhas por linguagem
cloc src/ --by-file --csv --out=reports/cloc.csv
cloc src/ | tee reports/cloc_resumo.txt

# Módulos e suas dependências de include
grep -rn "#include" src/ | \
    grep -v "/usr\|Qt" | \
    sed 's|.*#include.*"\(.*\)".*|\1|' | \
    sort | uniq -c | sort -rn | tee reports/includes_internos.txt

# Identificar binários gerados pelo CMake
find build -type f -executable -not -name "*.so" -not -name "*.cmake" \
    2>/dev/null | tee reports/binarios.txt

# Presença de testes
find . -name "test_*.cpp" -o -name "*_test.cpp" -o -name "CMakeLists.txt" \
    -path "*/tests/*" | tee reports/arquivos_teste.txt
```

**Entregável:** mapa de módulos com responsabilidades, lista de binários, status de cobertura de testes.

---

## A2 — Auditoria de Arquitetura e Camadas

**Objetivo:** validar que nenhuma camada viola as regras de dependência da arquitetura.

```bash
# Detectar chamadas diretas de API na camada de apresentação (violação)
echo "=== Violações: chamadas HTTP em widgets ===" | tee reports/violacoes_arq.txt
grep -rn "QNetworkAccessManager\|ClienteHttp\|GET\|POST" \
    src/apresentacao/ | tee -a reports/violacoes_arq.txt

# Detectar acesso a banco na camada de domínio (violação)
echo "=== Violações: SQL no domínio ===" | tee -a reports/violacoes_arq.txt
grep -rn "QSqlQuery\|sqlite\|QSqlDatabase" \
    src/dominio/ | tee -a reports/violacoes_arq.txt

# Detectar include de Qt em domínio puro (violação leve)
echo "=== Avisos: Qt em domínio ===" | tee -a reports/violacoes_arq.txt
grep -rn "#include <Q" src/dominio/entidades/ | \
    grep -v "QString\|QVector\|QStringList" | tee -a reports/violacoes_arq.txt

# Validar que serviço não importa apresentação
echo "=== Violações: servico importa apresentacao ===" | tee -a reports/violacoes_arq.txt
grep -rn "#include.*apresentacao" src/servico/ | tee -a reports/violacoes_arq.txt
```

**Critério de aprovação:** zero violações críticas (API em widget, SQL em domínio).

---

## A3 — UI/UX e Acessibilidade

**Objetivo:** verificar qualidade visual, contraste, navegação por teclado e conformidade com boas práticas.

```bash
# Verificar contraste de cores (via Python com wcag-contrast)
pip3 install wcag-contrast-ratio 2>/dev/null
python3 << 'EOF'
import wcag_contrast_ratio as contrast

# Pares de cores do tema do projeto — adapte conforme o projeto
pares = [
    ("Texto escuro / fundo escuro",    "#EFF0F1", "#31363B"),
    ("Texto claro / fundo claro",      "#232629", "#EFF0F1"),
    ("Acento / fundo escuro",          "#3DAEE9", "#31363B"),
    ("Negativo / fundo claro",         "#DA4453", "#EFF0F1"),
]
for nome, fg, bg in pares:
    fg_rgb = tuple(int(fg[i:i+2], 16)/255 for i in (1,3,5))
    bg_rgb = tuple(int(bg[i:i+2], 16)/255 for i in (1,3,5))
    ratio = contrast.rgb(fg_rgb, bg_rgb)
    aa = "✅ AA" if ratio >= 4.5 else "❌ falha AA"
    print(f"{nome}: {ratio:.2f}:1 {aa}")
EOF

# Verificar uso de tooltips nos botões (acessibilidade)
grep -rn "setToolTip\|setAccessibleName\|setWhatsThis" \
    src/apresentacao/ | tee reports/acessibilidade.txt

# Verificar navegação por teclado (Tab order)
grep -rn "setTabOrder\|setFocusPolicy\|Qt::NoFocus\|Qt::TabFocus" \
    src/apresentacao/ | tee -a reports/acessibilidade.txt
```

**Critério de aprovação:** todos os pares de cores com ratio ≥ 4.5:1 (WCAG AA). Todos os botões com tooltip.

---

## A4 — QA Geral C++

**Objetivo:** build estrito sem warnings + análise estática básica.

```bash
# Build com flags estritas
cmake -B build_strict \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-std=c++20 -Wall -Wextra -Wpedantic -Wshadow \
        -Wnull-dereference -Wdouble-promotion -Wformat=2 \
        -Wimplicit-fallthrough -Wno-unused-parameter"
cmake --build build_strict -j$(nproc) 2>&1 | tee reports/build_strict.txt

# Contar warnings
WARNINGS=$(grep -c "warning:" reports/build_strict.txt || true)
echo "Total de warnings: $WARNINGS"
[ "$WARNINGS" -eq 0 ] && echo "✅ Zero warnings" || echo "❌ $WARNINGS warnings encontrados"

# Executar testes se existirem
cd build_strict && ctest --output-on-failure 2>&1 | tee ../reports/ctest.txt && cd ..
```

**Critério de aprovação:** zero erros de compilação. Warnings documentados e justificados.

---

## A5 — Análise Estática e Dinâmica C/C++

**Objetivo:** varredura profunda em todos os arquivos `.cpp` em busca de undefined behavior e memory issues.

```bash
# cppcheck completo com saída estruturada
cppcheck \
    --enable=all \
    --inconclusive \
    --suppress=missingIncludeSystem \
    --suppress=unmatchedSuppression \
    --error-exitcode=1 \
    --xml --xml-version=2 \
    --std=c++20 \
    src/ 2> reports/cppcheck_full.xml

cppcheck-htmlreport \
    --file=reports/cppcheck_full.xml \
    --report-dir=reports/cppcheck_full_html \
    --source-dir=.

# clang-tidy com todos os checks
find src -name "*.cpp" | xargs -P$(nproc) -I{} \
    clang-tidy {} \
    --checks="-*,clang-analyzer-*,bugprone-*,performance-*,portability-*" \
    -- -std=c++20 -I src \
    2>&1 | tee reports/clang_tidy_full.txt

# Resumo de problemas
echo "--- cppcheck erros ---"
grep -c "<error " reports/cppcheck_full.xml || echo "0"
echo "--- clang-tidy warnings ---"
grep -c "warning:" reports/clang_tidy_full.txt || echo "0"
```

---

## A6 — Cobertura de Testes

**Objetivo:** medir qual percentual do código é exercitado pelos testes.

```bash
# Build com instrumentação de cobertura
cmake -B build_cov \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
    -DBUILD_TESTING=ON
cmake --build build_cov -j$(nproc)

# Executar testes para gerar dados de cobertura
cd build_cov && ctest --output-on-failure && cd ..

# Coletar dados com lcov
lcov --capture \
     --directory build_cov \
     --output-file reports/cobertura.info \
     --no-external

# Remover arquivos de terceiros e gerados automaticamente
lcov --remove reports/cobertura.info \
     "*/moc_*" "*/qrc_*" "*/ui_*" "/usr/*" \
     --output-file reports/cobertura_filtrada.info

# Gerar relatório HTML
genhtml reports/cobertura_filtrada.info \
        --output-directory reports/cobertura_html \
        --title "[NOME_DO_PROJETO] — Cobertura de Testes"

# Exibir resumo
lcov --summary reports/cobertura_filtrada.info 2>&1 | grep -E "lines|functions|branches"
```

**Critério de aprovação:** cobertura de linhas ≥ 70% nos módulos `dominio/` e `servico/`.

---

## A7 — Dependências e Acoplamento

**Objetivo:** detectar ciclos de dependência e módulos supercoplados.

```bash
# Gerar grafo de dependências de includes
python3 << 'EOF'
import os, re
from pathlib import Path

includes = {}
for cpp in Path("src").rglob("*.cpp"):
    deps = []
    for line in cpp.read_text(errors='ignore').splitlines():
        m = re.match(r'\s*#include\s+"([^"]+)"', line)
        if m:
            deps.append(m.group(1))
    includes[str(cpp)] = deps

# Detectar includes mais frequentes
from collections import Counter
all_deps = [d for deps in includes.values() for d in deps]
print("=== Top 10 headers mais incluídos ===")
for dep, count in Counter(all_deps).most_common(10):
    print(f"  {count:3d}x  {dep}")

# Detectar ciclos simples (A inclui B, B inclui A)
print("\n=== Possíveis ciclos de dependência ===")
for file, deps in includes.items():
    for dep in deps:
        dep_full = f"src/{dep}"
        if dep_full in includes:
            if any(file.endswith(d) for d in includes.get(dep_full, [])):
                print(f"  CICLO: {file} ↔ {dep_full}")
EOF


# Gerar grafo visual com graphviz (opcional)
which dot 2>/dev/null && python3 << 'GVEOF'
import os, re
from pathlib import Path

lines = ["digraph includes {", "  rankdir=LR;", "  node [shape=box];"]
for cpp in Path("src").rglob("*.cpp"):
    src_name = cpp.stem
    for line in cpp.read_text(errors='ignore').splitlines():
        m = re.match(r'\s*#include\s+"([^"]+)"', line)
        if m:
            dep = Path(m.group(1)).stem
            lines.append(f'  "{src_name}" -> "{dep}";')
lines.append("}")
Path("reports/dep_graph.dot").write_text("\n".join(lines))
os.system("dot -Tsvg reports/dep_graph.dot -o reports/dep_graph.svg")
print("Grafo gerado: reports/dep_graph.svg")
GVEOF
```

**Critério de aprovação:** zero ciclos de dependência. Nenhum header incluído em mais de 15 arquivos sem justificativa.

---

## A8 — Consistência .h vs .cpp

**Objetivo:** garantir que declarações e implementações estão alinhadas.

```bash

python3 << 'EOF'
import re
from pathlib import Path

problemas = []
for h in Path("src").rglob("*.h"):
    cpp = h.with_suffix(".cpp")
    if not cpp.exists():
        continue

    h_text   = h.read_text(errors='ignore')
    cpp_text = cpp.read_text(errors='ignore')

    # Funções declaradas no .h
    decls = re.findall(r'^\s+(?:virtual\s+)?[\w:<>*&]+\s+(\w+)\s*\([^)]*\)\s*(?:const\s*)?(?:override\s*)?;',
                        h_text, re.MULTILINE)
    # Funções implementadas no .cpp
    impls = re.findall(r'\w+::\s*(\w+)\s*\(', cpp_text)

    for d in decls:
        if d not in impls and d not in ('~'+h.stem, h.stem):
            problemas.append(f"DECLARADA SEM IMPL: {h.name}::{d}")

if problemas:
    print(f"❌ {len(problemas)} inconsistências encontradas:")
    for p in problemas: print(f"  {p}")
else:
    print("✅ Nenhuma inconsistência .h/.cpp detectada")
EOF | tee reports/consistencia_hcpp.txt

# Verificar const-correctness básica
grep -rn "void.*get\|QString.*get\|int.*get" src/ | \
    grep -v "const$\|const ;" | tee reports/const_correctness.txt
    
```


## A9 — Análise Arquitetural Geral

**Objetivo:** classificar a saúde arquitetural do projeto (coesão, acoplamento, SOLID).

```bash
python3 << 'EOF'
from pathlib import Path
import re

print("=== Análise de Complexidade e Coesão ===\n")
for cpp in sorted(Path("src").rglob("*.cpp")):
    text = cpp.read_text(errors='ignore')
    lines       = len(text.splitlines())
    methods     = len(re.findall(r'::\w+\s*\(', text))
    classes     = len(re.findall(r'\bclass\b', text))
    todos       = len(re.findall(r'TODO|FIXME|HACK|XXX', text))
    long_method = max((len(b) for b in re.split(r'\{', text)), default=0)

    issues = []
    if lines > 400:          issues.append(f"arquivo grande ({lines} linhas)")
    if methods > 20:         issues.append(f"muitos métodos ({methods})")
    if classes > 3:          issues.append(f"múltiplas classes ({classes}) — SRP?")
    if todos > 0:            issues.append(f"{todos} TODO(s)")

    if issues:
        print(f"⚠  {cpp.relative_to('src')}: {' | '.join(issues)}")
    else:
        print(f"✅ {cpp.relative_to('src')}: ok ({lines} linhas, {methods} métodos)")
EOF | tee reports/analise_arq.txt
```

---

## A10 — Relatório Final de Auditoria

**Objetivo:** consolidar todos os resultados em um único documento com score, problemas e patches.

```bash
python3 << 'REPORT'
import json, os
from pathlib import Path
from datetime import date

r = "reports"
scores = {}

# T2 — Análise estática
try:
    cppcheck_erros = sum(1 for line in Path(f"{r}/cppcheck_full.xml").read_text().splitlines()
                         if "<error " in line)
    scores["T2_cppcheck"] = max(0, 100 - cppcheck_erros * 10)
except: scores["T2_cppcheck"] = "N/A"

# T8 — Secrets
try:
    secrets = json.loads(Path(f"{r}/gitleaks.json").read_text())
    scores["T8_secrets"] = 100 if not secrets else 0
except: scores["T8_secrets"] = "N/A"

# T12 — CVEs
try:
    cve_data = json.loads(Path(f"{r}/cve_trivy.json").read_text())
    crits = sum(1 for v in cve_data.get("Results", [])
                for vuln in v.get("Vulnerabilities", [])
                if vuln.get("Severity") == "CRITICAL")
    scores["T12_cve"] = 100 if crits == 0 else max(0, 100 - crits * 25)
except: scores["T12_cve"] = "N/A"

print(f"""
╔══════════════════════════════════════════════════════════════╗
║           RELATÓRIO FINAL DE AUDITORIA — {date.today()}        ║
╠══════════════════════════════════════════════════════════════╣
║  SCORES PARCIAIS                                             ║
║  T2  Análise estática (cppcheck):  {scores.get('T2_cppcheck'):>5}                    ║
║  T8  Secrets no repositório:       {scores.get('T8_secrets'):>5}                    ║
║  T12 CVEs nas dependências:        {scores.get('T12_cve'):>5}                    ║
╠══════════════════════════════════════════════════════════════╣
║  Relatórios gerados em: reports/                            ║
║  - cppcheck_full.xml / cppcheck_full_html/                  ║
║  - clang_tidy_full.txt                                      ║
║  - cobertura_html/                                          ║
║  - gitleaks.json / trufflehog.json                          ║
║  - cve_trivy.json / cve_grype.json                          ║
║  - dep_graph.svg                                            ║
║  - violacoes_arq.txt                                        ║
║  - analise_arq.txt                                          ║
╚══════════════════════════════════════════════════════════════╝
""")
REPORT
```

**Entregável obrigatório:** documento com:
1. Score global por área (UX, QA, C/C++, Arquitetura) — 0 a 100
2. Percentual de cobertura de testes por módulo
3. Sumário de problemas: `CRÍTICO / MÉDIO / BAIXO`
4. Top 5 issues mais impactantes com arquivo + linha
5. Análise de ciclos de dependência
6. Patches no formato unificado (`--- a/file +++ b/file`)

---

## Classificação de Problemas

| Severidade | Critério | Ação |
|-----------|---------|------|
| **CRÍTICO** | Crash, CVE exploitável, ciclo de dependência, secret exposto, zero cobertura em módulo crítico, violação de camada | Bloquear release. Corrigir imediatamente. |
| **MÉDIO** | Warning de compilação, alto acoplamento, coverage < 50%, performance degradada | Corrigir antes do próximo release. |
| **BAIXO** | Estilo, organização, TODO aberto, inline desnecessário | Corrigir no backlog. |

---

## Formato de Patch

Todo problema com correção deve incluir patch unificado:

```diff
--- a/src/[caminho_do_arquivo].cpp
+++ b/src/[caminho_do_arquivo].cpp
@@ -45,7 +45,7 @@
-    void processar(Dados d);
+    void processar(const Dados& d);
```

---

*Documento genérico para qualquer projeto C++/Qt/CMake — adapte caminhos, módulos, executáveis e ferramentas conforme o projeto alvo.*  
*Ferramentas: cppcheck · clang-tidy · afl++ · ASan/UBSan · trivy · grype · gitleaks · nmap · sqlmap · hurl · ffuf · radare2 · lcov · gcovr*
```
