#pragma once

// Chave simétrica local de 32 bytes (AES-256) guardada em arquivo do próprio usuário.
//
// Origem da chave (MC-10: nada de segredo hardcoded): 32 bytes do CSPRNG do OpenSSL
// (RAND_bytes), gerados no primeiro uso que precisa gravar e salvos com O_CREAT|O_EXCL|
// O_NOFOLLOW e modo 0600. Na leitura, recusa (sem sobrescrever) arquivo que não seja
// regular, que seja link simbólico, que não tenha exatamente 32 bytes ou que seja
// legível/gravável por grupo ou outros (mesma postura do ssh com chaves privadas).
//
// Modelo de ameaça: protege o arquivo de dados que vaze sozinho (backup, sync, anexo
// de bug report) e o separa de outros usuários locais. NÃO protege contra código que
// rode como o próprio usuário, que lê a chave tanto quanto os dados. O endurecimento
// natural é guardar a chave no Secret Service (KWallet/GNOME Keyring via DBus).

#include <QString>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace my_comp::infrastructure::security {

class ArquivoChaveLocal {
public:
    static constexpr std::size_t TAMANHO_CHAVE = 32;

    explicit ArquivoChaveLocal(QString caminho);

    [[nodiscard]] bool existe() const;

    // Chave existente e válida; nullopt se ausente ou recusada.
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> ler() const;

    // Como ler(); se o arquivo não existe, gera e grava uma chave nova. Um arquivo
    // presente mas recusado devolve nullopt (trocar a chave perderia os dados cifrados).
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> ler_ou_criar() const;

private:
    QString m_caminho;
};

}
