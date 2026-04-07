#pragma once

#include <QObject>
#include <QString>
#include "../../domain/interfaces/i_installer.h"
#include <memory>

namespace my_comp::presentation::viewmodels {

class InstallationViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logOutput READ logOutput NOTIFY logOutputChanged)
    Q_PROPERTY(bool isWaiting READ isWaiting NOTIFY interactionRequired)
    Q_PROPERTY(QString question READ question NOTIFY interactionRequired)

public:
    explicit InstallationViewModel(std::shared_ptr<domain::interfaces::IInstaller> installer, QObject* parent = nullptr);
    
    Q_INVOKABLE void startInstallation(const QString& name, int providerType);
    Q_INVOKABLE void respond(bool confirmed);

    QString logOutput() const { return m_log; }
    bool isWaiting() const { return m_isWaiting; }
    QString question() const { return m_question; }

signals:
    void logOutputChanged();
    void interactionRequired();

private slots:
    void onOutputReceived(const QString& output);
    void onInteractionRequired(const QString& question);

private:
    std::shared_ptr<domain::interfaces::IInstaller> m_installer;
    QString m_log;
    bool m_isWaiting = false;
    QString m_question;
};

} // namespace my_comp::presentation::viewmodels
