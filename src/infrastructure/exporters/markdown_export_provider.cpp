#include "markdown_export_provider.h"
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QDebug>

namespace my_comp::infrastructure::exporters {

bool MarkdownExportProvider::export_report(const domain::models::ReportData& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[MarkdownExporter] Failed to open file:" << path;
        return false;
    }

    QTextStream out(&file);
    out << "# " << data.title << "\n\n";
    out << "- **Gerado em:** " << data.timestamp << "\n";
    out << "- **Sistema:** " << data.distro << " (" << data.kernel << ")\n";
    out << "- **Usuário:** " << data.user << "\n\n";

    for (auto it = data.sections.begin(); it != data.sections.end(); ++it) {
        out << "## " << it.key() << "\n\n";
        out << format_variant(it.value(), 0);
        out << "\n";
    }

    file.close();
    return true;
}

QString MarkdownExportProvider::format_variant(const QVariant& v, int level) {
    QString result;
    QString indent = QString(level * 2, ' ');

    if (v.typeId() == QMetaType::QVariantList) {
        auto list = v.toList();
        for (const auto& item : list) {
            if (item.typeId() == QMetaType::QVariantMap) {
                result += format_variant(item, level + 1);
                result += "\n";
            } else {
                result += indent + "- " + item.toString() + "\n";
            }
        }
    } else if (v.typeId() == QMetaType::QVariantMap) {
        auto map = v.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it.value().typeId() == QMetaType::QVariantMap || it.value().typeId() == QMetaType::QVariantList) {
                result += indent + "**" + it.key() + ":**\n";
                result += format_variant(it.value(), level + 1);
            } else {
                result += indent + "- **" + it.key() + ":** " + it.value().toString() + "\n";
            }
        }
    } else {
        result = indent + v.toString() + "\n";
    }

    return result;
}

} // namespace my_comp::infrastructure::exporters
