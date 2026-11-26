# Ferramental de release

Detalha o "como" da release semântica. As regras (o "o quê") estão na lei
[MC-12](../../GODS_LAWS.md#mc-12) (versão, tag, `VERSION`, `CHANGELOG.md`), e o
fluxo pós-feature na [MC-08](../../GODS_LAWS.md#mc-08).

## Peças

| Peça | Papel |
| :--- | :--- |
| [git-cliff](https://git-cliff.org) | Calcula o próximo SemVer pelos commits Conventional e gera o `CHANGELOG.md`. |
| `cliff.toml` | Configuração: parsers Conventional → seções pt-br do CHANGELOG e estratégia de bump. |
| `scripts/release.sh` | Orquestra: bump → atualiza `VERSION` + `CHANGELOG.md` → commit `chore(release): vX` → tag anotada. |
| `scripts/hooks/pre-push` | Bloqueia `git push` direto (sem release). Bypass explícito: `DEPLOY_SH_RUNNING=1`. |
| `deploy.sh` | Caminho normal de push: build → commit opcional → `release.sh` → `git push --follow-tags` para o GitHub. |
| `scripts/ci.sh` | Pré-CI local (build, ctest, clang-tidy, cppcheck, valgrind); rodar antes do `deploy.sh` (MC-07). |

## Regras de bump (git-cliff 2.14, conferidas empiricamente)

| Situação do último tag | Commit | Próxima versão |
| :--- | :--- | :--- |
| `v0.1.0-alpha.4` (pré-release) | qualquer releasável | `v0.1.0-alpha.5` |
| `v0.1.0` (0.x) | `feat:` ou `fix:` | `v0.1.1` |
| `v0.1.0` (0.x) | `BREAKING CHANGE:` | `v1.0.0` |
| `v1.0.0` | `feat:` | `v1.1.0` |
| `v1.1.0` | `fix:` | `v1.1.1` |

`chore(release)` é ignorado pelo parser. Sair do pré-release (ex.: o marco
`BT-04` cortar `v1.0.0-beta.0`) é decisão consciente: tag manual, não bump automático.

## Fluxo prático

```bash
# Caminho normal (release automática + push):
./deploy.sh                              # releaseia commits já criados e pusha
./deploy.sh "feat(x): mensagem"          # commita pendências + releaseia + pusha

# Pré-validação (sem alterar nada):
./scripts/release.sh --dry-run

# Setup inicial em clone novo:
git config core.hooksPath scripts/hooks
cargo install git-cliff   # ou: pip install git-cliff / dnf install git-cliff
```

O `release.sh` cria o baseline `v0.1.0-alpha.0` em `origin/main` se não houver
nenhum tag `v*` (clone sem tags: rode `git fetch --tags` antes).

O commit `chore(release)` não leva linha de coautoria fixa: a atribuição é de
quem roda o release.
