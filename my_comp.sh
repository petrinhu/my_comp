#!/usr/bin/env bash
# =============================================================================
# MYCOMP - Gerador de Relatório de Configuração do Sistema
# Versão: 0.3.1
# Descrição: Coleta informações exaustivas do sistema Linux e gera
#             relatório em Markdown e HTML, com log completo de debug.
# Uso: sudo bash my_comp.sh [/caminho/de/saida]
# =============================================================================

set -euo pipefail

# =============================================================================
# CONFIGURAÇÕES GLOBAIS
# =============================================================================
SCRIPT_VERSION="0.3.1"
HOSTNAME_VAL=$(hostname)
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
DATESTAMP=$(date '+%Y%m%d_%H%M%S')
OUTPUT_DIR="${1:-$(pwd)}"
MD_FILE="${OUTPUT_DIR}/MYCOMP_${DATESTAMP}.md"
HTML_FILE="${OUTPUT_DIR}/MYCOMP_${DATESTAMP}.html"
LOG_FILE="${OUTPUT_DIR}/MYCOMP_debug_${DATESTAMP}.log"

# Contadores globais de estatísticas
declare -i LOG_COUNT_OK=0
declare -i LOG_COUNT_WARN=0
declare -i LOG_COUNT_ERR=0
declare -i LOG_COUNT_SKIP=0
declare -i LOG_CMD_TOTAL=0

# Timeout máximo por comando (segundos) — evita travamentos
CMD_TIMEOUT=30
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

# =============================================================================
# SISTEMA DE LOGGING COMPLETO
# =============================================================================

_log_init() {
    mkdir -p "$OUTPUT_DIR"
    cat > "$LOG_FILE" << LOGINIT
================================================================================
  MYCOMP DEBUG LOG — Versão ${SCRIPT_VERSION}
  Host     : ${HOSTNAME_VAL}
  Início   : ${TIMESTAMP}
  Usuário  : ${USER} (EUID: ${EUID})
  PID      : $$
  Bash     : ${BASH_VERSION}
  OutputDir: ${OUTPUT_DIR}
================================================================================

LOGINIT
}

_log_write() {
    local level="$1" section="$2" message="$3"
    local ts
    ts=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    printf '[%s] [%-7s] [%-22s] %s\n' "$ts" "$level" "$section" "$message" >> "$LOG_FILE"
}

log_info()  {
    echo -e "${GREEN}[INFO]${NC}  $*"
    _log_write "INFO   " "SCRIPT" "$*"
    (( LOG_COUNT_OK++ ))   || true
}
log_warn()  {
    echo -e "${YELLOW}[AVISO]${NC} $*"
    _log_write "WARNING" "SCRIPT" "$*"
    (( LOG_COUNT_WARN++ )) || true
}
log_err()   {
    echo -e "${RED}[ERRO]${NC}  $*" >&2
    _log_write "ERROR  " "SCRIPT" "$*"
    (( LOG_COUNT_ERR++ ))  || true
}
log_step()  {
    echo -e "${CYAN}[>>>]${NC}   $*"
    _log_write "STEP   " "SCRIPT" "=== $* ==="
}
log_debug() { _log_write "DEBUG  " "SCRIPT" "$*"; }

cmd_exists() {
    local cmd="$1"
    if command -v "$cmd" &>/dev/null; then
        _log_write "DEBUG  " "CMD_CHECK" "EXISTS: ${cmd} => $(command -v "$cmd")"
        return 0
    else
        _log_write "DEBUG  " "CMD_CHECK" "MISSING: ${cmd}"
        return 1
    fi
}

# Executor principal: run_cmd "SECAO" "comando args..."
# - Loga: comando, exit code, tempo ms, linhas/bytes stdout, stderr completo
# - Nunca propaga falha — coleta continua sempre
run_cmd() {
    local section_name="$1"
    shift
    local cmd="$*"

    local stdout_tmp stderr_tmp
    stdout_tmp=$(mktemp /tmp/mycomp_out_XXXXXX)
    stderr_tmp=$(mktemp /tmp/mycomp_err_XXXXXX)

    local ts_start ts_end elapsed exit_code
    ts_start=$(date '+%s%3N')
    (( LOG_CMD_TOTAL++ )) || true

    set +e
    timeout "$CMD_TIMEOUT" bash -c "$cmd" > "$stdout_tmp" 2> "$stderr_tmp"
    exit_code=$?
    set -e

    # exit code 124 = timeout expirado
    if [[ $exit_code -eq 124 ]]; then
        (( LOG_COUNT_WARN++ )) || true
        _log_write "WARNING" "$section_name" "TIMEOUT: comando excedeu ${CMD_TIMEOUT}s — CMD: ${cmd}"
        echo "[TIMEOUT: ${CMD_TIMEOUT}s excedidos | cmd: ${cmd}]"
        rm -f "$stdout_tmp" "$stderr_tmp"
        return 0
    fi

    ts_end=$(date '+%s%3N')
    elapsed=$(( ts_end - ts_start ))

    local stdout_content stderr_content stdout_lines stderr_lines stdout_bytes
    stdout_content=$(cat "$stdout_tmp")
    stderr_content=$(cat "$stderr_tmp")
    stdout_lines=$(wc -l < "$stdout_tmp")
    stderr_lines=$(wc -l < "$stderr_tmp")
    stdout_bytes=$(wc -c < "$stdout_tmp")
    rm -f "$stdout_tmp" "$stderr_tmp"

    local ts_now
    ts_now=$(date '+%Y-%m-%d %H:%M:%S.%3N')

    if [[ $exit_code -eq 0 ]]; then
        (( LOG_COUNT_OK++ )) || true
        {
            printf '[%s] [OK     ] [%-22s] CMD: %s\n' "$ts_now" "$section_name" "$cmd"
            printf '[%s] [OK     ] [%-22s] EXIT:%d | TIME:%dms | STDOUT:%d linhas/%d bytes | STDERR:%d linhas\n' \
                "$ts_now" "$section_name" "$exit_code" "$elapsed" \
                "$stdout_lines" "$stdout_bytes" "$stderr_lines"
            if [[ -n "$stderr_content" ]]; then
                printf '[%s] [STDERR ] [%-22s] (presente mesmo com exit 0):\n' "$ts_now" "$section_name"
                while IFS= read -r line; do
                    printf '[%s] [STDERR ] [%-22s]   %s\n' "$ts_now" "$section_name" "$line"
                done <<< "$stderr_content"
            fi
        } >> "$LOG_FILE"
        echo "$stdout_content"

    else
        (( LOG_COUNT_ERR++ )) || true
        {
            printf '[%s] [FAILED ] [%-22s] CMD: %s\n' "$ts_now" "$section_name" "$cmd"
            printf '[%s] [FAILED ] [%-22s] EXIT:%d | TIME:%dms | STDOUT:%d linhas | STDERR:%d linhas\n' \
                "$ts_now" "$section_name" "$exit_code" "$elapsed" \
                "$stdout_lines" "$stderr_lines"
            if [[ -n "$stdout_content" ]]; then
                printf '[%s] [STDOUT ] [%-22s] stdout parcial:\n' "$ts_now" "$section_name"
                while IFS= read -r line; do
                    printf '[%s] [STDOUT ] [%-22s]   %s\n' "$ts_now" "$section_name" "$line"
                done <<< "$stdout_content"
            fi
            if [[ -n "$stderr_content" ]]; then
                printf '[%s] [STDERR ] [%-22s] stderr completo:\n' "$ts_now" "$section_name"
                while IFS= read -r line; do
                    printf '[%s] [STDERR ] [%-22s]   %s\n' "$ts_now" "$section_name" "$line"
                done <<< "$stderr_content"
            fi
        } >> "$LOG_FILE"
        echo "[ERRO | exit:${exit_code} | cmd: ${cmd}]"
        [[ -n "$stderr_content" ]] && echo "[STDERR: $(echo "$stderr_content" | head -3)]"
    fi

    return 0
}

# Loga ferramenta ausente como SKIP (sem tentar executar)
run_cmd_skip() {
    local section_name="$1" tool="$2" reason="$3"
    (( LOG_COUNT_SKIP++ )) || true
    _log_write "SKIP   " "$section_name" "TOOL: ${tool} | MOTIVO: ${reason}"
    echo "[não disponível: ${tool} — ${reason}]"
}

log_section_start() {
    local name="$1"
    printf '\n################################################################################\n' >> "$LOG_FILE"
    printf '# SEÇÃO : %-70s #\n' "$name"                                                        >> "$LOG_FILE"
    printf '# Início: %-70s #\n' "$(date '+%Y-%m-%d %H:%M:%S')"                                >> "$LOG_FILE"
    printf '################################################################################\n'  >> "$LOG_FILE"
}

log_section_end() {
    local name="$1" ts_start="$2"
    local elapsed=$(( $(date '+%s%3N') - ts_start ))
    printf '# FIM   : %-70s #\n' "$name"                  >> "$LOG_FILE"
    printf '# Tempo : %dms%-67s #\n' "$elapsed" ""         >> "$LOG_FILE"
    printf '################################################################################\n\n' >> "$LOG_FILE"
}

_log_finalize() {
    {
        echo ""
        echo "================================================================================"
        echo "  SUMÁRIO FINAL DE EXECUÇÃO"
        echo "================================================================================"
        printf '  Fim             : %s\n' "$(date '+%Y-%m-%d %H:%M:%S')"
        printf '  Comandos totais : %d\n' "$LOG_CMD_TOTAL"
        printf '  OK (sucesso)    : %d\n' "$LOG_COUNT_OK"
        printf '  WARNING (aviso) : %d\n' "$LOG_COUNT_WARN"
        printf '  ERROR (falha)   : %d\n' "$LOG_COUNT_ERR"
        printf '  SKIP (ausente)  : %d\n' "$LOG_COUNT_SKIP"
        echo "--------------------------------------------------------------------------------"
        printf '  MD   : %s\n' "$MD_FILE"
        printf '  HTML : %s\n' "$HTML_FILE"
        printf '  LOG  : %s\n' "$LOG_FILE"
        echo "================================================================================"
    } >> "$LOG_FILE"
}

# =============================================================================
# FUNÇÕES MD
# =============================================================================

section() {
    local level="$1" title="$2"
    local hashes
    hashes=$(printf '%0.s#' $(seq 1 "$level"))
    printf '\n%s %s\n\n' "$hashes" "$title" >> "$MD_FILE"
}

code_block() {
    local lang="${1:-}" content="$2"
    printf '```%s\n%s\n```\n\n' "$lang" "$content" >> "$MD_FILE"
}

write() { echo "$*" >> "$MD_FILE"; }

# =============================================================================
# VERIFICAÇÃO DE ROOT E DEPENDÊNCIAS
# =============================================================================

check_root() {
    if [[ $EUID -ne 0 ]]; then
        log_warn "Script não está rodando como root. Seções como dmidecode, smartctl e nvme serão limitadas."
        _log_write "WARNING" "ROOT_CHECK" "EUID=${EUID} — não é root"
        read -rp "Continuar mesmo sem root? [s/N] " resp
        [[ "$resp" =~ ^[sS]$ ]] || { _log_write "ERROR  " "ROOT_CHECK" "Usuário abortou por falta de root"; exit 1; }
    else
        _log_write "OK     " "ROOT_CHECK" "Executando como root (EUID=0)"
    fi
}

DEPS_REQUIRED=(
    "lscpu:util-linux"      "lsblk:util-linux"      "lsusb:usbutils"
    "lspci:pciutils"        "ip:iproute"             "ss:iproute"
    "nmcli:NetworkManager"  "hostnamectl:systemd"    "timedatectl:systemd"
    "localectl:systemd"     "systemctl:systemd"      "journalctl:systemd"
    "findmnt:util-linux"    "blkid:util-linux"
)

DEPS_OPTIONAL=(
    "upower:upower"             "sensors:lm_sensors"        "dmidecode:dmidecode"
    "smartctl:smartmontools"    "nvme:nvme-cli"              "nvidia-smi:akmod-nvidia"
    "compsize:btrfs-compsize"   "btrfs:btrfs-progs"         "tune2fs:e2fsprogs"
    "xfs_info:xfsprogs"        "dump.f2fs:f2fs-tools"       "cryptsetup:cryptsetup"
    "pvdisplay:lvm2"            "mdadm:mdadm"                "v4l2-ctl:v4l-utils"
    "bluetoothctl:bluez"        "flatpak:flatpak"            "podman:podman"
    "docker:docker"             "gcc:gcc"
    "python3:python3"           "node:nodejs"                "iostat:sysstat"
    "hdparm:hdparm"             "firewall-cmd:firewalld"     "getenforce:libselinux-utils"
    "usb-devices:usbutils"
    "powerprofilesctl:power-profiles-daemon"
)

check_dependencies() {
    log_step "Verificando dependências..."
    log_section_start "DEPENDÊNCIAS"
    local ts_sec
    ts_sec=$(date '+%s%3N')

    local missing_required=() missing_optional=()

    for dep_pkg in "${DEPS_REQUIRED[@]}"; do
        local dep="${dep_pkg%%:*}" pkg="${dep_pkg##*:}"
        cmd_exists "$dep" || missing_required+=("$dep:$pkg")
    done
    for dep_pkg in "${DEPS_OPTIONAL[@]}"; do
        local dep="${dep_pkg%%:*}" pkg="${dep_pkg##*:}"
        cmd_exists "$dep" || missing_optional+=("$dep:$pkg")
    done

    if [[ ${#missing_required[@]} -gt 0 ]]; then
        log_err "Dependências OBRIGATÓRIAS ausentes:"
        local pkgs_to_install=()
        for dp in "${missing_required[@]}"; do
            local d="${dp%%:*}" p="${dp##*:}"
            echo "  - ${d} (pacote: ${p})"
            _log_write "ERROR  " "DEPS" "OBRIGATÓRIA AUSENTE: ${d} / pacote: ${p}"
            pkgs_to_install+=("$p")
        done
        echo ""
        read -rp "Instalar obrigatórias agora? [s/N] " resp
        if [[ "$resp" =~ ^[sS]$ ]]; then
            _log_write "INFO   " "DEPS" "Instalando: ${pkgs_to_install[*]}"
            dnf install -y "${pkgs_to_install[@]}" || {
                log_err "Falha no dnf install. Abortando."
                _log_write "ERROR  " "DEPS" "FATAL: dnf install falhou"
                exit 1
            }
            _log_write "OK     " "DEPS" "Instalação de obrigatórias concluída"
        else
            log_err "Abortando por dependências obrigatórias ausentes."
            _log_write "ERROR  " "DEPS" "FATAL: usuário recusou instalação"
            exit 1
        fi
    else
        _log_write "OK     " "DEPS" "Todas as dependências obrigatórias presentes"
    fi

    if [[ ${#missing_optional[@]} -gt 0 ]]; then
        log_warn "Dependências OPCIONAIS ausentes:"
        local pkgs_to_install=()
        for dp in "${missing_optional[@]}"; do
            local d="${dp%%:*}" p="${dp##*:}"
            echo "  - ${d} (pacote: ${p})"
            _log_write "WARNING" "DEPS" "OPCIONAL AUSENTE: ${d} / pacote: ${p}"
            pkgs_to_install+=("$p")
        done
        echo ""
        read -rp "Instalar opcionais agora? [s/N] " resp
        if [[ "$resp" =~ ^[sS]$ ]]; then
            _log_write "INFO   " "DEPS" "Instalando opcionais: ${pkgs_to_install[*]}"
            dnf install -y "${pkgs_to_install[@]}" 2>/dev/null \
                || log_warn "Algumas opcionais não puderam ser instaladas."
        else
            log_warn "Continuando sem opcionais."
            _log_write "WARNING" "DEPS" "Usuário optou por continuar sem opcionais"
        fi
    else
        _log_write "OK     " "DEPS" "Todas as dependências opcionais presentes"
    fi

    log_info "Verificação de dependências concluída."
    log_section_end "DEPENDÊNCIAS" "$ts_sec"
}

# =============================================================================
# SEÇÕES DE COLETA
# =============================================================================

collect_os() {
    log_step "Coletando OS e Kernel..."
    log_section_start "OS/KERNEL"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Versão do Kernel e Arquitetura"
    code_block "text" "$(run_cmd "OS/uname" uname -a)"

    section 2 "Distribuição Linux — Metadados Completos"
    code_block "text" "$(run_cmd "OS/os-release" cat /etc/os-release)"

    section 2 "Informações do Host"
    code_block "text" "$(run_cmd "OS/hostnamectl" hostnamectl)"

    section 2 "Data, Hora e Fuso Horário"
    code_block "text" "$(run_cmd "OS/timedatectl" timedatectl)"

    section 2 "Locale e Teclado"
    code_block "text" "$(run_cmd "OS/localectl" localectl)"

    section 2 "Variáveis de Ambiente Relevantes"
    code_block "text" "$(run_cmd "OS/env" bash -c 'env | grep -E "LANG|PATH|DISPLAY|WAYLAND|XDG|SHELL|HOME|USER|DESKTOP" | sort')"

    section 2 "Limites do Sistema (ulimit)"
    code_block "text" "$(run_cmd "OS/ulimit" bash -c 'ulimit -a')"

    section 2 "SELinux / AppArmor"
    if cmd_exists getenforce; then
        code_block "text" "SELinux: $(run_cmd "OS/selinux" getenforce)
$(run_cmd "OS/sestatus" sestatus)"
    else
        write "$(run_cmd_skip "OS/selinux" "getenforce" "libselinux-utils não instalado")"
    fi

    log_section_end "OS/KERNEL" "$ts_sec"
}

collect_user() {
    log_step "Coletando informações de usuário..."
    log_section_start "USUÁRIO"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Usuário Atual"
    code_block "text" "$(run_cmd "USER/id" id)
Grupos: $(run_cmd "USER/groups" groups)
Shell: $SHELL | Home: $HOME"

    section 2 "Usuários do Sistema (não-sistema, UID >= 1000)"
    code_block "text" "$(run_cmd "USER/passwd" bash -c 'awk -F: "$3 >= 1000 && $3 < 65534 {print $1, \"UID:\"$3, \"Shell:\"$7, \"Home:\"$6}" /etc/passwd')"

    section 2 "Últimos Logins"
    code_block "text" "$(run_cmd "USER/last" last -n 20)"

    log_section_end "USUÁRIO" "$ts_sec"
}

collect_desktop() {
    log_step "Coletando ambiente de desktop..."
    log_section_start "DESKTOP"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Ambiente de Desktop"
    write "**Variáveis de sessão:**"
    code_block "text" "DESKTOP_SESSION    : ${DESKTOP_SESSION:-não definido}
XDG_SESSION_TYPE   : ${XDG_SESSION_TYPE:-não definido}
XDG_CURRENT_DESKTOP: ${XDG_CURRENT_DESKTOP:-não definido}
WAYLAND_DISPLAY    : ${WAYLAND_DISPLAY:-não definido}
DISPLAY            : ${DISPLAY:-não definido}"

    section 3 "Resolução e Monitores"
    # Detecta tipo de sessão do usuário real (não root)
    local real_user="${SUDO_USER:-$USER}"
    local real_uid
    real_uid=$(id -u "$real_user" 2>/dev/null || echo "")
    local xdg_type=""
    [[ -n "$real_uid" ]] && xdg_type=$(loginctl show-user "$real_user" 2>/dev/null | grep -i "Display\|Session" | head -3 || true)

    code_block "text" "Sessão do usuário $real_user:
$(run_cmd "DESKTOP/loginctl" bash -c "loginctl show-session \$(loginctl list-sessions --no-legend | awk '\$3==\"${real_user}\" {print \$1}' | head -1) 2>/dev/null | grep -E 'Type|State|Display' || echo '[sessão não detectada via loginctl]'")

=== kscreen-doctor (KDE/Wayland) ===
$(run_cmd "DESKTOP/kscreen" bash -c 'command -v kscreen-doctor && timeout 10 kscreen-doctor -o 2>/dev/null || echo "[kscreen-doctor indisponível]"')

=== wlr-randr (wlroots/Wayland) ===
$(run_cmd "DESKTOP/wlr-randr" bash -c 'command -v wlr-randr && timeout 10 wlr-randr 2>/dev/null || echo "[wlr-randr indisponível]"')

=== xrandr (X11 apenas) ===
$(run_cmd "DESKTOP/xrandr" bash -c 'if [[ -n "$DISPLAY" ]]; then timeout 10 xrandr --query 2>/dev/null || echo "[xrandr falhou]"; else echo "[xrandr ignorado — sem sessão X11/DISPLAY]"; fi')

=== Conectores DRM via sysfs (universal) ===
$(run_cmd "DESKTOP/drm-connectors" bash -c '
    for conn in /sys/class/drm/*/status; do
        name=$(echo "$conn" | sed "s|/sys/class/drm/||;s|/status||")
        status=$(cat "$conn" 2>/dev/null)
        modes=$(cat "$(dirname $conn)/modes" 2>/dev/null | head -3 | tr "\n" " ")
        printf "%-30s status: %-12s modos: %s\n" "$name" "$status" "$modes"
    done
')"

    log_section_end "DESKTOP" "$ts_sec"
}

collect_cpu() {
    log_step "Coletando CPU..."
    log_section_start "CPU"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Processador — Informações Completas"

    section 3 "lscpu — Visão Geral"
    code_block "text" "$(run_cmd "CPU/lscpu" lscpu)"

    section 3 "Topologia Extendida"
    code_block "text" "$(run_cmd "CPU/lscpu-ext" lscpu --extended)"

    section 3 "/proc/cpuinfo — Campos Relevantes"
    code_block "text" "$(run_cmd "CPU/cpuinfo" bash -c "grep -E 'processor|model name|cpu MHz|cache size|physical id|core id|flags' /proc/cpuinfo | head -80")"

    section 3 "Flags da CPU (extensões e virtualização)"
    code_block "text" "$(run_cmd "CPU/flags" bash -c 'grep -m1 "flags" /proc/cpuinfo | tr " " "\n" | sort | grep -E "vmx|svm|avx|aes|ht|lm|nx|pae|sse"')"

    section 3 "Governador de Frequência"
    code_block "text" "$(run_cmd "CPU/governor" bash -c '
        f=/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
        if [[ -r "$f" ]]; then
            echo "$(cat $f) (cpu0 — representativo)"
        else
            echo "[cpufreq não disponível neste kernel/hardware]"
        fi
    ')"

    section 3 "Temperatura da CPU"
    if cmd_exists sensors; then
        code_block "text" "$(run_cmd "CPU/sensors" sensors)"
    else
        write "$(run_cmd_skip "CPU/sensors" "sensors" "lm_sensors não instalado — dnf install lm_sensors")"
    fi

    section 3 "Carga do Sistema"
    code_block "text" "Uptime     : $(run_cmd "CPU/uptime" uptime)
LoadAverage: $(run_cmd "CPU/loadavg" cat /proc/loadavg)"

    log_section_end "CPU" "$ts_sec"
}

collect_memory() {
    log_step "Coletando memória RAM..."
    log_section_start "MEMÓRIA"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Memória RAM"

    section 3 "Uso Atual (free)"
    code_block "text" "$(run_cmd "MEM/free" free -h)"

    section 3 "/proc/meminfo Completo"
    code_block "text" "$(run_cmd "MEM/meminfo" cat /proc/meminfo)"

    section 3 "Slots e Módulos Físicos (dmidecode)"
    if cmd_exists dmidecode; then
        code_block "text" "$(run_cmd "MEM/dmidecode" dmidecode -t memory)"
    else
        write "$(run_cmd_skip "MEM/dmidecode" "dmidecode" "não instalado — dnf install dmidecode")"
    fi

    section 3 "Swap"
    code_block "text" "$(run_cmd "MEM/swapon" swapon --show)
$(run_cmd "MEM/swaps" cat /proc/swaps)"

    log_section_end "MEMÓRIA" "$ts_sec"
}

collect_gpu() {
    log_step "Coletando GPU..."
    log_section_start "GPU"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Processadores Gráficos"

    section 3 "Dispositivos PCI — GPUs"
    code_block "text" "$(run_cmd "GPU/lspci-basic" lspci | grep -i -E 'vga|3d|display|gpu')"

    section 3 "Detalhes PCI das GPUs"
    code_block "text" "$(run_cmd "GPU/lspci-v" bash -c "lspci -v | awk '/VGA|3D|Display/,/^\$/'")"

    section 3 "NVIDIA — nvidia-smi"
    if cmd_exists nvidia-smi; then
        code_block "text" "$(run_cmd "GPU/nvidia-smi" nvidia-smi)"
        section 4 "nvidia-smi — Query Completo"
        code_block "text" "$(run_cmd "GPU/nvidia-smi-q" nvidia-smi -q)"
    else
        write "$(run_cmd_skip "GPU/nvidia-smi" "nvidia-smi" "driver NVIDIA não instalado ou GPU não presente")"
    fi

    section 3 "Intel / Mesa — OpenGL"
    code_block "text" "$(run_cmd "GPU/glxinfo" bash -c 'command -v glxinfo && glxinfo | grep -E "OpenGL|renderer|vendor|version" || echo "[glxinfo indisponível]"')"

    section 3 "DRM — Dispositivos de Display (sysfs)"
    code_block "text" "$(run_cmd "GPU/drm-sysfs" ls -la /sys/class/drm/)"

    log_section_end "GPU" "$ts_sec"
}

collect_battery() {
    log_step "Coletando bateria..."
    log_section_start "BATERIA"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Bateria"

    if cmd_exists upower; then
        local bat
        bat=$(run_cmd "BAT/upower-enum" upower -e | grep -i bat || true)
        if [[ -n "$bat" ]]; then
            while IFS= read -r battery; do
                section 3 "Bateria: $battery"
                code_block "text" "$(run_cmd "BAT/upower-info" upower -i "$battery")"
            done <<< "$bat"
        else
            write "_Nenhuma bateria detectada via upower._"
            _log_write "WARNING" "BATERIA" "upower não encontrou dispositivos de bateria"
        fi
        section 3 "upower — Dump Completo"
        code_block "text" "$(run_cmd "BAT/upower-dump" upower -d | head -120)"
    else
        write "$(run_cmd_skip "BAT/upower" "upower" "não instalado — dnf install upower")"
    fi

    section 3 "Bateria via sysfs (/sys/class/power_supply)"
    code_block "text" "$(run_cmd "BAT/sysfs" bash -c '
        for f in /sys/class/power_supply/*/; do
            echo "=== $f ==="
            for attr in status capacity energy_now energy_full energy_full_design \
                        manufacturer model_name technology cycle_count voltage_now \
                        current_now charge_now charge_full charge_full_design; do
                val=$(cat "${f}${attr}" 2>/dev/null)
                [[ -n "$val" ]] && printf "  %-30s: %s\n" "$attr" "$val"
            done
            echo ""
        done
    ')"

    log_section_end "BATERIA" "$ts_sec"
}

collect_peripherals() {
    log_step "Coletando periféricos..."
    log_section_start "PERIFÉRICOS"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Dispositivos PCI — Completo (lspci -vvv)"
    code_block "text" "$(run_cmd "PCI/lspci-vvv" lspci -vvv)"

    section 2 "Dispositivos USB"

    section 3 "Lista Básica (lsusb)"
    code_block "text" "$(run_cmd "USB/lsusb" lsusb)"

    section 3 "Topologia em Árvore (lsusb -t)"
    code_block "text" "$(run_cmd "USB/lsusb-t" lsusb -t)"

    section 3 "Detalhes Completos (lsusb -v)"
    code_block "text" "$(run_cmd "USB/lsusb-v" lsusb -v)"

    section 3 "Dispositivos USB via usb-devices"
    if cmd_exists usb-devices; then
        code_block "text" "$(run_cmd "USB/usb-devices" usb-devices)"
    else
        write "$(run_cmd_skip "USB/usb-devices" "usb-devices" "não disponível")"
    fi

    section 3 "USB via sysfs — Atributos e Drivers"
    code_block "text" "$(run_cmd "USB/sysfs" bash -c '
        for d in /sys/bus/usb/devices/*/; do
            prod=$(cat "${d}product"       2>/dev/null || true)
            mfr=$( cat "${d}manufacturer"  2>/dev/null || true)
            ver=$( cat "${d}version"       2>/dev/null || true)
            speed=$(cat "${d}speed"        2>/dev/null || true)
            drv=$( readlink "${d}driver"   2>/dev/null | xargs basename 2>/dev/null || true)
            [[ -n "$prod" ]] && printf "%-40s | Produto: %-30s | Fab: %-20s | USB: %s | Speed: %s | Driver: %s\n" \
                "$d" "$prod" "$mfr" "$ver" "$speed" "$drv"
        done
    ')"

    section 3 "Eventos USB no Boot (dmesg)"
    code_block "text" "$(run_cmd "USB/dmesg" dmesg | grep -i usb | head -100)"

    section 2 "Áudio"
    code_block "text" "=== aplay -l ===
$(run_cmd "AUDIO/aplay" aplay -l)
=== arecord -l ===
$(run_cmd "AUDIO/arecord" arecord -l)
=== pactl info ===
$(run_cmd "AUDIO/pactl-info" bash -c 'command -v pactl && pactl info || echo "[pactl indisponível]"')
=== pactl list sinks ===
$(run_cmd "AUDIO/pactl-sinks" bash -c 'command -v pactl && pactl list sinks | head -80 || echo "[indisponível]"')
=== PipeWire ===
$(run_cmd "AUDIO/pw-cli" bash -c 'command -v pw-cli && pw-cli info || echo "[pw-cli indisponível]"')"

    section 2 "Webcam / Câmera (v4l2)"
    if cmd_exists v4l2-ctl; then
        code_block "text" "$(run_cmd "V4L/list-devices" v4l2-ctl --list-devices)
$(run_cmd "V4L/formats" v4l2-ctl --list-formats-ext | head -80)"
    else
        write "$(run_cmd_skip "V4L/v4l2-ctl" "v4l2-ctl" "v4l-utils não instalado — dnf install v4l-utils")"
    fi
    code_block "text" "$(run_cmd "V4L/dev" bash -c 'ls -la /dev/video* 2>/dev/null || echo "[nenhum /dev/video]"')"

    section 2 "Bluetooth"
    if cmd_exists bluetoothctl; then
        code_block "text" "=== bluetoothctl show ===
$(run_cmd "BT/show" bluetoothctl show)
=== bluetoothctl devices ===
$(run_cmd "BT/devices" bluetoothctl devices)"
    else
        write "$(run_cmd_skip "BT/bluetoothctl" "bluetoothctl" "bluez não instalado")"
    fi
    code_block "text" "=== dmesg bluetooth ===
$(run_cmd "BT/dmesg" dmesg | grep -i bluetooth | head -30)"

    log_section_end "PERIFÉRICOS" "$ts_sec"
}

# Analisa filesystem específico com comandos adequados ao tipo detectado
collect_fs_specific() {
    local device="$1" mountpoint="$2" fstype="$3"
    local sec="FS/${fstype}"

    section 4 "Detalhes específicos — Tipo: ${fstype} | ${device} em ${mountpoint}"

    case "$fstype" in
        btrfs)
            code_block "text" "=== btrfs filesystem show ===
$(run_cmd "${sec}/show" btrfs filesystem show "$mountpoint")
=== btrfs filesystem usage ===
$(run_cmd "${sec}/usage" btrfs filesystem usage "$mountpoint")
=== btrfs subvolume list ===
$(run_cmd "${sec}/subvol" btrfs subvolume list "$mountpoint")
=== btrfs scrub status ===
$(run_cmd "${sec}/scrub" btrfs scrub status "$mountpoint")
=== compsize — taxa de compressão real (amostra /usr) ===
$(if cmd_exists compsize; then
    run_cmd "${sec}/compsize" bash -c "compsize /usr 2>/dev/null | tail -5"
  else
    run_cmd_skip "${sec}/compsize" "compsize" "instale btrfs-compsize"
  fi)
=== btrfs check --readonly ===
$(run_cmd "${sec}/check" bash -c "btrfs check --readonly ${device} 2>&1 | head -30")"
            ;;
        ext4|ext3|ext2)
            code_block "text" "=== tune2fs -l ===
$(run_cmd "${sec}/tune2fs" tune2fs -l "$device")
=== e4defrag (análise de fragmentação) ===
$(run_cmd "${sec}/defrag" e4defrag -c "$mountpoint" | head -20)"
            ;;
        xfs)
            code_block "text" "=== xfs_info ===
$(run_cmd "${sec}/info" xfs_info "$mountpoint")
=== xfs_db frag ===
$(run_cmd "${sec}/frag" bash -c "xfs_db -c frag -r ${device} 2>&1")"
            ;;
        f2fs)
            if cmd_exists dump.f2fs; then
                code_block "text" "$(run_cmd "${sec}/dump" dump.f2fs "$device")"
            else
                write "$(run_cmd_skip "${sec}/dump.f2fs" "dump.f2fs" "instale f2fs-tools")"
            fi
            ;;
        zfs)
            code_block "text" "=== zpool status ===
$(run_cmd "${sec}/pool-status" zpool status)
=== zfs list ===
$(run_cmd "${sec}/list" zfs list)
=== compressão/dedup ===
$(run_cmd "${sec}/compress" zfs get compression,compressratio,dedup)"
            ;;
        ntfs|ntfs-3g)
            code_block "text" "=== ntfsinfo ===
$(run_cmd "${sec}/ntfsinfo" bash -c "command -v ntfsinfo && ntfsinfo -m ${device} || echo '[ntfsinfo indisponível]'")
=== ntfsfix -n (somente verificação) ===
$(run_cmd "${sec}/ntfsfix" bash -c "command -v ntfsfix && ntfsfix -n ${device} || echo '[ntfsfix indisponível]'")"
            ;;
        vfat|fat32|fat16|fat12)
            code_block "text" "=== fsck.fat -n (somente verificação) ===
$(run_cmd "${sec}/fsck-fat" bash -c "command -v fsck.fat && fsck.fat -n ${device} || echo '[fsck.fat indisponível]'")"
            ;;
        exfat)
            code_block "text" "=== fsck.exfat -n (somente verificação) ===
$(run_cmd "${sec}/fsck-exfat" bash -c "command -v fsck.exfat && fsck.exfat -n ${device} || echo '[fsck.exfat indisponível]'")"
            ;;
        *)
            write "_Tipo '${fstype}' — identificação genérica via blkid e file:_"
            code_block "text" "$(run_cmd "${sec}/blkid" blkid "$device")
$(run_cmd "${sec}/file-s" file -s "$device")"
            ;;
    esac
}

collect_storage() {
    log_step "Coletando armazenamento..."
    log_section_start "ARMAZENAMENTO"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Armazenamento"

    section 3 "Visão Geral dos Dispositivos de Bloco"
    code_block "text" "$(run_cmd "DISK/lsblk-full" lsblk -o NAME,TYPE,SIZE,FSTYPE,MOUNTPOINTS,LABEL,UUID,MODEL,SERIAL,ROTA,DISC-GRAN,DISC-MAX,PHY-SEC,LOG-SEC)"

    section 3 "lsblk -f (árvore com filesystem)"
    code_block "text" "$(run_cmd "DISK/lsblk-f" lsblk -f)"

    section 3 "Todos os Dispositivos — blkid"
    code_block "text" "$(run_cmd "DISK/blkid" blkid)"

    section 3 "Filesystems Montados — findmnt"
    code_block "text" "$(run_cmd "DISK/findmnt-D" findmnt -D)
$(run_cmd "DISK/findmnt-all" findmnt --all)"

    section 3 "Uso de Espaço — df"
    code_block "text" "$(run_cmd "DISK/df" df -hT)"

    section 3 "fstab — Montagens Permanentes"
    code_block "text" "$(run_cmd "DISK/fstab" cat /etc/fstab)"

    section 3 "Swap Ativo"
    code_block "text" "$(run_cmd "DISK/swapon" swapon --show --verbose)
$(run_cmd "DISK/proc-swaps" cat /proc/swaps)"

    section 3 "LUKS — Partições Criptografadas"
    if cmd_exists cryptsetup; then
        code_block "text" "$(run_cmd "DISK/luks" bash -c '
            devs=$(blkid -t TYPE=crypto_LUKS -o device 2>/dev/null)
            if [[ -z "$devs" ]]; then echo "[nenhuma partição LUKS detectada]"; exit 0; fi
            for dev in $devs; do
                echo "=== $dev ==="
                cryptsetup luksDump "$dev" 2>/dev/null || echo "[requer root]"
                echo ""
            done
        ')"
    else
        write "$(run_cmd_skip "DISK/cryptsetup" "cryptsetup" "não instalado")"
    fi

    section 3 "LVM"
    if cmd_exists pvdisplay; then
        code_block "text" "=== pvdisplay ===
$(run_cmd "DISK/pvdisplay" pvdisplay)
=== vgdisplay ===
$(run_cmd "DISK/vgdisplay" vgdisplay)
=== lvdisplay ===
$(run_cmd "DISK/lvdisplay" lvdisplay)"
    else
        write "$(run_cmd_skip "DISK/lvm" "pvdisplay" "lvm2 não instalado")"
    fi

    section 3 "RAID (mdadm)"
    if cmd_exists mdadm; then
        code_block "text" "$(run_cmd "DISK/mdadm" bash -c '
            found=0
            for md in /dev/md*; do
                [ -b "$md" ] && { mdadm --detail "$md"; found=1; }
            done
            [[ $found -eq 0 ]] && echo "[nenhum RAID detectado]"
        ')"
    else
        write "$(run_cmd_skip "DISK/mdadm" "mdadm" "não instalado")"
    fi

    section 3 "Detalhes por Filesystem Detectado"
    while IFS= read -r line; do
        local device mountpoint fstype
        device=$(echo "$line" | awk '{print $1}')
        mountpoint=$(echo "$line" | awk '{print $2}')
        fstype=$(echo "$line" | awk '{print $3}')
        # Ignora filesystems virtuais/kernel
        case "$fstype" in
            tmpfs|devtmpfs|sysfs|proc|cgroup*|pstore|efivarfs|securityfs|debugfs|\
tracefs|fusectl|mqueue|hugetlbfs|autofs|binfmt_misc|configfs|ramfs|\
devpts|bpf|selinuxfs|fuse.portal|overlay|squashfs) continue ;;
        esac
        [[ -z "$fstype" || "$fstype" == "-" ]] && continue
        collect_fs_specific "$device" "$mountpoint" "$fstype"
    done < <(run_cmd "DISK/findmnt-parse" findmnt -rno SOURCE,TARGET,FSTYPE 2>/dev/null)

    section 3 "NVMe — SMART e Identificação"
    local found_nvme=0
    for nvme_dev in /dev/nvme*n1; do
        [[ -b "$nvme_dev" ]] || continue
        found_nvme=1
        write "**Dispositivo: ${nvme_dev}**"
        if cmd_exists nvme; then
            code_block "text" "=== nvme smart-log ===
$(run_cmd "NVME/smart-log" nvme smart-log "$nvme_dev")
=== nvme id-ctrl ===
$(run_cmd "NVME/id-ctrl" nvme id-ctrl "$nvme_dev" | head -50)
=== nvme id-ns ===
$(run_cmd "NVME/id-ns" nvme id-ns "$nvme_dev")"
        else
            write "$(run_cmd_skip "NVME/nvme-cli" "nvme" "instale nvme-cli")"
        fi
        if cmd_exists smartctl; then
            code_block "text" "=== smartctl -a ===
$(run_cmd "NVME/smartctl" smartctl -a "$nvme_dev")"
        fi
        if cmd_exists hdparm; then
            code_block "text" "=== hdparm -I ===
$(run_cmd "NVME/hdparm" hdparm -I "$nvme_dev")"
        fi
    done
    [[ $found_nvme -eq 0 ]] && write "_Nenhum dispositivo NVMe encontrado em /dev/nvme*n1_"
    _log_write "DEBUG  " "ARMAZENAMENTO" "NVMe devices encontrados: $found_nvme"

    section 3 "Discos SATA/SAS (sdX)"
    local found_sata=0
    for disk in /dev/sd?; do
        [[ -b "$disk" ]] || continue
        found_sata=1
        write "**Disco: ${disk}**"
        if cmd_exists smartctl; then
            code_block "text" "$(run_cmd "SATA/smartctl" smartctl -a "$disk")"
        fi
        if cmd_exists hdparm; then
            code_block "text" "$(run_cmd "SATA/hdparm" hdparm -I "$disk")"
        fi
    done
    [[ $found_sata -eq 0 ]] && write "_Nenhum disco SATA encontrado em /dev/sd?_"
    _log_write "DEBUG  " "ARMAZENAMENTO" "SATA devices encontrados: $found_sata"

    section 3 "I/O Stats (iostat)"
    if cmd_exists iostat; then
        code_block "text" "$(run_cmd "DISK/iostat" iostat -x 1 3)"
    else
        write "$(run_cmd_skip "DISK/iostat" "iostat" "instale sysstat")"
    fi

    log_section_end "ARMAZENAMENTO" "$ts_sec"
}

collect_network() {
    log_step "Coletando rede..."
    log_section_start "REDE"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Rede"

    section 3 "Interfaces e IPs (ip addr)"
    code_block "text" "$(run_cmd "NET/ip-addr" ip addr)"

    section 3 "Rotas (ip route)"
    code_block "text" "$(run_cmd "NET/ip-route" ip route)
$(run_cmd "NET/ip-route6" ip -6 route)"

    section 3 "Tabela ARP / Vizinhos"
    code_block "text" "$(run_cmd "NET/ip-neigh" ip neigh)"

    section 3 "NetworkManager — Dispositivos"
    code_block "text" "$(run_cmd "NET/nmcli-dev" nmcli device status)"

    section 3 "NetworkManager — Conexões"
    code_block "text" "$(run_cmd "NET/nmcli-con" nmcli con show)"

    section 3 "DNS"
    code_block "text" "=== /etc/resolv.conf ===
$(run_cmd "NET/resolv-conf" cat /etc/resolv.conf)
=== resolvectl ===
$(run_cmd "NET/resolvectl" resolvectl status | head -50)"

    section 3 "Hostname"
    code_block "text" "Hostname: $(run_cmd "NET/hostname" hostname)
FQDN    : $(run_cmd "NET/hostname-f" hostname -f)"

    section 3 "Portas Locais Abertas (ss)"
    code_block "text" "=== TCP (ss -tlnp) ===
$(run_cmd "NET/ss-tcp" ss -tlnp)
=== UDP (ss -ulnp) ===
$(run_cmd "NET/ss-udp" ss -ulnp)"

    section 3 "Conexões Ativas"
    code_block "text" "$(run_cmd "NET/ss-active" ss -tnp | head -50)"

    section 3 "Firewall (firewalld)"
    if cmd_exists firewall-cmd; then
        code_block "text" "=== list-all ===
$(run_cmd "NET/firewall-all" firewall-cmd --list-all)
=== list-all-zones ===
$(run_cmd "NET/firewall-zones" firewall-cmd --list-all-zones | head -100)"
    else
        write "$(run_cmd_skip "NET/firewalld" "firewall-cmd" "firewalld não instalado")"
    fi

    if cmd_exists iptables; then
        section 3 "iptables"
        code_block "text" "$(run_cmd "NET/iptables" iptables -L -n -v)"
    fi

    log_section_end "REDE" "$ts_sec"
}

collect_software() {
    log_step "Coletando software instalado..."
    log_section_start "SOFTWARE"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Software Instalado"

    section 3 "Pacotes RPM — Contagem Total"
    local rpm_count
    rpm_count=$(run_cmd "PKG/rpm-count" bash -c 'rpm -qa | wc -l')
    write "Total de pacotes RPM instalados: **${rpm_count}**"
    _log_write "INFO   " "SOFTWARE" "Total RPM: ${rpm_count}"

    section 3 "Pacotes RPM — Lista Completa"
    code_block "text" "$(run_cmd "PKG/rpm-list" bash -c "rpm -qa --queryformat '%{NAME}-%{VERSION}-%{RELEASE}.%{ARCH}\n' | sort")"

    section 3 "Flatpak"
    if cmd_exists flatpak; then
        code_block "text" "$(run_cmd "PKG/flatpak" flatpak list --columns=application,name,version,branch,installation)"
    else
        write "$(run_cmd_skip "PKG/flatpak" "flatpak" "não instalado")"
    fi

    section 3 "Snap"
    if cmd_exists snap; then
        code_block "text" "$(run_cmd "PKG/snap" snap list)"
    else
        write "$(run_cmd_skip "PKG/snap" "snap" "não instalado")"
    fi

    section 3 "Python"
    code_block "text" "Versão: $(run_cmd "PKG/python-ver" python3 --version)
$(run_cmd "PKG/pip-list" bash -c 'pip3 list 2>/dev/null || pip list 2>/dev/null || echo "[pip indisponível]"')"

    section 3 "Node.js / npm"
    code_block "text" "Node: $(run_cmd "PKG/node-ver" bash -c 'command -v node && node --version || echo "[não instalado]"')
npm : $(run_cmd "PKG/npm-ver" bash -c 'command -v npm && npm --version || echo "[não instalado]"')
$(run_cmd "PKG/npm-global" bash -c 'command -v npm && npm list -g --depth=0 | head -30 || echo "[indisponível]"')"

    section 3 "Compiladores e Build Tools"
    code_block "text" "GCC  : $(run_cmd "PKG/gcc" bash -c 'gcc --version 2>/dev/null | head -1 || echo "[não instalado]"')
G++  : $(run_cmd "PKG/gpp" bash -c 'g++ --version 2>/dev/null | head -1 || echo "[não instalado]"')
Clang: $(run_cmd "PKG/clang" bash -c 'clang --version 2>/dev/null | head -1 || echo "[não instalado]"')
Make : $(run_cmd "PKG/make" bash -c 'make --version 2>/dev/null | head -1 || echo "[não instalado]"')
CMake: $(run_cmd "PKG/cmake" bash -c 'cmake --version 2>/dev/null | head -1 || echo "[não instalado]"')
Rust : $(run_cmd "PKG/cargo" bash -c 'cargo --version 2>/dev/null || echo "[não instalado]"')
Go   : $(run_cmd "PKG/go" bash -c 'go version 2>/dev/null || echo "[não instalado]"')"

    section 3 "Containers — Podman"
    if cmd_exists podman; then
        code_block "text" "=== versão ===
$(run_cmd "CONTAINER/podman-ver" podman version)
=== imagens ===
$(run_cmd "CONTAINER/podman-images" podman images)
=== containers ===
$(run_cmd "CONTAINER/podman-ps" podman ps -a)"
    else
        write "$(run_cmd_skip "CONTAINER/podman" "podman" "não instalado")"
    fi

    section 3 "Containers — Docker"
    if cmd_exists docker; then
        code_block "text" "=== versão ===
$(run_cmd "CONTAINER/docker-ver" docker version)
=== imagens ===
$(run_cmd "CONTAINER/docker-images" docker images)
=== containers ===
$(run_cmd "CONTAINER/docker-ps" docker ps -a)"
    else
        write "$(run_cmd_skip "CONTAINER/docker" "docker" "não instalado")"
    fi

    section 3 "Virtualização"
    code_block "text" "systemd-detect-virt: $(run_cmd "VIRT/detect" bash -c 'systemd-detect-virt; true')
virt-what         : $(run_cmd "VIRT/virt-what" bash -c 'command -v virt-what && virt-what || echo "[virt-what não instalado]"')"

    log_section_end "SOFTWARE" "$ts_sec"
}

collect_services() {
    log_step "Coletando serviços..."
    log_section_start "SERVIÇOS"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Serviços do Sistema"

    section 3 "Serviços em Execução"
    code_block "text" "$(run_cmd "SVC/running" systemctl list-units --type=service --state=running --no-pager)"

    section 3 "Serviços Habilitados no Boot"
    code_block "text" "$(run_cmd "SVC/enabled" systemctl list-unit-files --state=enabled --no-pager)"

    section 3 "Serviços Falhos"
    code_block "text" "$(run_cmd "SVC/failed" systemctl list-units --state=failed --no-pager)"

    section 3 "Erros no Boot Atual (journalctl -p err)"
    code_block "text" "$(run_cmd "SVC/journal-err" journalctl -b -p err --no-pager | head -100)"

    section 3 "Tempo de Boot (systemd-analyze)"
    code_block "text" "$(run_cmd "SVC/boot-time" systemd-analyze)
$(run_cmd "SVC/boot-blame" systemd-analyze blame | head -25)"

    log_section_end "SERVIÇOS" "$ts_sec"
}

collect_processes() {
    log_step "Coletando processos..."
    log_section_start "PROCESSOS"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Processos — Top 40 por CPU (excluindo kernel threads)"
    code_block "text" "$(run_cmd "PROC/top-cpu" bash -c "ps aux --no-headers | grep -v '^\[' | sort -k3 -rn | head -40")"

    section 2 "Processos — Top 40 por Memória"
    code_block "text" "$(run_cmd "PROC/top-mem" bash -c "ps aux --no-headers | grep -v '^\[' | sort -k4 -rn | head -40")"

    log_section_end "PROCESSOS" "$ts_sec"
}

collect_power() {
    log_step "Coletando perfil de energia..."
    log_section_start "ENERGIA"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "Perfil de Energia"

    code_block "text" "powerprofilesctl: $(run_cmd "PWR/profiles" bash -c 'command -v powerprofilesctl && powerprofilesctl status || echo "[não disponível]"')
tuned-adm       : $(run_cmd "PWR/tuned" bash -c 'command -v tuned-adm && tuned-adm active || echo "[não disponível]"')
TLP             : $(run_cmd "PWR/tlp" bash -c 'command -v tlp-stat && tlp-stat -s | head -20 || echo "[TLP não instalado]"')"

    section 3 "Temperatura Geral (sensors)"
    if cmd_exists sensors; then
        code_block "text" "$(run_cmd "PWR/sensors" sensors -A)"
    else
        write "$(run_cmd_skip "PWR/sensors" "sensors" "lm_sensors não instalado")"
    fi

    section 3 "Controle de Brilho (backlight)"
    code_block "text" "$(run_cmd "PWR/backlight" bash -c '
        for f in /sys/class/backlight/*/; do
            bright=$(cat "${f}brightness"     2>/dev/null || echo "?")
            maxbri=$(cat "${f}max_brightness" 2>/dev/null || echo "?")
            printf "%-40s brightness: %s / max: %s\n" "$f" "$bright" "$maxbri"
        done
    ')"

    log_section_end "ENERGIA" "$ts_sec"
}

collect_dmi() {
    log_step "Coletando DMI/SMBIOS..."
    log_section_start "DMI/SMBIOS"
    local ts_sec; ts_sec=$(date '+%s%3N')

    section 2 "DMI/SMBIOS — Informações de Hardware (dmidecode)"

    if cmd_exists dmidecode; then
        declare -A dmi_types=(
            [0]="BIOS"          [1]="System"         [2]="Baseboard"
            [3]="Chassis"       [4]="Processor"       [7]="Cache"
            [9]="System Slots"  [11]="OEM Strings"    [17]="Memory Device"
            [18]="Memory Error" [19]="Mem Array Map"  [20]="Mem Device Map"
            [32]="System Boot"  [38]="IPMI"
        )
        for type_id in 0 1 2 3 4 7 9 11 17 18 19 20 32 38; do
            local type_name="${dmi_types[$type_id]:-Type $type_id}"
            section 3 "DMI Type ${type_id} — ${type_name}"
            code_block "text" "$(run_cmd "DMI/type-${type_id}" bash -c "dmidecode -t ${type_id} | grep -v '^#' | grep -v '^\$'")"
        done
    else
        write "$(run_cmd_skip "DMI/dmidecode" "dmidecode" "instale com: dnf install dmidecode")"
    fi

    log_section_end "DMI/SMBIOS" "$ts_sec"
}

# =============================================================================
# GERAÇÃO DO HTML
# =============================================================================
generate_html() {
    log_step "Gerando HTML..."
    log_section_start "GERAÇÃO HTML"
    local ts_sec; ts_sec=$(date '+%s%3N')

    python3 - "$MD_FILE" "$HTML_FILE" "${HOSTNAME_VAL}" "${TIMESTAMP}" "${SCRIPT_VERSION}" << 'PYEOF'
import sys, html

md_file   = sys.argv[1]
html_file = sys.argv[2]
hostname  = sys.argv[3]
timestamp = sys.argv[4]
version   = sys.argv[5]

css = """
body{font-family:'Segoe UI',sans-serif;max-width:1400px;margin:0 auto;padding:20px;background:#0d1117;color:#c9d1d9}
h1,h2,h3,h4{color:#58a6ff;border-bottom:1px solid #30363d;padding-bottom:4px;margin-top:1.5em}
pre{background:#161b22;border:1px solid #30363d;border-radius:6px;padding:14px;
    overflow-x:auto;white-space:pre-wrap;word-wrap:break-word;
    font-size:12px;line-height:1.5;max-width:100%}
code{font-family:'Consolas','Liberation Mono',monospace}
.header{background:#161b22;border-radius:6px;padding:18px;margin-bottom:20px;border-left:4px solid #58a6ff}
strong{color:#79c0ff}em{color:#f0883e}
p{margin:0.4em 0}
"""

with open(md_file, 'r', encoding='utf-8', errors='replace') as f:
    lines = f.readlines()

out = []
out.append(f"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>MYCOMP — {html.escape(hostname)} — {html.escape(timestamp)}</title>
<style>{css}</style>
</head>
<body>
<div class="header">
<h1>MYCOMP — Relatório do Sistema</h1>
<p><strong>Host:</strong> {html.escape(hostname)} &nbsp;|&nbsp;
   <strong>Gerado em:</strong> {html.escape(timestamp)} &nbsp;|&nbsp;
   <strong>Versão:</strong> {html.escape(version)}</p>
</div>
""")

in_code = False
for line in lines:
    line = line.rstrip('\n')
    if line.startswith('```'):
        if not in_code:
            out.append('<pre><code>')
            in_code = True
        else:
            out.append('</code></pre>')
            in_code = False
        continue
    if in_code:
        out.append(html.escape(line) + '\n')
        continue
    # headings
    if line.startswith('#### '):
        out.append(f'<h4>{html.escape(line[5:])}</h4>')
    elif line.startswith('### '):
        out.append(f'<h3>{html.escape(line[4:])}</h3>')
    elif line.startswith('## '):
        out.append(f'<h2>{html.escape(line[3:])}</h2>')
    elif line.startswith('# '):
        out.append(f'<h1>{html.escape(line[2:])}</h1>')
    elif line.startswith('---'):
        out.append('<hr>')
    elif line.strip() == '':
        out.append('<br>')
    else:
        out.append(f'<p>{html.escape(line)}</p>')

if in_code:
    out.append('</code></pre>')

out.append('</body></html>')

with open(html_file, 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))

print(f"HTML gerado: {html_file}")
PYEOF

    log_info "HTML gerado: $HTML_FILE"
    log_section_end "GERAÇÃO HTML" "$ts_sec"
}

# =============================================================================
# MAIN
# =============================================================================
main() {
    clear
    echo "============================================================"
    echo "  MYCOMP — Gerador de Relatório do Sistema  v${SCRIPT_VERSION}"
    echo "============================================================"
    echo ""

    # Inicializa log antes de tudo
    _log_init
    log_debug "Script iniciado — PID $$ | Args: ${*:-nenhum}"

    check_root
    check_dependencies

    # Inicializa arquivo MD
    cat > "$MD_FILE" << HEADER
# Relatório de Análise do Sistema: ${HOSTNAME_VAL}

Gerado em  : ${TIMESTAMP}
Versão     : ${SCRIPT_VERSION}
Usuário    : ${USER} (EUID: ${EUID})

HEADER

    log_info "Iniciando coleta..."
    echo ""

    local ts_total
    ts_total=$(date '+%s%3N')

    collect_os
    collect_user
    collect_desktop
    collect_cpu
    collect_memory
    collect_gpu
    collect_battery
    collect_peripherals
    collect_storage
    collect_network
    collect_software
    collect_services
    collect_processes
    collect_power
    collect_dmi

    # Rodapé MD
    {
        echo ""
        echo "---"
        echo "## Análise Concluída"
        echo ""
        echo "Gerado por MYCOMP v${SCRIPT_VERSION} | ${TIMESTAMP}"
        echo "Log de debug: \`${LOG_FILE}\`"
    } >> "$MD_FILE"

    generate_html

    local ts_fim
    ts_fim=$(date '+%s%3N')
    local total_elapsed=$(( ts_fim - ts_total ))

    _log_finalize

    echo ""
    echo "============================================================"
    log_info "Concluído em ${total_elapsed}ms"
    echo ""
    log_info "Markdown : ${MD_FILE}"
    log_info "HTML     : ${HTML_FILE}"
    log_info "Log debug: ${LOG_FILE}"
    echo ""
    echo "  Resumo do log:"
    printf "  %-20s %d\n" "Comandos executados:" "$LOG_CMD_TOTAL"
    printf "  %-20s %d\n" "OK (sucesso):"        "$LOG_COUNT_OK"
    printf "  %-20s %d\n" "WARNING:"              "$LOG_COUNT_WARN"
    printf "  %-20s %d\n" "ERRO (falha):"         "$LOG_COUNT_ERR"
    printf "  %-20s %d\n" "SKIP (ausente):"       "$LOG_COUNT_SKIP"
    echo "============================================================"
}

main "$@"
