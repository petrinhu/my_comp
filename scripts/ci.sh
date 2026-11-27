#!/usr/bin/env bash
# scripts/ci.sh — BT-03
# Pipeline local de CI consolidada: build + ctest + clang-tidy + cppcheck + wcag + qml-plaintext + valgrind.
#
# Uso:
#   scripts/ci.sh                  # roda todas as etapas
#   scripts/ci.sh --skip-valgrind  # pula valgrind (útil em dev loop rápido)
#   scripts/ci.sh --skip-tidy      # pula clang-tidy (lento em repos grandes)
#   scripts/ci.sh --quick          # só build + ctest
#   scripts/ci.sh --help
#
# Códigos de saída: 0 = tudo passou; 1+ = número de etapas com falha.
# Logs por etapa em build/ci-logs/<etapa>.log. Summary em build/ci-logs/summary.txt.

set -u
set -o pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
LOG_DIR="${BUILD_DIR}/ci-logs"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SKIP_VALGRIND=0
SKIP_TIDY=0
SKIP_CPPCHECK=0
QUICK=0

for arg in "$@"; do
    case "$arg" in
        --skip-valgrind) SKIP_VALGRIND=1 ;;
        --skip-tidy)     SKIP_TIDY=1 ;;
        --skip-cppcheck) SKIP_CPPCHECK=1 ;;
        --quick)         QUICK=1; SKIP_VALGRIND=1; SKIP_TIDY=1; SKIP_CPPCHECK=1 ;;
        --help|-h)
            sed -n '2,15p' "${BASH_SOURCE[0]}"
            exit 0 ;;
        *)
            echo -e "${RED}[ci] argumento desconhecido: $arg${NC}" >&2
            exit 2 ;;
    esac
done

mkdir -p "${LOG_DIR}"
: > "${LOG_DIR}/summary.txt"

FAILED=0
STAGES_RUN=()
STAGES_FAIL=()

record() {
    local name="$1"; local status="$2"; local duration="$3"
    STAGES_RUN+=("$name")
    if [[ "$status" != "0" ]]; then
        STAGES_FAIL+=("$name")
        FAILED=$((FAILED + 1))
        printf "[FAIL] %-12s %6ss  (log: %s)\n" "$name" "$duration" "${LOG_DIR}/${name}.log" >> "${LOG_DIR}/summary.txt"
    else
        printf "[ OK ] %-12s %6ss\n" "$name" "$duration" >> "${LOG_DIR}/summary.txt"
    fi
}

run_stage() {
    local name="$1"; shift
    local log="${LOG_DIR}/${name}.log"
    echo -e "${BLUE}[ci] ▶ ${name}${NC}"
    local start=$(date +%s)
    if "$@" >"$log" 2>&1; then
        local dur=$(( $(date +%s) - start ))
        echo -e "${GREEN}[ci] ✓ ${name} (${dur}s)${NC}"
        record "$name" 0 "$dur"
    else
        local code=$?
        local dur=$(( $(date +%s) - start ))
        echo -e "${RED}[ci] ✗ ${name} falhou (${dur}s, exit=${code}) — ${log}${NC}"
        tail -n 20 "$log" | sed 's/^/    /'
        record "$name" "$code" "$dur"
    fi
}

stage_configure() {
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Ninja \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
}

stage_build() {
    cmake --build "${BUILD_DIR}" -j"$(nproc)"
}

stage_ctest() {
    # --timeout 180 = teto de segurança caso algum teste novo entre sem TIMEOUT no add_test.
    # CTest também respeita as TIMEOUT properties por teste do CMakeLists.txt.
    # --no-tests=error: sem build (configure falhou), "nenhum teste" não pode contar como verde.
    ( cd "${BUILD_DIR}" && ctest --output-on-failure --timeout 180 --no-tests=error )
}

stage_clang_tidy() {
    if ! command -v clang-tidy >/dev/null; then
        echo "clang-tidy não encontrado, pulando." ; return 0
    fi
    if [[ ! -f "${BUILD_DIR}/compile_commands.json" ]]; then
        echo "compile_commands.json ausente em ${BUILD_DIR}. Rode configure antes." ; return 1
    fi
    # Checks alinhados à MC-07 (gate T2): bugprone-*, security-*, cert-*, cppcoreguidelines-*, performance-*.
    # Só as famílias do gate T2 reprovam; performance/cppcoreguidelines são reportadas como
    # aviso. O valor do CLI é ANEXADO ao WarningsAsErrors '*' do .clang-tidy, por isso
    # começa com '-*' (zera antes de reativar o gate). Um arquivo por processo, em paralelo:
    # xargs devolve 123 se qualquer invocação falhar.
    local checks='-*,bugprone-*,cert-*,security-*,clang-analyzer-security*,cppcoreguidelines-*,performance-*,-cppcoreguidelines-avoid-magic-numbers,-cppcoreguidelines-pro-bounds-*,-cppcoreguidelines-pro-type-reinterpret-cast,-cppcoreguidelines-non-private-member-variables-in-classes,-cppcoreguidelines-avoid-non-const-global-variables,-cppcoreguidelines-owning-memory,-cppcoreguidelines-special-member-functions,-cppcoreguidelines-init-variables'
    local gate='-*,bugprone-*,cert-*,security-*,clang-analyzer-security*'
    find "${ROOT_DIR}/src" -name '*.cpp' -not -path '*/build/*' -type f -print0 \
        | xargs -0 -P "$(nproc)" -n 1 clang-tidy -p "${BUILD_DIR}" --quiet \
            --header-filter='src/.*' \
            --warnings-as-errors="${gate}" \
            --checks="${checks}" 2>&1
}

stage_cppcheck() {
    if ! command -v cppcheck >/dev/null; then
        echo "cppcheck não encontrado, pulando." ; return 0
    fi
    # Projeto é C++23 (CLAUDE.md); cppcheck < 2.14 só conhece até c++20.
    local std_flag="--std=c++20"
    if cppcheck --help 2>&1 | grep -q 'c++23'; then
        std_flag="--std=c++23"
    fi
    # Reprova só por error/warning (bugs prováveis); style/performance/portability ficam
    # visíveis no log como sugestão, sem bloquear (MC-07 não os inclui no gate).
    local out
    out=$(cppcheck \
        --enable=warning,style,performance,portability \
        --suppress=missingIncludeSystem \
        --suppress=unknownMacro \
        --suppress=functionStatic \
        --suppress=passedByValue \
        --inline-suppr \
        "${std_flag}" \
        -I "${ROOT_DIR}/src" \
        "${ROOT_DIR}/src" 2>&1)
    echo "$out"
    if echo "$out" | grep -qE ': (error|warning): '; then
        echo "[ci] cppcheck encontrou error/warning" ; return 1
    fi
    return 0
}

stage_wcag() {
    # Gate A3 (MC-07): contraste WCAG AA da paleta central.
    python3 -I "${ROOT_DIR}/scripts/check_contrast.py" "${ROOT_DIR}/src/presentation/qml/Theme.qml"
}

stage_valgrind() {
    if ! command -v valgrind >/dev/null; then
        echo "valgrind não encontrado, pulando." ; return 0
    fi
    if [[ ! -x "${BUILD_DIR}/unit_tests" ]]; then
        echo "unit_tests não construído." ; return 1
    fi
    local supp_arg=""
    if [[ -f "${ROOT_DIR}/valgrind.supp" ]]; then
        supp_arg="--suppressions=${ROOT_DIR}/valgrind.supp"
    fi
    # Critério do gate T4 (MC-07): zero *definitely lost* + zero ERROR SUMMARY.
    # QT_ENABLE_REGEXP_JIT=0: o JIT do PCRE2 lê 16 bytes além do buffer por design (SIMD),
    # falso positivo do memcheck que não tem como ser suprimido (frames JIT sem símbolo).
    local out rc
    out=$(QT_QPA_PLATFORM=offscreen QT_ENABLE_REGEXP_JIT=0 valgrind \
        --leak-check=full \
        --show-leak-kinds=definite \
        --error-exitcode=42 \
        ${supp_arg} \
        "${BUILD_DIR}/unit_tests" --gtest_brief=1 2>&1)
    rc=$?
    echo "$out"
    if echo "$out" | grep -qE "definitely lost: [1-9]"; then
        echo "[ci] DETECTADO: bytes definitely lost" ; return 1
    fi
    if echo "$out" | grep -qE "ERROR SUMMARY: [1-9]"; then
        echo "[ci] DETECTADO: erros de memória no ERROR SUMMARY" ; return 1
    fi
    # exit 42 = erro do memcheck; qualquer outro != 0 = teste falhou sob valgrind.
    if [[ $rc -ne 0 ]]; then
        echo "[ci] valgrind/unit_tests saiu com código ${rc}" ; return 1
    fi
    return 0
}

# Ordem: configure → build → ctest → (paralelizáveis: tidy/cppcheck/valgrind).
# Sequencial para simplificar leitura de logs e evitar contenção I/O.
run_stage configure stage_configure
run_stage build stage_build
run_stage ctest stage_ctest
[[ "$SKIP_TIDY"     == "0" ]] && run_stage clang-tidy stage_clang_tidy
[[ "$SKIP_CPPCHECK" == "0" ]] && run_stage cppcheck stage_cppcheck
run_stage wcag stage_wcag
stage_qml_plaintext() {
    # Gate MC-10: Text/Label com dado dinâmico declaram textFormat (AutoText renderiza marcação).
    python3 -I "${ROOT_DIR}/scripts/check_qml_plaintext.py" "${ROOT_DIR}/src/presentation/qml"
}
run_stage qml-plaintext stage_qml_plaintext
[[ "$SKIP_VALGRIND" == "0" ]] && run_stage valgrind stage_valgrind

echo ""
echo -e "${BLUE}====== CI Summary ======${NC}"
cat "${LOG_DIR}/summary.txt"
echo -e "${BLUE}========================${NC}"

if [[ $FAILED -eq 0 ]]; then
    echo -e "${GREEN}[ci] Todos os estágios passaram (${#STAGES_RUN[@]}).${NC}"
    exit 0
else
    echo -e "${RED}[ci] ${FAILED} estágio(s) falharam: ${STAGES_FAIL[*]}${NC}"
    exit $FAILED
fi
