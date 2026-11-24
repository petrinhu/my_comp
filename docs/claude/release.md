# Ferramental de release do my_comp

Quando rodar o `deploy.sh`: MC-08 do `GODS_LAWS.md`. Esquema de versão: MC-12.

## Tooling

- **[git-cliff](https://git-cliff.org)** (Rust, single binary) — calcula bump SemVer pelos commits Conventional, gera/atualiza CHANGELOG.
- **`cliff.toml`** na raiz — configuração (parsers Conventional → seções pt-br do CHANGELOG, estratégia de bump).
- **`scripts/release.sh`** — orquestra: bump → atualiza VERSION + CHANGELOG → commit `chore(release): vX` → tag anotada.
- **`scripts/hooks/pre-push`** — bloqueia `git push` direto (sem release semântica). Bypass via `DEPLOY_SH_RUNNING=1`.
- **`deploy.sh`** — único caminho normal para push: build → commit opcional → release.sh → `git push --follow-tags`.
- Ativação dos hooks: `git config core.hooksPath scripts/hooks` (já feito uma vez por clone).
