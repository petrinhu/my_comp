#pragma once

#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/auditoria/servico_auditar_vram_atual.h"
#include "application/vram/feedback/servico_registrar_feedback.h"

#include <QObject>
#include <QVariantList>

#include <atomic>
#include <memory>

namespace vramopt::presentation::viewmodels {

class VramAuditViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList grupos READ grupos NOTIFY gruposChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(bool feedbackDisponivel READ feedbackDisponivel CONSTANT)

public:
    explicit VramAuditViewModel(
        std::shared_ptr<application::ServicoAuditarVramAtual> servico,
        std::shared_ptr<application::ServicoRegistrarFeedback> feedback = nullptr,
        QObject* parent = nullptr);
    ~VramAuditViewModel() override = default;

    VramAuditViewModel(const VramAuditViewModel&) = delete;
    VramAuditViewModel(VramAuditViewModel&&) = delete;
    VramAuditViewModel& operator=(const VramAuditViewModel&) = delete;
    VramAuditViewModel& operator=(VramAuditViewModel&&) = delete;

    [[nodiscard]] QVariantList grupos() const { return m_grupos; }
    [[nodiscard]] bool isBusy() const { return m_busy.load(); }
    [[nodiscard]] bool feedbackDisponivel() const { return m_feedback != nullptr; }

    Q_INVOKABLE void refresh();

    // Fixa o veredito do processo pela chave de binário (VRAM-07). Entrada vinda do QML
    // é validada aqui; a gravação roda fora da thread de UI e termina em
    // feedbackRegistrado(). Com sucesso, dispara nova auditoria.
    Q_INVOKABLE void registrarFeedback(const QString& chaveBinario, const QString& categoria,
                                       const QString& motivo);

    void aplicarRelatorio(const application::RelatorioAuditoriaVram& relatorio);

signals:
    void gruposChanged();
    void isBusyChanged();
    void feedbackRegistrado(bool sucesso);

private:
    std::shared_ptr<application::ServicoAuditarVramAtual> m_servico;
    std::shared_ptr<application::ServicoRegistrarFeedback> m_feedback;
    QVariantList                                          m_grupos;
    std::atomic<bool>                                     m_busy{false};
};

}
