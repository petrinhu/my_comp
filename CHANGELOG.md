# Changelog

Todas as mudanças notáveis deste projeto serão documentadas neste arquivo seguindo [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) e [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0-alpha.6] - 2026-11-26

### Corrigido

- **release**: Exige git-cliff >= 2.14 em vez de pular a release em silêncio ([`c240989`](#c2409898ce827b92cad061985999abee293cf6ee))
- **release**: Pula só o commit de bump, não todo chore(release) ([`ff59610`](#ff59610cf178c4e2c7a144d048ebdcb5fe39a323))
- **deploy**: Configura o CMake quando falta e mostra o erro do build ([`f11da50`](#f11da50543f58652442724f1def9e8411fe50762))
- **viewmodels**: Inicializa ponteiros de QTimer em nullptr ([`f842fe0`](#f842fe0ebe6990453a3eb4f360e8a72d5b2ea15e))
- **lint**: Troca QString globais por constexpr QLatin1String (cert-err58) ([`6788a99`](#6788a99d218aca9524ef0fb7a8742fe0e7cb7fe9))
- **main**: Só força Wayland quando há sessão Wayland ([`8693f40`](#8693f408e36168c74cfae5722bc4e2180e01696e))
- **qml**: Corrige ReferenceError nos rótulos de prioridade do journal ([`4f98a9e`](#4f98a9ef7890606cf6fab9f9b45deead3259427e))
- **lint**: Zera os achados bugprone/cert do gate T2 ([`5b0030e`](#5b0030ea8ab084d4784c3c1b806b91841080f227))
- **viewmodels**: Checa QPointer uma vez antes de usar nos callbacks ([`c5ad4e1`](#c5ad4e1feeee0aaab3f2326be19b2467d8f1dfdf))
- **vram**: Audita contextos gráficos, monta a cascata completa num só lugar ([`462caf5`](#462caf519eb6f4831e2da96b952105acfc98a4ea))
- **viewmodels**: Tira coleta de GPU da UI, elimina race no export e faz ping opt-in ([`79f7a97`](#79f7a97a48facc63d83891857aa5e637bceea436))
- **security**: Valida PID, IV/tag do cofre e nome de pacote; timeout por chamada ([`dd21919`](#dd21919640e83438cceba8820fddd413d1e39fa8))

### Refatorado

- **qml**: Quebra o Main.qml monolítico em views, componentes e Theme ([`f8f180b`](#f8f180b0cf8da5c395ab1b348260ac39a8492fb4))
- **composition**: Quebra o main.cpp em um módulo de composição por feature ([`7f1523d`](#7f1523d9321f77e3b222d006e2667f7e5edf9963))
- **power**: Injeta IPowerProvider pelo composition root ([`d6de203`](#d6de20329f8d2bb5e5824269c9f8b80fe0e42b08))
- **style**: Troca casts C e NULL e usa o retorno de getgroups ([`f8b6cef`](#f8b6cef3dbbf7b2c44f1ccf8f4ca0900179cc824))
- **naming**: Adota prefixo m_ e alinha .clang-tidy à MC-03 ([`37adb0d`](#37adb0d78b7f1437c1bc1ec9a6e9bb62b31d0fdd))
- **qml**: Centraliza cores na paleta e envolve textos em qsTr ([`a73f189`](#a73f1892f1b06a5ede4ea801c9bc871b065edb3a))

### Documentação

- **todo**: Fecha AUDIT-27, wiki publicada ([`8d95510`](#8d95510609b4db7d54afe9ec11a590571acf14fd))
- Remove travessões das páginas da wiki e do release.md ([`f7ad9a7`](#f7ad9a7c4f754ffa228aa3a05f8b563deba2482b))
- **todo**: Registra a auditoria de 2026-11-26 e atualiza a contagem de warnings ([`0bfadeb`](#0bfadeb0933b3243e2ae1abae65a246419d054c8))
- **wiki**: Adiciona barra lateral de navegação da wiki ([`c61cfc6`](#c61cfc69307612effd99d1b44a50d81052ecd6f9))
- **wiki**: Adiciona fonte versionada das páginas da wiki com badges ([`ff0b8fd`](#ff0b8fd4baedd16fc40e3a41c809de1845fd6d39))
- **readme**: Adiciona badges e atualiza para GitHub, camadas e gates atuais ([`353d323`](#353d32351f86ad446742138e644c5089b8f746a6))
- **testes**: Anota que a meta de cobertura vale pela MC-07 ([`7f643cf`](#7f643cf710adb4bda234b57260712db49c821573))
- Cria docs/claude/release.md e troca citações 'CLAUDE.md §N' por MC-NN ([`ca24819`](#ca24819d6e5a55e20205fa5fe18e5c9b16b8ee26))
- **laws**: Alinha a MC-12 ao bump real do git-cliff ([`a5bb843`](#a5bb843eb6e3efbe83c6974cdcd0b35685a19f02))
- **laws**: Registra na MC-03 a exceção camelCase da API QML ([`37a3f50`](#37a3f503e0e6732a69c4101182d0523ab2eb34b2))

### Testes

- Separa testes dependentes do sistema real e remove placeholders ([`425af5c`](#425af5c696a55e914ec1fc369c065de0f8a9132c))

### Manutenção

- **release**: Remove coautoria fixa do commit de release ([`6ded36e`](#6ded36e7585c65da58cb1c31190adce0c484e646))
- **release**: Aponta deploy/release/pre-push para GitHub e main ([`ec543ca`](#ec543ca0ef115d1f3c749843414bd99874ec707c))

### Build

- Corrige gate de clang-tidy e cppcheck para reprovar só por bugs ([`cbdaee1`](#cbdaee19546e8f90d7305860e09a4f7d9d1481f9))
- Estágio clang-tidy reprova só pelas famílias do gate T2 ([`8fce8e5`](#8fce8e5592349990e78111f0de292d70fc3b571c))
- Faz o gate T4 reprovar por ERROR SUMMARY e usa c++23 no cppcheck ([`9abbbed`](#9abbbedb67b518c81749ebee405e14e625399e96))
- **cmake**: Compila o núcleo uma vez e não depende de download do GTest ([`bef5123`](#bef5123cb4a859ba3ba5e34e4258c14f62ed5a22))

### Outros

- Release.sh exige git-cliff >= 2.14 (petrinhu/my_comp#2) ([`3494053`](#3494053f63450a410cb2e4493f79f3d7a2a69453))
## [0.1.0-alpha.5] - 2026-11-26

### Documentação

- **leis**: GODS_LAWS do projeto e CLAUDE.md enxuto ([`3f287df`](#3f287df17e9938dc777fe74d72abe40200da1891))
- **todo**: Alinha o H1 ao modelo canonico (`# TODO - <projeto>`) ([`7a6a735`](#7a6a7352c945ec44f5e6274c533147bdefed80f9))
- **todo**: Normaliza TODO.md para a forma canonica (uma tabela, 9 colunas) ([`4f99abb`](#4f99abb3a4bd39d526be966d0761363ab8b663fc))

### Manutenção

- Migrate license to AGPL-3.0 ([`6a55cb8`](#6a55cb8c9a61a25375322d630063d500b33bcef0))
## [0.1.0-alpha.4] - 2026-04-30

### Adicionado

- **vram**: VRAM-01..05 — auditoria de uso de VRAM (domain+app+infra+QML+CLI) ([`93dbc0a`](#93dbc0ac2f52eed5bb7b1f2b4c18ed3a25d54613))
## [0.1.0-alpha.3] - 2026-04-30

### Corrigido

- **deploy**: Substitui deploy.sh antigo pelo wrapper de release semântica ([`83a5858`](#83a5858da105adc52446eaf351f4cbdd1ac75dfd))
## [0.1.0-alpha.2] - 2026-04-30

### Documentação

- **release**: README e §8 do CLAUDE refletem esquema git-cliff ([`e45f82c`](#e45f82c3b974477f49ba137d5e0c9f687bba042b))
## [0.1.0-alpha.1] - 2026-04-30

### Adicionado

- **vram**: Unifica feature vram_optimizer no my_comp + bump C++23 (Fase B) ([`d8e1b2d`](#d8e1b2ddc288c83c2565490c13354b1f0f166936))

### Documentação

- **todo**: Re-categoriza coluna Grupo→Tipo com 8 ícones por natureza ([`e081f2b`](#e081f2bb67b76fe0464133b613114c286b86c194))
- **claude**: Adiciona §0.1 — quando consultar os docs normativos ([`4cbe911`](#4cbe9112936c63f4ef5afdde82d88575021e337a))
- **normas**: Incorpora CONTRACT/TESTES/AUDITORIAS + auditoria de gaps ([`48c8e95`](#48c8e959e8fbd6b1ca6b72bf91ff22bdc92bcbcf))

### Manutenção

- **vram**: Integra feature vram_optimizer (Fase A — reconciliação estrutural) ([`2b42c39`](#2b42c398280ab8856f1c52b80fc867b6a065829c))
<!-- git-cliff prepende releases automaticamente acima desta linha. Não editar manualmente. -->
