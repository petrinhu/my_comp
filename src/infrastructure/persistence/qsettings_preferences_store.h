#pragma once
#include "../../domain/interfaces/i_preferences_store.h"

namespace my_comp::infrastructure::persistence {

// IPreferencesStore sobre QSettings (organização/aplicação definidas no main).
// Listas usam beginReadArray/beginWriteArray, o mesmo formato que o ExportViewModel já
// gravava; históricos existentes continuam legíveis.
class QSettingsPreferencesStore : public domain::interfaces::IPreferencesStore {
public:
    [[nodiscard]] QVariant value(const QString& key, const QVariant& fallback = {}) const override;
    void set_value(const QString& key, const QVariant& value) override;
    [[nodiscard]] QList<QVariantMap> read_list(const QString& key) const override;
    void write_list(const QString& key, const QList<QVariantMap>& items) override;
    void remove(const QString& key) override;
};

} // namespace my_comp::infrastructure::persistence
