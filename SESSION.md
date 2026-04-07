# Registro de Sessão - my_comp v2.0 Alpha
**Início:** 3 de abril de 2026

## 🎨 Estrutura de UI Aprovada (Mission Control)
- **Navegação:** Sidebar Kirigami fixa com 6 abas lógicas.
- **Sistema de Temas:** Implementação nativa de Breeze Light, Breeze Dark e modo Auto (seguindo sistema).
- **Controle de Tema:** Botão único na Sidebar que alterna ciclicamente entre Light -> Dark -> Auto, com ícones dinâmicos (Sol, Lua, Tema).
- **Abas:**
  1. **Dashboard:** Monitores em tempo real (CPU, GPU, I/O, Rede, Heatmap, Watts).

  2. **Updates:** Gerenciamento multi-provedor (DNF, Flatpak, NPM, Cargo, Go, Snap).
  3. **Journal:** Analisador forense com filtros dinâmicos.
  4. **Hardware:** Topologia exaustiva (USB, PCI, SMBIOS, SMART, Partições).
  5. **Security:** Auditoria de portas, firewall, SELinux e logins.
  6. **Export:** Geração de relatórios paridade legado (MD/HTML/JSON).

## 📋 Tabela de Pendências por Prioridade (43 Itens)

| # | Pendência | Prioridade | Dificuldade | Estado | Auditoria |
| :-- | :--- | :--- | :--- | :--- | :--- |
| 1 | Arquitetura Hexagonal (Foundation) | Alta | Média | ✅ | Domínio isolado. |
| 2 | Mandato Wayland-Native | Alta | Baixa | ✅ | Suporte Wayland nativo. |
| 3 | TDD & ATDD Infrastructure | Alta | Média | ✅ | GTest/GMock operacionais. |
| 4 | Manager: Update Search (Orquestrador) | Alta | Alta | ✅ | Gerenciamento multi-provedor. |
| 5 | Provider: DNF (System) | Alta | Média | ✅ | Validado para base RPM. |
| 9 | Journal: Full Analyzer (Completo) | Alta | Alta | ✅ | **Auditado: Implementação Hexagonal e ATDD OK.** |
| 10 | Journal: Filtro de Erros Críticos | Alta | Média | ✅ | **Auditado: Filtro -p 3 ativo e testado.** |
| 12 | Journal: Filtro de Hardware/Firmware | Alta | Média | ✅ | fwupd, smartd, kernel kernel. |
| 15 | Monitor: CPU Real-time (Load/Freq) | Alta | Média | ✅ | **Auditado: Coleta real via /proc, Deep Dive funcional.** |
| 19 | Monitor: RAM & Swap (ZRAM/Usage) | Alta | Baixa | ✅ | **Coleta via /proc/meminfo e ZRAM via /sys/block/zram0.** |
| 17 | Monitor: Firmware (LVFS/fwupd) | Alta | Média | ✅ | **Integração nativa D-Bus com org.freedesktop.fwupd.** |
| 20 | Monitor: Storage (I/O Throughput/SMART) | Alta | Média | ✅ | **Coleta via /proc/diskstats com cálculo de MB/s.** |
| 28 | Segurança: AES-256-GCM Vault | Alta | Alta | ❌ | Persistência local criptografada. |
| 29 | Gerenciador: Cargo/Go/Snap/APT/Pacman | Alta | Alta | ❌ | Expansão Multi-distro. |
| T1 | Testes Unitários (QtTest/GTest) | Alta | Alta | 🟡 | **Em execução (Updates e Journal validados).** |
| T2 | Análise Estática (Clang-Tidy/CppCheck) | Alta | Média | ❌ | Mandato Final. |
| T4 | Análise Dinâmica (ASan/Valgrind) | Alta | Alta | ❌ | Mandato Final. |
| T8 | Verificação de Secrets (Gitleaks) | Alta | Baixa | ❌ | Mandato Final. |
| T10 | SQL Injection (Sqlmap) | Alta | Média | ❌ | Mandato Final. |
| T12 | Busca de CVEs nas Dependências | Alta | Média | ❌ | Mandato Final. |
| A1 | Descoberta e Modelagem | Alta | Baixa | ❌ | Auditoria Final. |
| A2 | Auditoria de Arquitetura/Camadas | Alta | Média | ❌ | Auditoria Final. |
| A4 | QA Geral C++ (Build Strict) | Alta | Média | ❌ | Auditoria Final. |
| A5 | Análise Estática/Dinâmica Profunda | Alta | Alta | ❌ | Auditoria Final. |
| A6 | Cobertura de Testes (LCOV 70%+) | Alta | Média | ❌ | Auditoria Final. |
| A9 | Análise Arquitetural (SOLID/SRP) | Alta | Alta | ❌ | Auditoria Final. |
| A10 | Relatório Final de Auditoria | Alta | Baixa | ❌ | Entrega Final. |
| 6 | Provider: Flatpak | Média | Média | ✅ | Suporte a pacotes universais. |
| 7 | Provider: NPM (Node.js Global) | Média | Média | ✅ | Suporte a dev-tools. |
| 8 | Interface: UI de Atualizações (QML) | Média | Média | ✅ | ListView reativa integrada. |
| 11 | Journal: Filtro de Carregamento (Boot) | Média | Média | ✅ | **Auditado: Comando journalctl -b filtrado por unidade.** |
| 33 | Journal: Searchbar e Filtros Dinâmicos | Alta | Média | ❌ | Implementar busca e menu de categorias. |
| 34 | Journal: Numeração de Linhas (Texto Original) | Alta | Baixa | ❌ | Mostrar índice real da linha no journalctl. |
| 35 | Journal: Double-Click Search (Navegador) | Média | Baixa | ❌ | Abrir busca no Google via QDesktopServices. |
| 36 | Updates: Instalação Individual (Double-Click) | Alta | Alta | ❌ | Fluxo de instalação com terminal/verbose e sudo. |
| 37 | Updates: Botão "Atualizar Tudo" | Alta | Alta | ❌ | Orquestrar atualização multi-provedor com feedback. |
| 13 | Journal: Filtro de KDE/Wayland | Média | Média | ✅ | **Auditado: Filtro por _COMM=kwin_wayland e plasmashell.** |
| 14 | Journal: Filtro de Segurança/Network | Média | Média | ✅ | **Auditado: sshd, firewalld e sudo logs verificados.** |
| 16 | Monitor: GPU Real-time (Load/Temp/VRAM) | Média | Alta | ❌ | Multi-vendor (Nvidia/Intel/AMD). |
| 18 | Monitor: Bateria & Energia (Watts/Saúde) | Média | Baixa | ❌ | Real-time: Power draw em Watts. |
| 21 | Monitor: Network (Latency/Traffic/Ping) | Média | Baixa | ✅ | **Coleta via /proc/net/dev e Latência via ICMP Ping.** |
| 23 | Collector: Hardware Topology | Média | Baixa | ❌ | USB Tree, Bluetooth paired, SMBIOS. |
| 24 | Collector: Systemd Services | Média | Baixa | ❌ | Gerenciamento de unidades e falhas. |
| 26 | Collector: Dev Stacks (Compilers/Cont) | Média | Média | ❌ | Toolchain info + Podman/Docker. |
| 30 | Áudio: FFT Waveform (Real-time) | Média | Alta | ❌ | Visualizador de ondas (QtMultimedia). |
| T3 | Fuzzing de Inputs (AFL++) | Média | Alta | ❌ | Mandato Final. |
| T5 | Scanning de Dependências (Trivy) | Média | Média | ❌ | Mandato Final. |
| T6 | Teste de APIs (Hurl/FFUF) | Média | Média | ❌ | Mandato Final. |
| T7 | Scanning de Binário (Radare2) | Média | Média | ❌ | Mandato Final. |
| T9 | Teste de Rede (Nmap) | Média | Baixa | ❌ | Mandato Final. |
| T11 | Fuzzing de Protocolos de Rede | Média | Alta | ❌ | Mandato Final. |
| A3 | UI/UX e Acessibilidade (WCAG) | Média | Média | ❌ | Auditoria Final. |
| A7 | Dependências e Acoplamento | Média | Média | ❌ | Auditoria Final. |
| 22 | Collector: User & Env Metadata | Baixa | Baixa | ❌ | Login history, Env Vars, limits. |
| 25 | Collector: Process List (Top) | Baixa | Baixa | ❌ | Top processos por recurso. |
| 27 | Export: Multi-format (MD/HTML/JSON) | Baixa | Média | ❌ | Relatórios completos agnósticos. |
| A8 | Consistência .h vs .cpp | Baixa | Baixa | ❌ | Auditoria Final. |
