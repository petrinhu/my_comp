#!/bin/bash
# scripts/release.sh — release automatizada via git-cliff.
#
# Lê commits Conventional desde o último tag, calcula a próxima versão
# SemVer, atualiza VERSION + CHANGELOG.md, cria um commit chore(release)
# e uma tag anotada. Idempotente: se não há commits novos desde o último
# tag, sai com aviso sem alterar nada.
#
# Uso (normalmente invocado via deploy.sh, não diretamente):
#   ./scripts/release.sh           # calcula bump e aplica
#   ./scripts/release.sh --dry-run # mostra o que faria sem alterar nada

set -euo pipefail

GREEN='\033[0;32m'; YELLOW='\033[1;33m'; RED='\033[0;31m'; NC='\033[0m'
DRY_RUN=0
[ "${1:-}" = "--dry-run" ] && DRY_RUN=1

cd "$(git rev-parse --show-toplevel)"

if ! command -v git-cliff >/dev/null 2>&1; then
    echo -e "${RED}[release] git-cliff não instalado. Instale com 'cargo install git-cliff' ou 'dnf install git-cliff'.${NC}"
    exit 1
fi

# Garante baseline: se não existe nenhum tag semver, cria v0.1.0-alpha.0
# no commit do último push registrado (origin/main, MC-11). Por que 0.1 e não
# 2.0: nenhuma release estável foi cortada ainda; "v2.0" no README/nome
# é codename (segunda geração pós-bash legacy) — SemVer real começa em
# 0.x até o BT-04 (Release Beta) cortar v1.0.0-beta.0.
if [ -z "$(git tag --list 'v*')" ]; then
    BASELINE_COMMIT="$(git rev-parse origin/main 2>/dev/null || git rev-list --max-parents=0 HEAD | head -1)"
    echo -e "${YELLOW}[release] Nenhuma tag semver encontrada. Criando baseline v0.1.0-alpha.0 em ${BASELINE_COMMIT:0:7}.${NC}"
    [ "$DRY_RUN" -eq 0 ] && git tag -a v0.1.0-alpha.0 "$BASELINE_COMMIT" -m "Baseline inicial do esquema SemVer + git-cliff."
fi

# Calcula próxima versão via git-cliff (semver bump baseado em commits Conventional)
NEXT_VERSION_RAW="$(git cliff --bumped-version 2>/dev/null || echo '')"
NEXT_VERSION="${NEXT_VERSION_RAW#v}"

if [ -z "$NEXT_VERSION" ] || [ "$NEXT_VERSION" = "$(cat VERSION 2>/dev/null || echo '')" ]; then
    echo -e "${YELLOW}[release] Sem commits releasáveis desde o último tag. Nada a fazer.${NC}"
    exit 0
fi

CURRENT_VERSION="$(cat VERSION 2>/dev/null || echo 'unknown')"
echo -e "${GREEN}[release] $CURRENT_VERSION → $NEXT_VERSION${NC}"

if [ "$DRY_RUN" -eq 1 ]; then
    echo -e "${YELLOW}[dry-run] Mudanças que SERIAM aplicadas:${NC}"
    echo "  - VERSION: $CURRENT_VERSION → $NEXT_VERSION"
    echo "  - CHANGELOG.md: nova entrada para v$NEXT_VERSION (prepend)"
    echo "  - commit: chore(release): v$NEXT_VERSION"
    echo "  - tag: v$NEXT_VERSION (anotada)"
    echo ""
    echo "--- preview do CHANGELOG ---"
    git cliff --tag "v$NEXT_VERSION" --unreleased
    exit 0
fi

# Aplica
echo "$NEXT_VERSION" > VERSION
git cliff --tag "v$NEXT_VERSION" --unreleased --prepend CHANGELOG.md
git add VERSION CHANGELOG.md
git commit -m "chore(release): v$NEXT_VERSION

Bump automático via git-cliff a partir dos commits Conventional desde
o tag anterior. CHANGELOG.md atualizado com a seção de v$NEXT_VERSION.

Co-Authored-By: Claude Opus 4.7 (1M context) <noreply@anthropic.com>"
git tag -a "v$NEXT_VERSION" -m "Release v$NEXT_VERSION

Veja CHANGELOG.md para detalhes da seção [$NEXT_VERSION]."

echo -e "${GREEN}[release] commit + tag v$NEXT_VERSION criados localmente. Push: 'git push --follow-tags' (ou via deploy.sh).${NC}"
