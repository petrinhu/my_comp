#ifndef UPDATE_PACKAGE_MODEL_H
#define UPDATE_PACKAGE_MODEL_H

#include <QAbstractListModel>
#include <vector>
#include "../../domain/models/update_package.h"

namespace my_comp::presentation::viewmodels {

using UpdatePackage = my_comp::domain::models::UpdatePackage;

class UpdatePackageModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum UpdateRoles {
        NameRole = Qt::UserRole + 1,
        CurrentVersionRole,
        NewVersionRole,
        SourceRole,
        DescriptionRole,
        IsCriticalRole
    };

    explicit UpdatePackageModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

    void setUpdates(std::vector<UpdatePackage> updates) {
        beginResetModel();
        updates_ = std::move(updates);
        endResetModel();
    }

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return static_cast<int>(updates_.size());
    }

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || index.row() >= static_cast<int>(updates_.size()))
            return {};

        const auto& update = updates_[static_cast<size_t>(index.row())];
        switch (role) {
            case NameRole: return update.name;
            case CurrentVersionRole: return update.current_version;
            case NewVersionRole: return update.new_version;
            case SourceRole: return update.source;
            case DescriptionRole: return update.description;
            case IsCriticalRole: return update.is_critical;
            default: return {};
        }
    }

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[CurrentVersionRole] = "currentVersion";
        roles[NewVersionRole] = "newVersion";
        roles[SourceRole] = "source";
        roles[DescriptionRole] = "description";
        roles[IsCriticalRole] = "isCritical";
        return roles;
    }

private:
    std::vector<UpdatePackage> updates_;
};

} // namespace my_comp::presentation::viewmodels

#endif // UPDATE_PACKAGE_MODEL_H
