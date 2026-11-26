# Execução

```bash
./build/my_comp                                   # GUI
./build/my_comp vram                              # auditoria de VRAM (tabela)
./build/my_comp vram --json                       # idem, JSON
./build/my_comp vram --all-processes --no-config-origins
```

## Plataforma gráfica

- Com sessão Wayland (`WAYLAND_DISPLAY` definido), o app usa Wayland.
- Em X11, o Qt escolhe `xcb` sozinho.
- `QT_QPA_PLATFORM=...` explícito sempre prevalece.

## Privilégios

Instalar/atualizar pacotes pede autenticação via polkit (`pkexec`). Flatpak e Cargo rodam no escopo do usuário.
