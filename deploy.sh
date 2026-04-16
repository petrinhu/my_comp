#!/bin/bash

# Cores para o terminal
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}[Hook] Iniciando fluxo de deploy...${NC}"

# 1. Build
echo "Construindo o projeto..."
cmake --build build -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}[Erro] Falha no build. Corrija os erros antes de prosseguir.${NC}"
    exit 1
fi

# 2. Git Operations
echo "Preparando commit..."
git add .

# Se uma mensagem for passada por argumento, usa ela; senão, usa uma mensagem padrão
COMMIT_MSG=${1:-"chore: sprint update and technical improvements"}
git commit -m "$COMMIT_MSG"

echo "Realizando push para Codeberg..."
git push
if [ $? -ne 0 ]; then
    echo -e "${RED}[Erro] Falha no push. Verifique sua conexão e repositório.${NC}"
    exit 1
fi

# 3. Mostrar Tabela
echo -e "${GREEN}[Sucesso] Deploy concluído. Estado atual do projeto:${NC}"
echo "--------------------------------------------------------"
cat TABELA.md
echo "--------------------------------------------------------"
