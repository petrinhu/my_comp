# Release

SemVer 2.0 com pré-release, bump automático pelo git-cliff a partir de commits Conventional. Regras na lei MC-12; ferramental em [`docs/claude/release.md`](https://github.com/petrinhu/my_comp/blob/main/docs/claude/release.md).

| Último tag | Commit | Próxima versão |
| :--- | :--- | :--- |
| `v0.1.0-alpha.4` | qualquer releasável | `v0.1.0-alpha.5` |
| `v0.1.0` | `feat:`/`fix:` | `v0.1.1` |
| `v0.1.0` | `BREAKING CHANGE:` | `v1.0.0` |
| `v1.0.0` | `feat:` | `v1.1.0` |

```bash
./scripts/release.sh --dry-run   # preview
./deploy.sh                      # build + release + push --follow-tags
```
