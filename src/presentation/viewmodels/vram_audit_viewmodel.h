#pragma once

#include "application/vram/auditoria/relatorio_auditoria_vram.h"
#include "application/vram/auditoria/servico_auditar_vram_atual.h"

#include <QObject>
#include <QVariantList>

#include <atomic>
#include <memory>

namespace vramopt::presentation::viewmodels {

class VramAuditViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList grupos READ grupos NOTIFY gruposChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit VramAuditViewModel(
        std::shared_ptr<application::ServicoAuditarVramAtual> servico,
        QObject* parent = nullptr);
    ~VramAuditViewModel() override = default;

    VramAuditViewModel(const VramAuditViewModel&) = delete;
    VramAuditViewModel(VramAuditViewModel&&) = delete;
    VramAuditViewModel& operator=(const VramAuditViewModel&) = delete;
    VramAuditViewModel& operator=(VramAuditViewModel&&) = delete;

    [[nodiscard]] QVariantList grupos() const { return m_grupos; }
    [[nodiscard]] bool isBusy() const { return m_busy.load(); }

    Q_INVOKABLE void refresh();

    void aplicarRelatorio(const application::RelatorioAuditoriaVram& relatorio);

signals:
    void gruposChanged();
    void isBusyChanged();

private:
    std::shared_ptr<application::ServicoAuditarVramAtual> m_servico;
    QVariantList                                          m_grupos;
    std::atomic<bool>                                     m_busy{false};
};

}
