#include "qsettings_preferences_store.h"

#include <QSettings>

namespace my_comp::infrastructure::persistence {

QVariant QSettingsPreferencesStore::value(const QString& key, const QVariant& fallback) const {
    return QSettings().value(key, fallback);
}

void QSettingsPreferencesStore::set_value(const QString& key, const QVariant& value) {
    QSettings settings;
    settings.setValue(key, value);
    settings.sync();
}

QList<QVariantMap> QSettingsPreferencesStore::read_list(const QString& key) const {
    QSettings settings;
    QList<QVariantMap> items;
    const int size = settings.beginReadArray(key);
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QVariantMap item;
        for (const QString& field : settings.childKeys()) {
            item.insert(field, settings.value(field));
        }
        items.append(item);
    }
    settings.endArray();
    return items;
}

void QSettingsPreferencesStore::write_list(const QString& key, const QList<QVariantMap>& items) {
    QSettings settings;
    settings.remove(key);
    settings.beginWriteArray(key, static_cast<int>(items.size()));
    for (int i = 0; i < items.size(); ++i) {
        settings.setArrayIndex(i);
        for (auto it = items[i].cbegin(); it != items[i].cend(); ++it) {
            settings.setValue(it.key(), it.value());
        }
    }
    settings.endArray();
    settings.sync();
}

void QSettingsPreferencesStore::remove(const QString& key) {
    QSettings settings;
    settings.remove(key);
    settings.sync();
}

} // namespace my_comp::infrastructure::persistence
