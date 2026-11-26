#pragma once
#include "../../domain/interfaces/i_preferences_store.h"

#include <QDateTime>
#include <QString>
#include <QVariantList>
#include <memory>

namespace my_comp::application {

// Histórico das exportações (SRP: saiu do ExportViewModel, QA-07). FIFO limitado.
class ExportHistory {
public:
    static constexpr int MAX_ENTRIES = 50;
    static constexpr const char* KEY = "export_history";

    explicit ExportHistory(std::shared_ptr<domain::interfaces::IPreferencesStore> store)
        : m_store(std::move(store)) {}

    // Cada entrada: {"timestamp" (ISO 8601), "format", "path"}, da mais antiga à mais recente.
    [[nodiscard]] QVariantList entries() const {
        QVariantList list;
        for (const auto& item : m_store->read_list(KEY)) {
            QVariantMap entry;
            entry.insert("timestamp", item.value("timestamp").toString());
            entry.insert("format", item.value("format").toString());
            entry.insert("path", item.value("path").toString());
            list.append(entry);
        }
        return list;
    }

    void append(const QString& format_name, const QString& file_path,
                const QDateTime& when = QDateTime::currentDateTime()) {
        auto items = m_store->read_list(KEY);
        items.append(QVariantMap{{"timestamp", when.toString(Qt::ISODate)},
                                 {"format", format_name},
                                 {"path", file_path}});
        while (items.size() > MAX_ENTRIES) items.removeFirst();
        m_store->write_list(KEY, items);
    }

    void clear() { m_store->remove(KEY); }

private:
    std::shared_ptr<domain::interfaces::IPreferencesStore> m_store;
};

} // namespace my_comp::application
