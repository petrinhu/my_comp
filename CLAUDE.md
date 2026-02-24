# CLAUDE.md — my_comp

Guia de contexto para Claude Code e agentes de IA trabalhando neste projeto.
Para documentação mais detalhada, consulte `agents.md`.

## Projeto

Script Bash (`my_comp.sh`) que coleta informações exaustivas do sistema Linux e
gera relatório em Markdown + HTML com log de debug completo. Versão atual: **0.3.6**.

**Não modifica o sistema** — apenas lê e coleta informações.

## Execução

```bash
sudo bash my_comp.sh [/caminho/de/saida]
```

Requer root para `dmidecode` e leitura de `/sys`.

## Dependências obrigatórias

- `bash` 4+
- `python3` — geração de HTML e coleta de usuários

## Convenções críticas do código

- `set -euo pipefail` ativo no nível do script.
- `run_cmd "SECAO" "comando"` — executa via script temporário (`mktemp`) para
  evitar dupla expansão de shell. **Não altere esse mecanismo** sem testar pipes
  compostos.
- Contadores globais usam `(( VAR++ )) || true` — o `|| true` é obrigatório com `set -e`.
- Funções de coleta: `collect_<nome>()` — nunca usam `exit`, apenas `return`.
- Nome de seção em `run_cmd`: padrão `"CATEGORIA/subcategoria"`.

## Arquivos de saída (ignorados pelo .gitignore)

```
MYCOMP_<YYYYMMDD_HHMMSS>.md
MYCOMP_<YYYYMMDD_HHMMSS>.html
MYCOMP_debug_<YYYYMMDD_HHMMSS>.log
```

## Estrutura do repositório

```
my_comp/
├── my_comp.sh        ← script principal
├── CLAUDE.md         ← este arquivo
├── README.md         ← documentação de usuário
├── CHANGELOG.md      ← histórico de versões (Keep a Changelog + SemVer)
├── agents.md         ← guia detalhado para agentes de IA
└── LICENSE           ← MIT
```

## Ao modificar o script

1. Atualize `SCRIPT_VERSION` em `my_comp.sh`.
2. Registre a mudança em `CHANGELOG.md`.
3. Atualize versão em `agents.md` (tabela da seção 1 e rodapé).
4. Atualize versão em `README.md` se necessário.
5. Siga SemVer: PATCH = bug fix, MINOR = nova feature, MAJOR = quebra de compatibilidade.

## Repositório

https://github.com/petrinhu/my_comp
