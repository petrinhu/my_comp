#pragma once

// Repositório do feedback do usuário (VRAM-07), persistido cifrado via ISecurityVault
// (MC-10: AES-256-GCM, IV aleatório por gravação, tag de autenticação).
//
// Arquivos em `diretorio` (a composição usa AppDataLocation/vram):
//   vram_feedback.bin  cabeçalho "MCFB" + versão (1 byte) + IV (12) + tag (16) + ciphertext
//   vram_feedback.key  chave de 32 bytes, ver ArquivoChaveLocal (modo 0600)
// O plaintext é JSON {versao, proximo_id, entradas[]}; mesmo decifrado com tag válida,
// cada entrada é revalidada (arquivo em disco é entrada externa).
//
// Sem I/O no construtor (MC-04): a carga é preguiçosa, no primeiro acesso, que costuma
// ocorrer na thread de trabalho da auditoria. A chave só é criada na primeira gravação.
// Se há dados que não se consegue ler (tag inválida, chave ausente ou recusada), o
// repositório segue só em memória e não sobrescreve o arquivo, para não destruir o que
// talvez ainda seja recuperável. Thread-safe (mutex interno).

#include "domain/interfaces/i_security_vault.h"
#include "domain/vram/interfaces/i_repositorio_feedback.h"

#include <QByteArray>
#include <QString>

#include <memory>
#include <mutex>

namespace vramopt::infrastructure {

class RepositorioFeedbackCifrado final : public domain::IRepositorioFeedback {
public:
    RepositorioFeedbackCifrado(std::shared_ptr<my_comp::domain::interfaces::ISecurityVault> vault,
                               QString diretorio);

    [[nodiscard]] static QByteArray cabecalho();

    [[nodiscard]] std::optional<domain::EntradaFeedback>
        buscar(const domain::ChaveProcesso& chave) const override;

    // id <= 0 recebe o próximo id livre; feedback para uma chave já marcada substitui o anterior.
    void inserir(const domain::EntradaFeedback& entrada) override;
    void atualizarVistoEm(int id, QDateTime quando) override;
    void remover(int id) override;

    [[nodiscard]] QVector<domain::EntradaFeedback> listarTodos() const override;

private:
    void garantir_carregado() const;   // exige m_mutex travado
    void carregar() const;             // exige m_mutex travado
    void persistir();                  // exige m_mutex travado

    [[nodiscard]] QString caminho_dados() const;
    [[nodiscard]] QString caminho_chave() const;

    std::shared_ptr<my_comp::domain::interfaces::ISecurityVault> m_vault;
    QString                                                      m_diretorio;

    mutable std::mutex                       m_mutex;
    mutable bool                             m_carregado{false};
    mutable bool                             m_somente_memoria{false};
    mutable int                              m_proximo_id{1};
    mutable QVector<domain::EntradaFeedback> m_entradas;
};

}
