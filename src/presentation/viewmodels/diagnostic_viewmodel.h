#ifndef DIAGNOSTIC_VIEWMODEL_H
#define DIAGNOSTIC_VIEWMODEL_H

#include <QObject>
#include <QDebug>
#include <memory>
#include "../../domain/diagnostic_manager.h"
#include "log_entry_model.h"

namespace my_comp::presentation::viewmodels {

class DiagnosticViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(LogEntryModel* logModel READ logModel CONSTANT)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit DiagnosticViewModel(std::shared_ptr<my_comp::domain::DiagnosticManager> manager, QObject* parent = nullptr)
        : QObject(parent), manager_(std::move(manager)), model_(new LogEntryModel(this)) {
        qDebug() << "[DiagnosticVM] Initialized.";
    }

    [[nodiscard]] LogEntryModel* logModel() const { return model_; }
    [[nodiscard]] bool isBusy() const { return is_busy_; }

    Q_INVOKABLE void refresh(int filterIndex = 0) {
        if (is_busy_) return;
        qDebug() << "[DiagnosticVM] Fetching logs with filter index:" << filterIndex;
        is_busy_ = true;
        emit isBusyChanged();

        auto filter = static_cast<my_comp::domain::DiagnosticFilter>(filterIndex);
        auto logs = manager_->fetch_all_logs(filter);
        qDebug() << "[DiagnosticVM] Found" << logs.size() << "logs.";
        model_->setLogs(std::move(logs));

        is_busy_ = false;
        emit isBusyChanged();
    }

signals:
    void isBusyChanged();

private:
    std::shared_ptr<my_comp::domain::DiagnosticManager> manager_;
    LogEntryModel* model_;
    bool is_busy_{false};
};

} // namespace my_comp::presentation::viewmodels

#endif // DIAGNOSTIC_VIEWMODEL_H
