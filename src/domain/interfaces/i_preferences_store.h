#pragma once
#include <QList>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace my_comp::domain::interfaces {

// Preferências e históricos pequenos do usuário (não sensíveis; dado sensível vai pelo
// ISecurityVault, MC-10). Porta para tirar QSettings dos ViewModels (MC-02, QA-07).
class IPreferencesStore {
public:
    virtual ~IPreferencesStore() = default;

    [[nodiscard]] virtual QVariant value(const QString& key, const QVariant& fallback = {}) const = 0;
    virtual void set_value(const QString& key, const QVariant& value) = 0;

    // Lista de registros (ex.: histórico de exportações), na ordem em que foi gravada.
    [[nodiscard]] virtual QList<QVariantMap> read_list(const QString& key) const = 0;
    virtual void write_list(const QString& key, const QList<QVariantMap>& items) = 0;

    virtual void remove(const QString& key) = 0;
};

} // namespace my_comp::domain::interfaces
