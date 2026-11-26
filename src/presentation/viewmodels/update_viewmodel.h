#ifndef UPDATE_VIEWMODEL_H
#define UPDATE_VIEWMODEL_H

#include <QObject>
#include <QDebug>
#include <QPointer>
#include <QtConcurrent>
#include <algorithm>
#include <memory>
#include "../../application/update_manager.h"
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
    explicit UpdateViewModel(std::shared_ptr<my_comp::application::UpdateManager> manager, QObject* parent = nullptr)
        : QObject(parent), m_manager(std::move(manager)), m_model(new UpdatePackageModel(this)) {
        qDebug() << "[UpdateVM] Initialized.";
    }

    [[nodiscard]] UpdatePackageModel* updateModel() const { return m_model; }
    [[nodiscard]] int totalUpdates() const { return m_model->rowCount(); }
    [[nodiscard]] bool isBusy() const { return m_is_busy; }
    [[nodiscard]] QString sortField() const { return m_sort_field; }
    [[nodiscard]] bool sortAscending() const { return m_sort_ascending; }

    void setSortField(const QString& f) {
        if (m_sort_field != f) {
            m_sort_field = f;
            emit sortFieldChanged();
            applySortAndPush();
        }
    }
    void setSortAscending(bool a) {
        if (m_sort_ascending != a) {
            m_sort_ascending = a;
            emit sortAscendingChanged();
            applySortAndPush();
        }
    }

    Q_INVOKABLE void refresh() {
        if (m_is_busy) return;
        qDebug() << "[UpdateVM] Starting update check...";

        m_is_busy = true;
        emit isBusyChanged();

        QPointer<UpdateViewModel> self(this);
        auto manager = m_manager;
        (void)QtConcurrent::run([self, manager]() {
            auto updates = manager->check_all_updates();

            QMetaObject::invokeMethod(self.data(), [self, updates]() mutable {
                auto* vm = self.data();
                if (!vm) return;
                qDebug() << "[UpdateVM] Found" << updates.size() << "updates.";
                vm->m_data = std::move(updates);
                vm->applySortAndPush();

                vm->m_is_busy = false;
                emit vm->isBusyChanged();
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
        auto sorted = m_data;
        const QString field = m_sort_field;
        const bool asc = m_sort_ascending;
        std::stable_sort(sorted.begin(), sorted.end(), [field, asc](const auto& a, const auto& b) {
            if (field == "source") return asc ? (a.source < b.source) : (a.source > b.source);
            if (field == "newVersion") return asc ? (a.new_version < b.new_version) : (a.new_version > b.new_version);
            return asc ? (a.name < b.name) : (a.name > b.name);
        });
        m_model->setUpdates(std::move(sorted));
        emit updatesChanged();
    }

    std::shared_ptr<my_comp::application::UpdateManager> m_manager;
    UpdatePackageModel* m_model;
    std::vector<my_comp::domain::models::UpdatePackage> m_data;
    bool m_is_busy{false};
    QString m_sort_field{"name"};
    bool m_sort_ascending{true};
};

} // namespace my_comp::presentation::viewmodels

#endif // UPDATE_VIEWMODEL_H
