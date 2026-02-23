# my_comp

**Gerador de Relatório de Configuração do Sistema Linux**

Coleta informações exaustivas do sistema e gera relatório em Markdown e HTML, com log completo de debug.

## Funcionalidades

- Coleta de OS, CPU, memória, GPU, bateria, armazenamento, rede e periféricos
- Informações de desktop environment, serviços systemd e processos
- Dados DMI/SMBIOS via `dmidecode`
- Perfil de energia (powerprofilesctl, tuned, TLP, sensors)
- Saída em Markdown + HTML (pandoc ou fallback sed)
- Log de debug detalhado com timestamps, exit codes e métricas por comando

## Requisitos

- Bash 4+
- `sudo` / root (necessário para dmidecode e algumas coletas)
- Recomendados: `pandoc`, `dmidecode`, `lm_sensors`, `lspci`, `lsusb`

```bash
sudo dnf install pandoc dmidecode lm_sensors pciutils usbutils
```

## Uso

```bash
sudo bash my_comp.sh [/caminho/de/saida]
```

Se nenhum caminho for fornecido, os arquivos são gerados no diretório atual.

### Saída

| Arquivo | Descrição |
|---|---|
| `MYCOMP_<timestamp>.md` | Relatório principal em Markdown |
| `MYCOMP_<timestamp>.html` | Relatório em HTML |
| `MYCOMP_debug_<timestamp>.log` | Log completo de debug |

## Versão

v0.3.0 — veja [CHANGELOG.md](CHANGELOG.md)

## Licença

MIT — veja [LICENSE](LICENSE)
