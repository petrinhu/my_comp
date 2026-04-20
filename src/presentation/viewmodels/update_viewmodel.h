#ifndef UPDATE_VIEWMODEL_H
#define UPDATE_VIEWMODEL_H

#include <QObject>
#include <QDebug>
#include <QtConcurrent>
#include <algorithm>
#include <memory>
#include "../../domain/update_manager.h"
#include "update_package_model.h"

namespace my_comp::presentation::viewmodels {

class UpdateViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(UpdatePackageModel* updateModel READ updateModel CONSTANT)
    Q_PROPERTY(int totalUpdates READ totalUpdates NOTIFY updatesChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(QString sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortAscending READ sortAscending WRITE setSortAscending NOTIFY sortAscendingChanged)

public:
    explicit UpdateViewModel(std::shared_ptr<my_comp::domain::UpdateManager> manager, QObject* parent = nullptr)
        : QObject(parent), manager_(std::move(manager)), model_(new UpdatePackageModel(this)) {
        qDebug() << "[UpdateVM] Initialized.";
    }

    [[nodiscard]] UpdatePackageModel* updateModel() const { return model_; }
    [[nodiscard]] int totalUpdates() const { return model_->rowCount(); }
    [[nodiscard]] bool isBusy() const { return is_busy_; }
    [[nodiscard]] QString sortField() const { return sort_field_; }
    [[nodiscard]] bool sortAscending() const { return sort_ascending_; }

    void setSortField(const QString& f) {
        if (sort_field_ != f) {
            sort_field_ = f;
            emit sortFieldChanged();
            applySortAndPush();
        }
    }
    void setSortAscending(bool a) {
        if (sort_ascending_ != a) {
            sort_ascending_ = a;
            emit sortAscendingChanged();
            applySortAndPush();
        }
    }

    Q_INVOKABLE void refresh() {
        if (is_busy_) return;
        qDebug() << "[UpdateVM] Starting update check...";

        is_busy_ = true;
        emit isBusyChanged();

        (void)QtConcurrent::run([this]() {
            auto updates = manager_->check_all_updates();

            QMetaObject::invokeMethod(this, [this, updates]() mutable {
                qDebug() << "[UpdateVM] Found" << updates.size() << "updates.";
                data_ = std::move(updates);
                applySortAndPush();

                is_busy_ = false;
                emit isBusyChanged();
            }, Qt::QueuedConnection);
        });
    }

signals:
    void updatesChanged();
    void isBusyChanged();
    void sortFieldChanged();
    void sortAscendingChanged();

private:
    void applySortAndPush() {
        auto sorted = data_;
        const QString field = sort_field_;
        const bool asc = sort_ascending_;
        std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
            if (field == "source") return asc ? (a.source < b.source) : (a.source > b.source);
            if (field == "newVersion") return asc ? (a.new_version < b.new_version) : (a.new_version > b.new_version);
            return asc ? (a.name < b.name) : (a.name > b.name);
        });
        model_->setUpdates(std::move(sorted));
        emit updatesChanged();
    }

    std::shared_ptr<my_comp::domain::UpdateManager> manager_;
    UpdatePackageModel* model_;
    std::vector<my_comp::domain::models::UpdatePackage> data_;
    bool is_busy_{false};
    QString sort_field_{"name"};
    bool sort_ascending_{true};
};

} // namespace my_comp::presentation::viewmodels

#endif // UPDATE_VIEWMODEL_H
