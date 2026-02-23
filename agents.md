# agents.md — Guia para Agentes de IA

Este documento descreve o projeto `my_comp` para agentes de IA (LLMs, assistentes,
ferramentas de codificação automática), cobrindo contexto, uso, arquitetura,
roadmap e guia de contribuição.

---

## 1. Visão Geral do Projeto

| Campo | Valor |
|---|---|
| **Nome** | my_comp |
| **Repositório** | https://github.com/petrinhu/my_comp |
| **Versão atual** | 0.3.5 |
| **Linguagem** | Bash (POSIX-compatible, bash 4+) |
| **Plataforma alvo** | Linux (desenvolvido e testado em Fedora) |
| **Autor** | Petrus Costa |
| **Licença** | MIT |

**Propósito:** Script Bash que coleta informações exaustivas do sistema Linux
(hardware, OS, rede, serviços, processos, energia, DMI/SMBIOS) e gera relatório
em Markdown + HTML com log de debug completo.

---

## 2. Arquivo Principal

```
my_comp.sh            ← script principal
```

**Execução:**
```bash
sudo bash my_comp.sh [/caminho/de/saida]
```

Requer root para coletas via `dmidecode` e leitura de `/sys`.

---

## 3. Arquitetura Interna

### 3.1 Fluxo de Execução

```
main()
 ├── _log_init()           ← cria LOG_FILE, escreve cabeçalho
 ├── check_root()          ← valida EUID == 0
 ├── check_dependencies()  ← verifica ferramentas essenciais
 ├── [inicializa MD_FILE]
 ├── collect_os()
 ├── collect_user()
 ├── collect_desktop()
 ├── collect_cpu()
 ├── collect_memory()
 ├── collect_gpu()
 ├── collect_battery()
 ├── collect_peripherals()
 ├── collect_storage()
 ├── collect_network()
 ├── collect_software()
 ├── collect_services()
 ├── collect_processes()
 ├── collect_power()
 ├── collect_dmi()
 ├── generate_html()       ← pandoc ou fallback sed
 └── _log_finalize()       ← sumário de estatísticas no LOG_FILE
```

### 3.2 Sistema de Logging

Todas as execuções passam por `run_cmd "SECAO" "comando"`:

- **Nunca propaga falha** — coleta continua sempre (`set -e` suspenso internamente)
- Registra: comando, exit code, tempo em ms, linhas/bytes stdout, stderr completo
- Contadores globais: `LOG_COUNT_OK`, `LOG_COUNT_WARN`, `LOG_COUNT_ERR`, `LOG_COUNT_SKIP`

Ferramentas ausentes usam `run_cmd_skip "SECAO" "tool" "motivo"` em vez de `run_cmd`.

### 3.3 Funções de Markdown

| Função | Uso |
|---|---|
| `section N "título"` | Cabeçalho `##`, `###`, `####` no MD |
| `write "texto"` | Parágrafo simples |
| `code_block "lang" "conteúdo"` | Bloco de código com cerca |
| `table_row "col1" "col2" ...` | Linha de tabela MD |

### 3.4 Saída de Arquivos

```
MYCOMP_<YYYYMMDD_HHMMSS>.md        ← relatório Markdown
MYCOMP_<YYYYMMDD_HHMMSS>.html      ← relatório HTML
MYCOMP_debug_<YYYYMMDD_HHMMSS>.log ← log de debug
```

Todos ignorados pelo `.gitignore`.

---

## 4. Dependências

### Obrigatórias (script falha sem elas)
- `bash` 4+
- `hostname`, `date`, `uname`, `cat`, `grep`, `awk`, `sed`, `wc`, `mktemp`

### Recomendadas (coleta degradada se ausentes)
| Ferramenta | Pacote Fedora | Seção afetada |
|---|---|---|
| `pandoc` | `pandoc` | ~~geração HTML~~ removido — geração via sed nativa |
| `ollama` | `ollama` | ~~modelos de IA~~ removido — fora do escopo do script |
| `dmidecode` | `dmidecode` | DMI/SMBIOS |
| `sensors` | `lm_sensors` | temperatura |
| `lspci` | `pciutils` | GPU, periféricos PCI |
| `lsusb` | `usbutils` | periféricos USB |
| `smartctl` | `smartmontools` | saúde de discos |
| `ss` / `netstat` | `iproute` / `net-tools` | rede |

```bash
sudo dnf install dmidecode lm_sensors pciutils usbutils smartmontools
```

---

## 5. Comportamento Importante para Agentes

- **`set -euo pipefail`** está ativo no nível do script. Internamente, `run_cmd`
  suspende `set -e` com `set +e` para capturar falhas sem abortar.
- Variáveis de contadores usam `(( VAR++ )) || true` para não disparar `set -e`
  quando o resultado é zero.
- `eval "$cmd"` é usado em `run_cmd` — ao modificar, mantenha essa abordagem para
  suportar pipes e redirecionamentos compostos.
- O script **não modifica o sistema** — apenas lê e coleta informações.
- Compatibilidade: testado em Fedora; deve funcionar em qualquer distro com bash 4+.

---

## 6. Roadmap

### v0.4.0 (planejado)
- [ ] Flag `--no-html` para pular geração de HTML
- [ ] Flag `--sections cpu,memory,network` para coleta seletiva
- [ ] Modo `--quiet` (sem output no terminal, apenas arquivos)
- [ ] Detecção de container/VM (Docker, Podman, VirtualBox, KVM)

### v0.5.0 (planejado)
- [ ] Saída JSON estruturada além de Markdown/HTML
- [ ] Comparação de dois relatórios (`dump_tree.sh --diff report1.md report2.md`)
- [ ] Coleta de métricas de performance instantânea (iostat, vmstat)

### v1.0.0 (visão futura)
- [ ] Suite de testes automatizados (bats-core)
- [ ] Pacote RPM/DEB
- [ ] Página de documentação (GitHub Pages)

---

## 7. Guia de Contribuição

### Para agentes de IA e desenvolvedores

**Ao adicionar uma nova seção de coleta:**

1. Crie uma função `collect_<nome>()` seguindo o padrão:
```bash
collect_exemplo() {
    log_step "Coletando exemplo..."
    log_section_start "EXEMPLO"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Título da Seção"

    if cmd_exists ferramenta; then
        code_block "text" "$(run_cmd "EXEMPLO/sub" ferramenta --args)"
    else
        write "$(run_cmd_skip "EXEMPLO/sub" "ferramenta" "motivo da ausência")"
    fi

    log_section_end "EXEMPLO" "$ts_sec"
}
```

2. Adicione a chamada em `main()` na ordem lógica.
3. Documente a dependência na seção 4 deste arquivo e no `README.md`.

**Convenções de nomenclatura:**
- Nome de seção em `run_cmd`: `"CATEGORIA/subcategoria"` (ex: `"NET/interfaces"`)
- Funções de coleta: `collect_<nome_em_minusculo>()`
- Variáveis locais: `snake_case`

**O que NÃO fazer:**
- Não usar `exit` dentro de funções `collect_*` — use `return`
- Não modificar arquivos do sistema — script é somente leitura
- Não remover o `|| true` dos contadores — quebra com `set -e`
- Não substituir `eval` em `run_cmd` sem testar pipes compostos

### Versionamento

Seguimos [Semantic Versioning](https://semver.org/lang/pt-BR/):
- `PATCH` (0.3.**x**): correções de bugs, ajustes de coleta
- `MINOR` (0.**x**.0): novas seções, novas flags, novos formatos de saída
- `MAJOR` (**x**.0.0): quebra de compatibilidade (ex: mudança de formato de saída)

Atualize sempre `SCRIPT_VERSION` no script e registre em `CHANGELOG.md`.

---

## 8. Estrutura do Repositório

```
my_comp/
├── my_comp.sh        ← script principal
├── README.md         ← documentação de usuário
├── CHANGELOG.md      ← histórico de versões
├── agents.md         ← este arquivo
├── LICENSE           ← MIT (pt-BR + en)
└── .gitignore        ← ignora saídas geradas
```

---

*Última atualização: 2026-02-23 — v0.3.5*
