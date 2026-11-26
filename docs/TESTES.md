# Guia Completo de Testes, Qualidade e Auditoria

> Documento instrucional para qualquer agente de IA ou engenheiro executar a suíte
> completa de verificação em projetos **C · C++ · Python · Rust · Node.js/TypeScript**.
> As seções T1–T12 e A1–A10 cobrem C/C++/Qt. As seções T13–T15 e A11–A13 cobrem os demais stacks.
> Adapte os caminhos e nomes de módulos conforme o projeto alvo.
>
> **Nota do my_comp (MC-01):** as metas numéricas de cobertura deste documento (70%, 80%, 90%)
> são referência genérica. No my_comp vale a [MC-07](../GODS_LAWS.md#mc-07): cobertura **medida,
> sem meta numérica** (L-35). Em conflito, o `GODS_LAWS.md` prevalece.

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

**Stack extensions (Python · Rust · Node.js/TS):**

26. [T13 — Python Testing Suite](#t13--python-testing-suite)
27. [T14 — Rust Testing Suite](#t14--rust-testing-suite)
28. [T15 — Node.js / TypeScript Testing Suite](#t15--nodejs--typescript-testing-suite)
29. [A11 — QA Python](#a11--qa-python)
30. [A12 — QA Rust](#a12--qa-rust)
31. [A13 — QA Node.js / TypeScript](#a13--qa-nodejs--typescript)

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
./build_test/tests/test_modulo_alvo
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
    src/infraestrutura/repositorios/repositorio_api.cpp \
    -I src -lQt6Core -lQt6Network

# Criar corpus inicial
mkdir -p fuzz/corpus fuzz/findings
echo '{"nome":"item_teste"}' > fuzz/corpus/seed1.json
echo '{"nome":""}' > fuzz/corpus/seed2.json
echo '{"nome":"A"}' > fuzz/corpus/seed3.json

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
./build_asan/meu_app 2>&1 | tee reports/sanitizers.txt

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
         ./build/meu_app
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
    --project "meu_app" \
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
GET https://api.exemplo.com/v1/itens?nome=teste
HTTP 200
[Asserts]
header "Content-Type" contains "application/json"
jsonpath "$[0].name" exists
jsonpath "$[0].id" exists
EOF

hurl --test tests/api_externa.hurl \
     --report-html reports/hurl_html/ \
     --report-json reports/hurl.json

# ffuf — fuzzing de endpoints da API (parâmetros inválidos)
ffuf -u "https://api.exemplo.com/v1/itens?nome=FUZZ" \
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
    build/meu_app 2>&1 | tee reports/r2_analysis.txt

# Verificar proteções do binário (NX, PIE, RELRO, stack canary)
r2 -q -c "iI" build/meu_app | grep -E "nx|pic|relro|canary"

# checksec (alternativa rápida)
checksec --file=build/meu_app 2>/dev/null || \
    python3 -c "
import subprocess, json
r = subprocess.run(['readelf','-l','build/meu_app'], capture_output=True, text=True)
print('GNU_STACK' in r.stdout and 'RWE' not in r.stdout and 'NX: habilitado' or 'NX: DESABILITADO')
"

# ghidra headless (requer ghidra instalado)
ghidra_headless /tmp/ghidra_proj meu_app \
    -import build/meu_app \
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
APP_PID=$(pgrep -f meu_app) && echo "PID: $APP_PID"

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

**Critério de aprovação:** aplicação não deve abrir portas de escuta. Conexões apenas para APIs conhecidas (api.exemplo.com e demais APIs configuradas no projeto).

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
sqlmap -u "http://localhost:PORT/api?q=teste" \
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
    b'{"name": ' + b'A'*100000,  # JSON truncado gigante
    b'<html>erro 500</html>',     # HTML onde se espera JSON
    b'\x00\xFF\xFE' * 100,       # bytes inválidos
    b'[]',                        # array vazio
    b'[{"id": null, "name": null}]',  # campos nulos
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
timeout 30 ./build/meu_app --api-url=http://localhost:18080 2>&1 | \
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

# Pares de cores do tema Breeze (texto/fundo) — adapte conforme o projeto
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
        --title "Cobertura de Testes"

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

---

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

## T13 — Python Testing Suite

> Substitui T1–T4 + T5/T12 para projetos Python. Execute na raiz do projeto com virtualenv ativo.

### T13.1 — Testes Unitários e de Integração (pytest)

```bash
pip install pytest pytest-cov pytest-asyncio

# Executar todos os testes
pytest -v --tb=short

# Com cobertura (gera relatório HTML em htmlcov/)
pytest --cov=src --cov-report=html --cov-report=term-missing --cov-fail-under=70

# Testes async (FastAPI, asyncio)
pytest -v --asyncio-mode=auto
```

**Critério de aprovação:** 0 falhas. Cobertura ≥ 70% nos módulos de domínio e serviço.

### T13.2 — Análise Estática

```bash
pip install ruff mypy bandit

# ruff — lint + formatação (substitui flake8 + isort + pyupgrade)
ruff check src/ --output-format=json > reports/ruff.json
ruff check src/

# mypy — verificação de tipos estrita
mypy src/ \
    --strict \
    --ignore-missing-imports \
    --disallow-untyped-defs \
    2>&1 | tee reports/mypy.txt

# bandit — análise de segurança (injeção, uso de eval, MD5, etc.)
bandit -r src/ \
    -f json -o reports/bandit.json \
    -l  # apenas medium/high
bandit -r src/ 2>&1 | tee reports/bandit_resumo.txt
```

**Critério de aprovação:** zero erros mypy. Zero bandit HIGH/CRITICAL. ruff sem erros de segurança.

### T13.3 — Property-Based Testing / Fuzzing (Hypothesis)

```bash
pip install hypothesis

# Exemplo de teste Hypothesis — adicionar ao pytest
# tests/test_propriedades.py:
# from hypothesis import given, settings, strategies as st
#
# @given(st.integers(), st.integers())
# def test_somar_comutativo(a, b):
#     assert somar(a, b) == somar(b, a)
#
# @given(st.text(min_size=1, max_size=1000))
# @settings(max_examples=500)
# def test_processar_nunca_crasha(entrada):
#     try:
#         processar(entrada)
#     except ValueError:
#         pass  # erros de validação são esperados; crashes não

pytest tests/test_propriedades.py -v --hypothesis-show-statistics
```

**Critério de aprovação:** zero falhas inesperadas. Hipóteses executadas ≥ 100 exemplos por propriedade.

### T13.4 — Memória e Performance

```bash
pip install memory-profiler

# tracemalloc — detectar crescimento de memória em testes
python3 -c "
import tracemalloc, sys
tracemalloc.start()
# importar e exercitar o módulo alvo
import src.servico as s
s.executar()
snapshot = tracemalloc.take_snapshot()
top = snapshot.statistics('lineno')[:10]
for stat in top:
    print(stat)
"

# memory-profiler — perfil linha a linha
python3 -m memory_profiler src/servico.py 2>&1 | tee reports/memory_profile.txt

# Verificar leaks em testes de longa duração
pytest --memray 2>/dev/null || echo "pip install pytest-memray para análise de leaks"
```

### T13.5 — Dependências e CVEs

```bash
pip install pip-audit safety

# pip-audit — CVEs nas dependências instaladas (usa OSV + PyPI Advisory DB)
pip-audit --format json -o reports/pip_audit.json
pip-audit 2>&1 | tee reports/pip_audit_resumo.txt

# safety — verifica requirements*.txt contra base de dados Safety DB
safety check -r requirements.txt \
    --json --output reports/safety.json 2>/dev/null || true
safety check -r requirements.txt 2>&1 | tee reports/safety_resumo.txt

# Verificar dependências desatualizadas
pip list --outdated --format=json > reports/pip_outdated.json
```

**Critério de aprovação:** zero CVEs CRITICAL/HIGH sem patch. Dependências CRITICAL atualizadas em ≤ 48 h.

---

## T14 — Rust Testing Suite

> Substitui T1–T4 + T5/T12 para projetos Rust. Execute na raiz do workspace.

### T14.1 — Testes Unitários e de Integração (cargo test)

```bash
# Executar todos os testes (unit + integration + doc tests)
cargo test -- --nocapture

# Testes com output detalhado
cargo test -- --nocapture 2>&1 | tee reports/cargo_test.txt

# Apenas testes de integração (diretório tests/)
cargo test --test '*'

# Benchmarks (requer nightly ou criterion)
cargo bench 2>&1 | tee reports/bench.txt
```

**Critério de aprovação:** 0 falhas. Doc tests passando. `cargo test` clean em `--release` também.

### T14.2 — Análise Estática (clippy + rustfmt)

```bash
# clippy — lint estrito (trata warnings como erros)
cargo clippy --all-targets --all-features -- \
    -D warnings \
    -D clippy::pedantic \
    -D clippy::nursery \
    -A clippy::module_name_repetitions \
    2>&1 | tee reports/clippy.txt

# rustfmt — verificação de formatação (não formata, apenas verifica)
cargo fmt --all -- --check 2>&1 | tee reports/rustfmt.txt

# Verificar uso de unsafe (todo unsafe precisa de justificativa)
grep -rn "unsafe" src/ --include="*.rs" | tee reports/unsafe_blocks.txt
echo "Total de blocos unsafe: $(grep -rn 'unsafe' src/ --include='*.rs' | wc -l)"
```

**Critério de aprovação:** zero warnings clippy. `rustfmt --check` passa. Cada bloco `unsafe` documentado com `// SAFETY:`.

### T14.3 — Fuzzing (cargo-fuzz)

```bash
# Instalar cargo-fuzz (requer nightly)
cargo install cargo-fuzz
rustup install nightly

# Inicializar fuzzing no projeto
cargo fuzz init 2>/dev/null || true

# Criar fuzz target (fuzz/fuzz_targets/fuzz_processar.rs):
# #![no_main]
# use libfuzzer_sys::fuzz_target;
# fuzz_target!(|data: &[u8]| {
#     if let Ok(s) = std::str::from_utf8(data) {
#         let _ = meu_projeto::processar(s);
#     }
# });

# Executar por 5 minutos
cargo +nightly fuzz run fuzz_processar -- -max_total_time=300 \
    2>&1 | tee reports/cargo_fuzz.txt

# Verificar crashes
ls fuzz/artifacts/fuzz_processar/ 2>/dev/null && \
    echo "CRASHES ENCONTRADOS" || echo "Sem crashes"
```

**Critério de aprovação:** sem crashes após 5 min com corpus variado.

### T14.4 — Undefined Behavior (Miri)

```bash
# Miri — interpretador que detecta UB, acessos inválidos, data races
rustup component add miri --toolchain nightly

# Executar testes sob Miri
cargo +nightly miri test 2>&1 | tee reports/miri.txt

# Verificar data races com ThreadSanitizer
RUSTFLAGS="-Z sanitizer=thread" cargo +nightly test \
    --target x86_64-unknown-linux-gnu \
    2>&1 | tee reports/tsan_rust.txt
```

**Critério de aprovação:** zero erros Miri. Nenhum data race detectado pelo TSan.

### T14.5 — Dependências e CVEs (cargo audit)

```bash
cargo install cargo-audit cargo-deny

# cargo audit — CVEs via RustSec Advisory Database
cargo audit --json > reports/cargo_audit.json 2>/dev/null
cargo audit 2>&1 | tee reports/cargo_audit_resumo.txt

# cargo deny — licenças, duplicatas, yanked crates
cargo deny check 2>&1 | tee reports/cargo_deny.txt

# Dependências desatualizadas
cargo install cargo-outdated 2>/dev/null
cargo outdated 2>&1 | tee reports/cargo_outdated.txt
```

**Critério de aprovação:** zero advisories UNMAINTAINED/VULNERABILITY críticos. Nenhum crate yanked em uso.

### T14.6 — Cobertura

```bash
cargo install cargo-tarpaulin

# tarpaulin — cobertura de linhas
cargo tarpaulin \
    --out Html --output-dir reports/cobertura_rust/ \
    --out Lcov --output-dir reports/ \
    --exclude-files "tests/*" \
    2>&1 | tee reports/tarpaulin.txt

# Exibir percentual
grep "Coverage" reports/tarpaulin.txt
```

**Critério de aprovação:** cobertura de linhas ≥ 70% nos módulos de domínio.

---

## T15 — Node.js / TypeScript Testing Suite

> Substitui T1–T4 + T5/T12 para projetos Node.js/TypeScript. Execute na raiz do projeto com `node` e gerenciador de pacotes instalados.

### T15.1 — Testes Unitários e de Integração (Jest / Vitest)

```bash
# Jest (projetos existentes)
npx jest --coverage --coverageThreshold='{"global":{"lines":70}}' \
    2>&1 | tee reports/jest.txt

# Vitest (projetos Vite/ESM modernos)
npx vitest run --coverage 2>&1 | tee reports/vitest.txt

# Supertest — testes de integração de endpoints HTTP
# (requer express/fastify + supertest instalados)
# Em test/api.test.ts:
# import request from 'supertest';
# it('GET /health retorna 200', async () => {
#   const res = await request(app).get('/health');
#   expect(res.status).toBe(200);
# });
```

**Critério de aprovação:** 0 falhas. Cobertura de linhas ≥ 70% em src/.

### T15.2 — Análise Estática (ESLint + TypeScript strict)

```bash
# TypeScript — verificação de tipos estrita (sem emitir arquivos)
npx tsc --noEmit --strict 2>&1 | tee reports/tsc.txt

# ESLint — lint com regras de segurança e qualidade
npx eslint src/ \
    --ext .ts,.tsx,.js \
    --format json -o reports/eslint.json
npx eslint src/ --ext .ts,.tsx,.js 2>&1 | tee reports/eslint_resumo.txt

# Verificar uso de any explícito (dívida de tipagem)
grep -rn ": any\|as any\|<any>" src/ --include="*.ts" | tee reports/any_types.txt
echo "Total de 'any' explícitos: $(grep -rn ': any\|as any' src/ --include='*.ts' | wc -l)"
```

**Critério de aprovação:** zero erros `tsc --strict`. Zero regras ESLint `error` ativas violadas.

### T15.3 — Property-Based Testing / Fuzzing (fast-check)

```bash
npm install --save-dev fast-check

# Exemplo de propriedade — adicionar ao Jest/Vitest:
# import fc from 'fast-check';
#
# test('somar é comutativo', () => {
#   fc.assert(fc.property(fc.integer(), fc.integer(), (a, b) => {
#     expect(somar(a, b)).toBe(somar(b, a));
#   }));
# });
#
# test('processar nunca lança exceção não tratada', () => {
#   fc.assert(fc.property(fc.string(), (s) => {
#     expect(() => processar(s)).not.toThrow(TypeError);
#   }), { numRuns: 500 });
# });

npx jest --testPathPattern="propriedades" -v 2>&1 | tee reports/fast_check.txt
```

### T15.4 — Dependências e CVEs

```bash
# npm audit — CVEs nas dependências (base NPM)
npm audit --json > reports/npm_audit.json 2>/dev/null
npm audit 2>&1 | tee reports/npm_audit_resumo.txt

# Verificar dependências desatualizadas
npm outdated --json > reports/npm_outdated.json 2>/dev/null || true

# retire.js — CVEs em dependências JS (client-side também)
npx retire --outputformat json --outputpath reports/retire.json 2>/dev/null || \
    echo "npm install -g retire para análise adicional"

# Verificar licenças das dependências
npx license-checker --json > reports/licencas.json 2>/dev/null || \
    echo "npm install -g license-checker para auditoria de licenças"
```

**Critério de aprovação:** zero CVEs HIGH/CRITICAL sem patch disponível. Nenhuma dependência com licença incompatível (GPL em projeto proprietário, etc.).

### T15.5 — Cobertura (c8 / Istanbul)

```bash
# c8 — cobertura nativa V8 (sem instrumentação)
npx c8 \
    --reporter=html --reporter=text --reporter=lcov \
    --reports-dir=reports/cobertura_node/ \
    --branches=70 --lines=70 --functions=70 \
    npx jest --no-coverage \
    2>&1 | tee reports/c8.txt

# Alternativa com nyc (Istanbul)
npx nyc \
    --reporter=html --reporter=text-summary \
    --report-dir=reports/nyc/ \
    npx jest 2>&1 | tee reports/nyc.txt
```

**Critério de aprovação:** lines/branches/functions ≥ 70% em src/.

---

## A11 — QA Python

**Objetivo:** auditoria estática e de qualidade equivalente a A4/A5 para projetos Python.

```bash
pip install ruff mypy pylint bandit radon

# ruff — lint completo com auto-fix preview
ruff check src/ --select ALL \
    --ignore ANN101,ANN401,D,ERA001 \
    --output-format=json > reports/ruff_full.json
ruff check src/ 2>&1 | tee reports/ruff_resumo.txt

# pylint — análise mais profunda de design
pylint src/ \
    --output-format=json > reports/pylint.json 2>/dev/null
pylint src/ 2>&1 | tee reports/pylint_resumo.txt

# radon — complexidade ciclomática (> 10 é dívida)
radon cc src/ -a -s | tee reports/complexidade_python.txt
radon mi src/ -s | tee reports/manutenibilidade_python.txt

# mypy estrito completo
mypy src/ \
    --strict \
    --show-error-codes \
    --show-column-numbers \
    --pretty \
    2>&1 | tee reports/mypy_full.txt

echo "=== Resumo ==="
echo "ruff erros:   $(python3 -c "import json; d=json.load(open('reports/ruff_full.json')); print(len(d))" 2>/dev/null || echo N/A)"
echo "pylint score: $(grep 'Your code has been rated' reports/pylint_resumo.txt || echo N/A)"
echo "mypy erros:   $(grep -c 'error:' reports/mypy_full.txt || echo 0)"
```

**Critério de aprovação:** pylint score ≥ 8.0. Zero erros mypy. Zero funções com complexidade ciclomática > 10 sem justificativa.

---

## A12 — QA Rust

**Objetivo:** auditoria de qualidade equivalente a A4/A5 para projetos Rust.

```bash
# Build estrito — todos os warnings são erros
RUSTFLAGS="-D warnings" cargo build --all-targets 2>&1 | tee reports/rust_build_strict.txt

# clippy com perfil máximo
cargo clippy --all-targets --all-features -- \
    -D warnings \
    -D clippy::all \
    -D clippy::pedantic \
    -W clippy::nursery \
    2>&1 | tee reports/clippy_full.txt

# rustfmt check
cargo fmt --all -- --check 2>&1 | tee reports/rustfmt.txt

# Complexidade — contar funções longas (> 50 linhas como heurística)
python3 << 'EOF'
from pathlib import Path
import re

for rs in Path("src").rglob("*.rs"):
    text = rs.read_text()
    funcs = re.split(r'\bfn \w+', text)
    for i, body in enumerate(funcs[1:], 1):
        braces, lines = 0, 0
        for line in body.splitlines():
            if '{' in line: braces += line.count('{')
            if '}' in line: braces -= line.count('}')
            lines += 1
            if braces <= 0: break
        if lines > 50:
            print(f"⚠  {rs}: função #{i} com {lines} linhas")
EOF | tee reports/funcoes_longas_rust.txt

# Verificar unsafe não documentado
python3 << 'EOF'
from pathlib import Path
import re

for rs in Path("src").rglob("*.rs"):
    lines = rs.read_text().splitlines()
    for i, line in enumerate(lines):
        if re.search(r'\bunsafe\b', line):
            contexto = lines[i-1] if i > 0 else ""
            if "SAFETY" not in contexto and "safety" not in contexto:
                print(f"❌ unsafe sem SAFETY comment: {rs}:{i+1}: {line.strip()}")
EOF | tee reports/unsafe_sem_doc.txt
```

**Critério de aprovação:** zero warnings clippy. rustfmt passa. Zero `unsafe` sem comentário `// SAFETY:`.

---

## A13 — QA Node.js / TypeScript

**Objetivo:** auditoria de qualidade equivalente a A4/A5 para projetos Node.js/TypeScript.

```bash
# TypeScript estrito
npx tsc --noEmit --strict --noImplicitReturns --noFallthroughCasesInSwitch \
    2>&1 | tee reports/tsc_strict.txt

# ESLint com regras de segurança
npx eslint src/ --ext .ts,.tsx \
    --rule '{"no-eval": "error", "no-implied-eval": "error",
             "no-new-func": "error", "no-param-reassign": "warn",
             "@typescript-eslint/no-explicit-any": "error",
             "@typescript-eslint/no-non-null-assertion": "warn"}' \
    --format json -o reports/eslint_qa.json 2>/dev/null
npx eslint src/ --ext .ts,.tsx 2>&1 | tee reports/eslint_qa_resumo.txt

# Contar 'any' explícitos — dívida de tipagem
echo "=== Contagem de 'any' ==="
grep -rn ": any\b\|as any\b" src/ --include="*.ts" | wc -l | xargs echo "Total:"
grep -rn ": any\b\|as any\b" src/ --include="*.ts" | tee reports/any_types.txt

# Detectar callbacks não tratados / promises sem await
grep -rn "\.then\|\.catch" src/ --include="*.ts" | \
    grep -v "await\|return" | tee reports/promises_sem_await.txt

# Verificar console.log em produção (não deve existir em src/)
grep -rn "console\.\(log\|warn\|error\|debug\)" src/ --include="*.ts" \
    | grep -v "test\|spec\|__tests__" | tee reports/console_log.txt

echo "=== Resumo QA ==="
echo "tsc erros:          $(grep -c 'error TS' reports/tsc_strict.txt || echo 0)"
echo "any explícito:      $(grep -rn ': any\b' src/ --include='*.ts' 2>/dev/null | wc -l)"
echo "console.log em src: $(grep -rn 'console\.log' src/ --include='*.ts' 2>/dev/null | grep -v test | wc -l)"
```

**Critério de aprovação:** zero erros `tsc --strict`. Zero `as any` em código de domínio. Zero `console.log` fora de `tests/`.

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
--- a/src/infraestrutura/repositorios/repositorio_api.cpp
+++ b/src/infraestrutura/repositorios/repositorio_api.cpp
@@ -45,7 +45,7 @@
-    void processar(Dados d);
+    void processar(const Dados& d);
```

---

## Checklists de Auditoria Detalhados por Stack

Os procedimentos T1–T15 e A1–A13 cobrem execução automatizada.
Para checklists de auditoria manual com critérios de aceitação por item e roadmaps de remediação, consulte [AUDITORIAS.md](AUDITORIAS.md).

| Stack | Seção em AUDITORIAS.md |
|-------|------------------------|
| **C++ / Qt6 / MySQL** | Parte I — seções I1–I9 (arquitetura, segurança, C++23, Qt6, MySQL, performance, DevOps, qualidade) |
| **Python / Qt6 / SQLite** | Parte II — seções II1–II4 (completo, arquitetura, segurança, Python moderno) |
| **Web — PHP / JS / MySQL** | Parte III — seções III1–III7 (arquitetura 4 camadas, qualidade, performance, DevOps, API REST, acessibilidade/SEO, hardening PHP + LGPD) |

---

*Documento instrucional genérico — adapte caminhos, módulos e ferramentas conforme o projeto alvo.*
*Ferramentas: cppcheck · clang-tidy · afl++ · ASan/UBSan · trivy · grype · gitleaks · nmap · sqlmap · hurl · ffuf · radare2 · lcov · gcovr · pytest · cargo · jest · ruff · mypy · bandit · cargo-audit · pip-audit · npm-audit*
