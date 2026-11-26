#pragma once
#include "../../../src/domain/interfaces/i_preferences_store.h"
#include <QHash>

namespace my_comp::tests {

// Dublê em memória: testes de ViewModel sem tocar no QSettings do usuário.
class InMemoryPreferencesStore : public domain::interfaces::IPreferencesStore {
public:
    QVariant value(const QString& key, const QVariant& fallback = {}) const override {
        return m_values.value(key, fallback);
    }
    void set_value(const QString& key, const QVariant& value) override { m_values.insert(key, value); }
    QList<QVariantMap> read_list(const QString& key) const override { return m_lists.value(key); }
    void write_list(const QString& key, const QList<QVariantMap>& items) override { m_lists.insert(key, items); }
    void remove(const QString& key) override { m_values.remove(key); m_lists.remove(key); }

private:
    QHash<QString, QVariant> m_values;
    QHash<QString, QList<QVariantMap>> m_lists;
};

} // namespace my_comp::tests
