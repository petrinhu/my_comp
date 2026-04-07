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
| 33 | Journal: Searchbar e Filtros Dinâmicos | Alta | Média | ✅ | Auditado. |
| 36 | Updates: Gerenciamento Universal | Alta | Alta | ✅ | **Auditado: Backend interativo OK.** |
| 36.1 | Terminal Interativo (y/n & Logs) | Alta | Alta | ✅ | **Auditado: UI/UX Terminal OK.** |
| 36.2 | Multi-provedor (DNF/Flatpak) | Alta | Média | ✅ | **Auditado: Roteamento IInstaller.** |
| 39 | Updates: Kernel/Distro/Driver info | Alta | Alta | ✅ | **Auditado: SystemInfo integrado.** |
| 38.1 | Link Duplo (Wi-Fi + Eth) e Prioridade | Alta | Alta | ❌ | Indicadores de rota ativa. |

| T1 | Testes Unitários (QtTest/GTest) | Alta | Alta | 🟡 | Em execução. |
| T4 | Análise Dinâmica (ASan/Valgrind) | Alta | Alta | ❌ | Mandato Final. |
| A4 | QA Geral C++ (Build Strict) | Alta | Média | 🟡 | Build/Testes ok. |
