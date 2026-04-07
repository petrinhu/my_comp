#include <gtest/gtest.h>
#include "../../src/domain/interfaces/i_security_vault.h"
#include "../../src/infrastructure/security/openssl_vault.h"
#include <string>
#include <vector>

using namespace my_comp::domain::interfaces;
using namespace my_comp::domain::models;
using namespace my_comp::infrastructure::security;

TEST(SecurityVaultAcceptance, ShouldEncryptAndDecryptDataReal) {
    OpenSslVault vault;
    
    // 1. O segredo que queremos proteger
    std::string secret = "My Comp System Secret Password 123!";
    std::vector<uint8_t> plaintext(secret.begin(), secret.end());
    
    // 2. Uma chave de 32 bytes (256 bits)
    std::vector<uint8_t> key = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    // 3. Tenta trancar (criptografar)
    VaultData encrypted = vault.encrypt(plaintext, key);
    
    // O texto criptografado deve ser diferente do original
    EXPECT_NE(encrypted.ciphertext, plaintext);
    EXPECT_EQ(encrypted.iv.size(), 12); // Padrão GCM
    EXPECT_EQ(encrypted.tag.size(), 16); // Padrão GCM

    // 4. Tenta abrir (descriptografar)
    std::vector<uint8_t> decrypted = vault.decrypt(encrypted, key);
    
    // 5. O resultado final deve ser igual ao segredo original
    std::string result(decrypted.begin(), decrypted.end());
    EXPECT_EQ(result, secret);
}

TEST(SecurityVaultAcceptance, ShouldFailIfDataIsTampered) {
    OpenSslVault vault;
    std::string secret = "Very important data";
    std::vector<uint8_t> plaintext(secret.begin(), secret.end());
    std::vector<uint8_t> key(32, 0xAA);

    VaultData data = vault.encrypt(plaintext, key);

    // Simula um ataque: altera um byte do texto criptografado
    data.ciphertext[0] ^= 0xFF;

    // O cofre deve perceber a violação e retornar vazio
    std::vector<uint8_t> result = vault.decrypt(data, key);
    EXPECT_TRUE(result.empty());
}
