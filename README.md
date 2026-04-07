# my_comp v2.0 Alpha

Uma evolução do gerador de relatórios do sistema Linux para uma aplicação nativa de alta performance e visual moderno.

## Características Principais
- **Core:** C++20 com Qt6.
- **Ambiente:** Otimizado para Wayland (KDE Plasma 6).
- **Arquitetura:** Hexagonal (Ports & Adapters) para facilidade de testes e extensibilidade.
- **UI/UX:** Design nativo inspirado no Breeze (Light/Dark) via Kirigami.
- **Segurança:** Zero-Cloud com criptografia local AES-256-GCM.
- **Monitoramento Real-time:** Dashboard para CPU, GPU, RAM, VRAM, Wi-Fi e Áudio.

## Como Compilar
```bash
cmake -B build -G Ninja
cmake --build build
```

## Testes
O projeto utiliza **TDD/ATDD** via GoogleTest.
```bash
cd build && ctest
```

## Formatos de Exportação
JSON, XML, CSV e Markdown.
