# 📋 Tabela de Verdade — my_comp v2.0 Alpha
*Este arquivo é a fonte única de verdade para o progresso do projeto.*

| # | Pendência | Prioridade | Dificuldade | Estado | Auditoria |
| :-- | :--- | :--- | :--- | :--- | :--- |
| 1 | Arquitetura Hexagonal (Foundation) | Alta | Média | ✅ | Domínio isolado. |
| 2 | Mandato Wayland-Native | Alta | Baixa | ✅ | Suporte Wayland nativo. |
| 3 | TDD & ATDD Infrastructure | Alta | Média | ✅ | GTest/GMock operacionais. |
| 4 | Manager: Update Search (Orquestrador) | Alta | Alta | ✅ | Gerenciamento multi-provedor. |
| 5 | Provider: DNF (System) | Alta | Média | ✅ | Validado para base RPM. |
| 6 | Provider: Flatpak | Média | Média | ✅ | Suporte a pacotes universais. |
| 7 | Provider: NPM (Node.js Global) | Média | Média | ✅ | Suporte a dev-tools. |
| 8 | Interface: UI de Atualizações (QML) | Média | Média | ✅ | ListView reativa integrada. |
| 9 | Journal: Full Analyzer (Completo) | Alta | Alta | ✅ | Implementação Hexagonal e ATDD OK. |
| 10 | Journal: Filtro de Erros Críticos | Alta | Média | ✅ | Filtro -p 3 ativo e testado. |
| 11 | Journal: Filtro de Carregamento (Boot) | Média | Média | ✅ | Auditado via acceptance tests. |
| 12 | Journal: Filtro de Hardware/Firmware | Alta | Média | ✅ | fwupd, smartd, kernel kernel. |
| 13 | Journal: Filtro de KDE/Wayland | Média | Média | ✅ | Auditado via acceptance tests. |
| 14 | Journal: Filtro de Segurança/Network | Média | Média | ✅ | Auditado via acceptance tests. |
| 15 | Monitor: CPU Real-time (Load/Freq) | Alta | Média | ✅ | Coleta real via /proc. |
| 16 | Monitor: GPU Real-time (Load/Temp/VRAM) | Média | Alta | ✅ | **Auditado: NVIDIA RTX 3050 (via nvidia-smi CSV).** |
| 17 | Monitor: Firmware (LVFS/fwupd) | Alta | Média | ✅ | Integração nativa D-Bus. |
| 18 | Monitor: Bateria & Energia (Watts/Saúde) | Média | Baixa | ✅ | **Auditado: Real-time via Sysfs (BAT1).** |
| 19 | Monitor: RAM & Swap (ZRAM/Usage) | Alta | Baixa | ✅ | Coleta via /proc/meminfo e ZRAM. |
| 20 | Monitor: Storage (I/O Throughput/SMART) | Alta | Média | ✅ | Coleta via /proc/diskstats (MB/s). |
| 21 | Monitor: Network (Latency/Traffic/Ping) | Média | Baixa | ✅ | Coleta via /proc/net/dev e ICMP Ping. |
| 22 | Collector: User & Env Metadata | Baixa | Baixa | ✅ | **Auditado: Logins, Env Vars e Limits via C API/Shell.** |
| 23 | Collector: Hardware Topology | Média | Baixa | ✅ | **Auditado: SMBIOS (sysfs), USB (lsusb) e BT (bluetoothctl).** |
| 24 | Collector: Systemd Services | Média | Baixa | ✅ | **Auditado: Controle (Start/Stop/Enable) e Criador de Daemons.** |
| 25 | Collector: Process List (Top) | Baixa | Baixa | ❌ | Top processos por recurso. |
| 26 | Collector: Dev Stacks (Compilers/Cont) | Média | Média | ❌ | Toolchain info + Podman/Docker. |
| 27 | Export: Multi-format (MD/HTML/JSON) | Baixa | Média | ❌ | Relatórios completos agnósticos. |
| 28 | Segurança: AES-256-GCM Vault | Alta | Alta | ✅ | Auditado via OpenSSL (100% offline). |
| 29 | Gerenciador: Cargo/Go/Snap/APT/Pacman | Alta | Alta | ❌ | Expansão Multi-distro. |
| 30 | Áudio: FFT Waveform (Real-time) | Média | Alta | ❌ | Visualizador de ondas (QtMultimedia). |
| 31 | UI/UX: Refatoração de DashboardCards | Alta | Baixa | ✅ | **Auditado: Layout responsivo (2-4 colunas) e 8 cards integrados.** |
| 32 | Infra: Sistema de Refresh em Tempo Real | Alta | Média | ✅ | Atualização dinâmica integrada. |
| 33 | Journal: Searchbar e Filtros Dinâmicos | Alta | Média | ✅ | **Auditado: Busca por texto unificada a categorias (journalctl -g).** |
| 34 | Journal: Numeração de Linhas (Texto Original) | Alta | Baixa | ✅ | **Auditado: Índice sequencial integrado ao modelo LogEntry.** |
| 35 | Journal: Double-Click Search (Navegador) | Média | Baixa | ❌ | Abrir busca no Google. |
| 36 | Updates: Instalação Individual (Double-Click) | Alta | Alta | ❌ | Fluxo de instalação com terminal/verbose. |
| 37 | Updates: Botão "Atualizar Tudo" | Alta | Alta | ✅ | **Auditado: Orquestração interativa via InstallerAdapter.** |
| T1 | Testes Unitários (QtTest/GTest) | Alta | Alta | 🟡 | Em execução (Novos testes para Memory/Net). |
| T2 | Análise Estática (Clang-Tidy/CppCheck) | Alta | Média | ❌ | Mandato Final. |
| T4 | Análise Dinâmica (ASan/Valgrind) | Alta | Alta | ❌ | Mandato Final. |
| A3 | UI/UX e Acessibilidade (WCAG) | Média | Média | ❌ | Auditoria Final. |
| A9 | Análise Arquitetural (SOLID/SRP) | Alta | Alta | ❌ | Auditoria Final. |
| A10 | Relatório Final de Auditoria | Alta | Baixa | ❌ | Entrega Final. |
