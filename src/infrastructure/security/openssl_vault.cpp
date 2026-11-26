#include "openssl_vault.h"
#include <stdexcept>
#include <memory>
#include <openssl/crypto.h>

namespace my_comp::infrastructure::security {

namespace {
// RAII: zera o buffer sensível (chave ou plaintext) ao sair do escopo.
struct KeyCleanser {
    explicit KeyCleanser(std::vector<uint8_t>& buffer) : k(buffer) {}
    std::vector<uint8_t>& k;
    ~KeyCleanser() { OPENSSL_cleanse(k.data(), k.size()); }
    KeyCleanser(const KeyCleanser&) = delete;
    KeyCleanser& operator=(const KeyCleanser&) = delete;
};
}

domain::models::VaultData OpenSslVault::encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key) {
    if (key.size() != AES_KEY_SIZE) {
        throw std::invalid_argument("Chave deve ter 256 bits (32 bytes)");
    }

    domain::models::VaultData data;
    data.iv.resize(GCM_IV_SIZE);
    if (RAND_bytes(data.iv.data(), static_cast<int>(GCM_IV_SIZE)) != 1) {
        throw std::runtime_error("Falha ao gerar IV aleatório (RAND_bytes)");
    }

    std::vector<uint8_t> key_local(key.begin(), key.end());
    KeyCleanser cleanser{key_local};

    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!ctx) throw std::runtime_error("Falha ao criar OpenSSL CTX");

    if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        throw std::runtime_error("Falha ao inicializar GCM");

    if (EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr, key_local.data(), data.iv.data()) != 1)
        throw std::runtime_error("Falha ao configurar chave/IV");

    data.ciphertext.resize(plaintext.size());
    int out_len = 0;
    if (EVP_EncryptUpdate(ctx.get(), data.ciphertext.data(), &out_len, plaintext.data(), static_cast<int>(plaintext.size())) != 1)
        throw std::runtime_error("Falha durante criptografia");

    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx.get(), data.ciphertext.data() + out_len, &final_len) != 1)
        throw std::runtime_error("Falha ao finalizar criptografia");

    data.tag.resize(GCM_TAG_SIZE);
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, static_cast<int>(GCM_TAG_SIZE), data.tag.data()) != 1)
        throw std::runtime_error("Falha ao obter tag GCM");

    return data;
}

std::vector<uint8_t> OpenSslVault::decrypt(const domain::models::VaultData& data, const std::vector<uint8_t>& key) {
    if (key.size() != AES_KEY_SIZE) return {};
    // VaultData vem de armazenamento (entrada externa): o OpenSSL lê exatamente
    // GCM_IV_SIZE/GCM_TAG_SIZE bytes, então tamanhos diferentes = over-read ou tag truncada.
    if (data.iv.size() != GCM_IV_SIZE || data.tag.size() != GCM_TAG_SIZE) return {};

    std::vector<uint8_t> key_local(key.begin(), key.end());
    KeyCleanser cleanser{key_local};

    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if (!ctx) return {};

    if (EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        return {};

    if (EVP_DecryptInit_ex(ctx.get(), nullptr, nullptr, key_local.data(), data.iv.data()) != 1)
        return {};

    std::vector<uint8_t> plaintext(data.ciphertext.size());
    KeyCleanser plaintext_cleanser{plaintext}; // zera o buffer em qualquer saída
    int out_len = 0;
    if (EVP_DecryptUpdate(ctx.get(), plaintext.data(), &out_len, data.ciphertext.data(), static_cast<int>(data.ciphertext.size())) != 1)
        return {};

    std::vector<uint8_t> tag_local(data.tag.begin(), data.tag.end());
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_TAG, static_cast<int>(GCM_TAG_SIZE), tag_local.data()) != 1)
        return {};

    int final_len = 0;
    int ret = EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + out_len, &final_len);

    if (ret <= 0) {
        return {}; // tag inválida: plaintext parcial é descartado e zerado pelo cleanser
    }
    // Copia antes do cleanser zerar o buffer de trabalho.
    return {plaintext.begin(), plaintext.begin() + out_len + final_len};
}

} // namespace my_comp::infrastructure::security
