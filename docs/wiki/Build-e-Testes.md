# Build e testes

## Dependências (Fedora)

```bash
sudo dnf install cmake ninja-build gcc-c++ qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtmultimedia-devel qt6-qtwayland-devel openssl-devel gtest-devel gmock-devel
```

## Compilar

```bash
cmake -B build -G Ninja
cmake --build build -j$(nproc)
```

O núcleo é compilado uma vez (`my_comp_core`) e linkado pelo app e pelas duas suítes.

## Testar

```bash
cd build && ctest --output-on-failure
```

- `unit_tests`: herméticos (mocks; não dependem de distro nem hardware).
- `acceptance_tests`: tocam o sistema real; usam `GTEST_SKIP` quando o recurso (bateria, ferramenta) não existe.

## Gates (MC-07)

```bash
./scripts/ci.sh                 # build + ctest + clang-tidy + cppcheck + contraste + texto puro QML + valgrind
./scripts/ci.sh --quick         # só build + ctest
./scripts/coverage.sh           # cobertura de linhas por camada (gcovr)
```

No GitHub, o workflow `.github/workflows/ci.yml` roda em todo PR e push na `main`, em três jobs:

- **Ubuntu 24.04:** o `scripts/ci.sh` inteiro, menos o clang-tidy.
- **Gate T2:** o clang-tidy em paralelo.
- **Fedora 43:** build e testes com o GCC do SO alvo; o resumo do job mostra a contagem de warnings.

| Gate | Critério |
| :--- | :--- |
| Build | zero erros, zero novos warnings |
| T1 Unit | 100% passando |
| T2 Estática | zero `bugprone-*`/`security-*`/`cert-*` |
| T4 Dinâmica | zero *definitely lost*, zero ERROR SUMMARY |
| A6 Cobertura | medida, sem meta numérica |
