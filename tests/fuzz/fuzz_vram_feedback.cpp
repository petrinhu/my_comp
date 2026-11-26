// AUDIT-33 — libFuzzer harness para a leitura do vram_feedback.bin.
// A tag GCM barra qualquer byte alterado antes do JSON, então o harness usa um vault de
// identidade (o "decifrado" é o próprio ciphertext): assim o fuzzer exercita o cabeçalho
// MCFB, os tamanhos de IV/tag e a revalidação de cada entrada, que é o código que lida
// com conteúdo vindo do disco (MC-10).

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include "domain/interfaces/i_security_vault.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"
#include "infrastructure/security/arquivo_chave_local.h"

namespace {

class VaultIdentidade final : public my_comp::domain::interfaces::ISecurityVault {
public:
    my_comp::domain::models::VaultData encrypt(const std::vector<uint8_t>& plaintext,
                                               const std::vector<uint8_t>&) override {
        return {.ciphertext = plaintext, .iv = std::vector<uint8_t>(12), .tag = std::vector<uint8_t>(16)};
    }
    std::vector<uint8_t> decrypt(const my_comp::domain::models::VaultData& data,
                                 const std::vector<uint8_t>&) override {
        return data.ciphertext;
    }
};

QTemporaryDir* diretorio() {
    static QTemporaryDir dir;
    return &dir;
}

}  // namespace

extern "C" int LLVMFuzzerInitialize(int*, char***) {
    // A chave precisa existir (0600) para o repositório chegar ao "decrypt".
    const QString caminho = diretorio()->filePath(QStringLiteral("vram_feedback.key"));
    (void)my_comp::infrastructure::security::ArquivoChaveLocal{caminho}.ler_ou_criar();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 64 * 1024) {
        return 0;
    }
    // Prefixa o cabeçalho válido em metade dos casos para o fuzzer passar da 1ª barreira.
    QByteArray conteudo;
    const bool com_cabecalho = size > 0 && (data[0] & 1U) != 0U;
    if (com_cabecalho) {
        conteudo = my_comp::infrastructure::vram::RepositorioFeedbackCifrado::cabecalho()
                 + QByteArray(28, '\0');  // IV (12) + tag (16)
        conteudo.append(reinterpret_cast<const char*>(data + 1), static_cast<qsizetype>(size - 1));
    } else {
        conteudo = QByteArray(reinterpret_cast<const char*>(data), static_cast<qsizetype>(size));
    }

    QFile f{diretorio()->filePath(QStringLiteral("vram_feedback.bin"))};
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return 0;
    }
    f.write(conteudo);
    f.close();

    my_comp::infrastructure::vram::RepositorioFeedbackCifrado repo{
        std::make_shared<VaultIdentidade>(), diretorio()->path()};
    const auto entradas = repo.listar_todos();
    for (const auto& e : entradas) {
        (void)repo.buscar(e.chave);
    }
    return 0;
}
