# Registro de Sessão — my_comp v2.0 Alpha
**Data:** 16 de abril de 2026

## 🎯 Objetivo da Sessão
Resolver instabilidade de memória (SEGV) e estabilizar a inicialização do sistema em ambiente Fedora/Wayland.
## ✅ Progresso Realizado
- [x] **Estabilidade**: Corrigido crash (SIGSEGV) no `ProcessViewModel` causado por comparador `std::sort` inválido (violação de strict weak ordering).
- [x] **Estabilidade**: Blindagem de concorrência no `ProcessViewModel` usando `std::atomic<bool>` para `m_isBusy`.
- [x] **Estabilidade**: Adicionado mutex no `LinuxProcessProvider` para garantir thread-safety durante coleta assíncrona.
- [x] **Testes**: Criado `process_sorting_test.cpp` para validar lógica de ordenação e evitar regressões.
- [x] **Estabilidade**: Resolvida corrupção de heap causada por chamadas inseguras ao DBus (Firmware).
...
- [x] **Estabilidade**: Resolvida condição de corrida no Áudio usando `QMetaObject::invokeMethod`.
- [x] **Estabilidade**: Resolvida corrupção de heap no Parsing de Processos (Remoção de substrings inseguras).
- [x] **Estabilidade**: Blindagem de funções libc não reentrantes (`getpwuid_r`, `getgrgid_r`).
- [x] **Arquitetura**: Implementada inicialização progressiva (Lazy Init) para evitar sobrecarga de processos.
- [x] **Audio**: Refatorado `QtAudioProvider` usando composição para isolar ciclo de vida do backend Pipewire.

## 🛑 Diagnóstico de UI/UX (Avaliação Manual v1)
1. **Dados Vazios**: Dashboard, Journal e Hardware não carregam automaticamente (requerem refresh manual).
2. **Interatividade Nula**: Cliques nos cards do Dashboard não geram ações.
3. **UX Crítica (0/10)**: O seletor de pasta de exportação está confuso, sem árvore de arquivos e não nativo do KDE.
4. **Layout**: Falta de barras de rolagem em listas longas (Explorador de Processos).

## 🚀 Próximos Passos (Sprint de UX e Integração)
1. **Item 43**: Implementar sequenciador de carga assíncrona para que os dados apareçam sozinhos sem crashar.
2. **Item 41**: Integrar `StandardFileDialog` ou `KDE Portal` para seletor de pastas nativo.
3. **Item 40**: Corrigir Scrollbars e UX de navegação interna.
4. **Item 37/39**: Reativar monitoramento em tempo real de CPU/Audio de forma segura.
