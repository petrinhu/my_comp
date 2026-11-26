# Arquitetura

```
Presentation  →  Application  →  Domain  ←  Infrastructure
                         ↑ composição (DI manual): src/main.cpp + src/composition/
```

| Camada | Diretório | Regra |
| :--- | :--- | :--- |
| Domain | `src/domain/` | Entidades e interfaces `IPascalCase`. Sem Qt GUI, OpenSSL ou processo externo. |
| Application | `src/application/` | Casos de uso; depende só do Domain. |
| Infrastructure | `src/infrastructure/` | Implementa as interfaces: `/proc`, `/sys`, D-Bus, OpenSSL, QtAudio, NVML, exportadores. |
| Presentation | `src/presentation/` | ViewModels `Q_OBJECT`, CLI e QML. Não instancia infraestrutura. |
| Composition | `src/composition/` | Um `compor_<feature>()` por feature (sistema, armazenamento, processos, atualizações, VRAM); o `main.cpp` só escolhe CLI/GUI e publica os ViewModels. |

## QML

- `Main.qml`: janela, navegação e overlays.
- `Theme.qml` (singleton): paleta central claro/escuro; nenhuma cor fixa fora dele.
- `views/`: uma view por aba. `components/`: blocos reutilizáveis.
- Views falam com o Main por signals e propriedades explícitos.

## Concorrência

Coleta pesada (nvidia-smi, smartctl, gerenciadores de pacote) roda em `QtConcurrent`; o resultado volta à thread de UI por `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`. Snapshots que alimentam workers são tirados na thread principal.

## Segurança (Zero-Cloud)

- Nenhuma telemetria. A única saída para a internet é a medição de latência (ping a 1.1.1.1), **opt-in** e desligada por padrão.
- Persistência sensível via `ISecurityVault` (AES-256-GCM), com validação de IV/tag.
- Toda entrada externa é validada: saída de processos, `/proc`, `/sys`, nomes de pacote antes de instalar, PIDs antes de `kill`/`setpriority`.
- Elevação de privilégio via polkit (`pkexec`), nunca `sudo` na GUI.
