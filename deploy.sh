#!/bin/bash
# deploy.sh — fluxo único de release + push para o GitHub (MC-11).
#
# Encadeia: build → (commit opcional) → release.sh (bump SemVer +
# CHANGELOG + tag via git-cliff) → git push --follow-tags. O hook
# pre-push (scripts/hooks/pre-push) bloqueia push direto fora deste
# script (bypass via DEPLOY_SH_RUNNING=1).
#
# Uso:
#   ./deploy.sh                       # apenas releaseia commits já criados e pusha
#   ./deploy.sh "feat(x): mensagem"   # commita pendências + releaseia + pusha
#
# MC-12 (GODS_LAWS.md) define o esquema de versão; docs/claude/release.md detalha o ferramental.

set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

cd "$(git rev-parse --show-toplevel)"

# Garante git-cliff acessível (instalado via cargo fica em ~/.cargo/bin)
export PATH="$HOME/.cargo/bin:$PATH"

echo -e "${GREEN}[deploy] 1/4 — build${NC}"
if ! cmake --build build -j"$(nproc)" >/dev/null 2>&1; then
    echo -e "${RED}[deploy] Falha no build. Rode 'cmake --build build' para ver os erros.${NC}"
    exit 1
fi

# Commit opcional de mudanças pendentes
if [ $# -ge 1 ] && [ -n "$1" ]; then
    echo -e "${GREEN}[deploy] 2/4 — commit das mudanças pendentes${NC}"
    git add -A
    if ! git diff --cached --quiet; then
        git commit -m "$1"
    else
        echo -e "${YELLOW}[deploy] Nada para commitar — pulando.${NC}"
    fi
else
    echo -e "${GREEN}[deploy] 2/4 — sem mensagem; preservando commits já existentes${NC}"
fi

echo -e "${GREEN}[deploy] 3/4 — release semântica (git-cliff)${NC}"
./scripts/release.sh

echo -e "${GREEN}[deploy] 4/4 — push para o GitHub (com tag)${NC}"
DEPLOY_SH_RUNNING=1 git push --follow-tags

echo -e "${GREEN}[deploy] ✓ Build + release + push concluídos.${NC}"
