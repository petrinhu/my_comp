#pragma once
#include <vector>
#include <string>
#include "../models/vault_data.h"

namespace my_comp::domain::interfaces {

class ISecurityVault {
public:
    virtual ~ISecurityVault() = default;

    /**
     * @brief Criptografa dados em texto claro.
     * @param plaintext Dados a serem protegidos.
     * @param key Chave de criptografia (deve ter 32 bytes para AES-256).
     * @return VaultData contendo ciphertext, iv e auth tag.
     */
    virtual models::VaultData encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& key) = 0;

    /**
     * @brief Descriptografa dados e valida integridade via GCM tag.
     * @param data VaultData estruturado.
     * @param key Chave de descriptografia.
     * @return Vetor com texto claro, ou vazio em caso de falha de integridade.
     */
    virtual std::vector<uint8_t> decrypt(const models::VaultData& data, const std::vector<uint8_t>& key) = 0;
};

} // namespace my_comp::domain::interfaces
