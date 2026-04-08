#ifndef UPDATE_VIEWMODEL_H
#define UPDATE_VIEWMODEL_H

#include <QObject>
#include <QDebug>
#include <memory>
#include "../../domain/update_manager.h"
#include "update_package_model.h"

namespace my_comp::presentation::viewmodels {

class UpdateViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(UpdatePackageModel* updateModel READ updateModel CONSTANT)
    Q_PROPERTY(int totalUpdates READ totalUpdates NOTIFY updatesChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit UpdateViewModel(std::shared_ptr<my_comp::domain::UpdateManager> manager, QObject* parent = nullptr)
        : QObject(parent), manager_(std::move(manager)), model_(new UpdatePackageModel(this)) {
        qDebug() << "[UpdateVM] Initialized.";
    }

    [[nodiscard]] UpdatePackageModel* updateModel() const { return model_; }
    [[nodiscard]] int totalUpdates() const { return model_->rowCount(); }
    [[nodiscard]] bool isBusy() const { return is_busy_; }

    Q_INVOKABLE void upgradeAll() {
        qDebug() << "[UpdateVM] Requesting full upgrade...";
        manager_->upgrade_all();
    }

    Q_INVOKABLE void refresh() {
        if (is_busy_) return;
        qDebug() << "[UpdateVM] Starting update check...";

        is_busy_ = true;
        emit isBusyChanged();

        auto updates = manager_->check_all_updates();
        qDebug() << "[UpdateVM] Found" << updates.size() << "updates.";
        model_->setUpdates(std::move(updates));

        is_busy_ = false;
        emit isBusyChanged();
        emit updatesChanged();
    }

signals:
    void updatesChanged();
    void isBusyChanged();

private:
    std::shared_ptr<my_comp::domain::UpdateManager> manager_;
    UpdatePackageModel* model_;
    bool is_busy_{false};
};

} // namespace my_comp::presentation::viewmodels

#endif // UPDATE_VIEWMODEL_H
