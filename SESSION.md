# Registro de Sessão - my_comp v2.0 Alpha
**Início:** 3 de abril de 2026

## 🎨 Estrutura de UI Aprovada (Mission Control)
- **Navegação:** Sidebar Kirigami fixa com 6 abas lógicas.
- **Sistema de Temas:** Implementação nativa de Breeze Light, Breeze Dark e modo Auto.
- **Abas:** Dashboard, Updates, Journal, Network, Hardware, Security, Export.

## 📋 Tabela de Pendências Completa

| # | Pendência | Prioridade | Dificuldade | Estado | Auditoria |
|:--|:---|:---|:---|:---|:---|
| 1 | Arquitetura Hexagonal (Foundation) | Alta | Média | ✅ | Domínio isolado. |
| 2 | Mandato Wayland-Native | Alta | Baixa | ✅ | Suporte Wayland nativo. |
| 3 | TDD & ATDD Infrastructure | Alta | Média | ✅ | GTest/GMock operacionais. |
| 4 | Manager: Update Search (Orquestrador) | Alta | Alta | ✅ | Gerenciamento multi-provedor. |
| 5 | Provider: DNF (System) | Alta | Média | ✅ | Validado para base RPM. |
| 9 | Journal: Full Analyzer (Completo) | Alta | Alta | ✅ | Auditado. |
| 15 | Monitor: CPU Real-time (Load/Freq) | Alta | Média | ✅ | Auditado. |
| 16 | Monitor: GPU Real-time (RTX 3050) | Média | Alta | ✅ | Auditado (NVIDIA + Sysfs). |
| 17 | Monitor: Firmware (LVFS/fwupd) | Alta | Média | ✅ | Integração D-Bus. |
| 18 | Monitor: Bateria & Energia (Watts) | Média | Baixa | ✅ | Auditado (Sysfs). |
| 19 | Monitor: RAM & Swap (ZRAM/Usage) | Alta | Baixa | ✅ | Coleta via /proc. |
| 20 | Monitor: Storage (I/O / SMART) | Alta | Média | ✅ | Coleta via /proc. |
| 21 | Monitor: Network (Latency/Traffic) | Média | Baixa | ✅ | Via /proc/net/dev. |
| 28 | Segurança: AES-256-GCM Vault | Alta | Alta | ✅ | Auditado (OpenSSL). |
| 33 | Journal: Searchbar e Filtros Dinâmicos | Alta | Média | ❌ | Busca e menu de categorias. |
| 36 | Updates: Instalação Individual | Alta | Alta | ❌ | Fluxo sudo/terminal. |
| 38.1 | Link Duplo (Wi-Fi + Eth) e Prioridade | Alta | Alta | ❌ | Indicadores de rota ativa. |
| 38.2 | IP Stack (Local, Público, GW, DNS) | Alta | Média | ❌ | Endereçamento completo. |
| 38.3 | Latência e Throughput (Gráfico) | Alta | Alta | ❌ | Ping/Speed real-time. |
| 38.6 | Scanner de Rede Local (ARP/Nmap) | Alta | Média | ❌ | Lista de dispositivos. |
| 38.6.1 | Descoberta Específica: Alexa | Alta | Média | ❌ | mDNS/MAC signature. |
| 38.6.2 | Mapper de Fabricantes (MAC/OUI DB) | Média | Baixa | ❌ | Identificação via OUI. |
| 38.7 | Geolocalizador de IP Público | Baixa | Baixa | ❌ | Info geográfica. |
| 38.8 | Verificador de Portas e Serviços | Média | Média | ❌ | Diagnóstico de serviços. |
| 38.9 | Teste de Performance DNS | Baixa | Média | ❌ | Comparativo. |
| 38.10 | Histórico de Uptime de Conexão | Média | Média | ❌ | Logs de quedas. |
| 38.11 | Monitor de Packet Loss e Jitter | Alta | Alta | ❌ | Instabilidade real-time. |
| 38.12 | Monitor de Rede por Aplicativo (Top) | Alta | Alta | ❌ | Consumo por processo. |
| 38.13 | Ferramenta de Teste de MTU | Baixa | Média | ❌ | Otimização. |
| 38.14 | Wake-on-LAN (WoL) Trigger | Média | Alta | ❌ | Ligar remoto. |
| 39 | Updates: Kernel/Distro/Driver info | Alta | Alta | ❌ | Info de sistema. |
| T1 | Testes Unitários (QtTest/GTest) | Alta | Alta | 🟡 | Em execução. |
| T4 | Análise Dinâmica (ASan/Valgrind) | Alta | Alta | ❌ | Mandato Final. |
| A4 | QA Geral C++ (Build Strict) | Alta | Média | 🟡 | Build/Testes ok. |
