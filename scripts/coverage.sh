#!/usr/bin/env bash
# scripts/coverage.sh — QA-06
# Mede a cobertura de linhas da suíte (unit + acceptance + qml) por camada.
# Sem meta numérica (MC-07): o número orienta onde faltam testes, não bloqueia.
#
# Uso:
#   scripts/coverage.sh            # build instrumentado em build_cov/, roda ctest, imprime resumo
#   scripts/coverage.sh --html     # também gera build_cov/coverage/index.html
#
# Requer gcovr (dnf install gcovr, ou pip install gcovr).

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COV_DIR="${ROOT_DIR}/build_cov"
HTML=0

for arg in "$@"; do
    case "${arg}" in
        --html) HTML=1 ;;
        -h|--help) sed -n '2,10p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "Opção desconhecida: ${arg}" >&2; exit 2 ;;
    esac
done

if ! command -v gcovr >/dev/null; then
    echo "gcovr não encontrado (dnf install gcovr)" >&2
    exit 1
fi

cmake -S "${ROOT_DIR}" -B "${COV_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage -O0" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" >/dev/null
cmake --build "${COV_DIR}" -j"$(nproc)"

find "${COV_DIR}" -name '*.gcda' -delete
(cd "${COV_DIR}" && ctest --output-on-failure)

GCOVR_ARGS=(-r "${ROOT_DIR}" "${COV_DIR}" --exclude '.*_autogen.*' --exclude 'src/main.cpp')

echo
echo "== Cobertura por camada (linhas) =="
for camada in domain application infrastructure presentation composition; do
    printf '%-15s ' "${camada}"
    gcovr "${GCOVR_ARGS[@]}" --filter "src/${camada}/" -s 2>/dev/null | grep -E '^lines' || echo "sem dados"
done

echo
echo "== Total =="
gcovr "${GCOVR_ARGS[@]}" --filter 'src/' -s 2>/dev/null | grep -E '^(lines|functions|branches)'

if [[ "${HTML}" -eq 1 ]]; then
    mkdir -p "${COV_DIR}/coverage"
    gcovr "${GCOVR_ARGS[@]}" --filter 'src/' --html-details "${COV_DIR}/coverage/index.html"
    echo "Relatório HTML: ${COV_DIR}/coverage/index.html"
fi
