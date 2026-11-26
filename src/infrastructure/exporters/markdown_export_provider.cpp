#include "markdown_export_provider.h"
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QDebug>

namespace my_comp::infrastructure::exporters {

namespace {

// Escape CommonMark especiais para evitar que valores com `_`, `*`, `|`, `<`, `>`,
// backtick ou backslash virem itálico/código/HTML ou quebrem tabelas/estrutura.
// Newlines viram espaço para manter cada valor numa linha só.
QString md_escape(const QString& raw) {
    QString out;
    out.reserve(raw.size());
    for (const QChar c : raw) {
        if (c == QLatin1Char('\n') || c == QLatin1Char('\r')) {
            out += QLatin1Char(' ');
        } else if (c == QLatin1Char('\\') || c == QLatin1Char('`') ||
                   c == QLatin1Char('*') || c == QLatin1Char('_') ||
                   c == QLatin1Char('{') || c == QLatin1Char('}') ||
                   c == QLatin1Char('[') || c == QLatin1Char(']') ||
                   c == QLatin1Char('(') || c == QLatin1Char(')') ||
                   c == QLatin1Char('#') || c == QLatin1Char('+') ||
                   c == QLatin1Char('-') || c == QLatin1Char('!') ||
                   c == QLatin1Char('|') || c == QLatin1Char('<') ||
                   c == QLatin1Char('>')) {
            out += QLatin1Char('\\');
            out += c;
        } else {
            out += c;
        }
    }
    return out;
}

} // namespace

bool MarkdownExportProvider::export_report(const domain::models::ReportData& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[MarkdownExporter] Failed to open file:" << path;
        return false;
    }

    QTextStream out(&file);
    out << "# " << md_escape(data.title) << "\n\n";
    out << "- **Gerado em:** " << md_escape(data.timestamp) << "\n";
    out << "- **Sistema:** " << md_escape(data.distro) << " (" << md_escape(data.kernel) << ")\n";
    out << "- **Usuário:** " << md_escape(data.user) << "\n\n";

    for (auto it = data.sections.begin(); it != data.sections.end(); ++it) {
        out << "## " << md_escape(it.key()) << "\n\n";
        out << format_variant(it.value(), 0);
        out << "\n";
    }

    file.close();
    return true;
}

QString MarkdownExportProvider::format_variant(const QVariant& v, int level) {
    QString result;
    const QString indent = QString(static_cast<qsizetype>(level) * 2, QLatin1Char(' '));

    if (v.typeId() == QMetaType::QVariantList) {
        auto list = v.toList();
        for (const auto& item : list) {
            if (item.typeId() == QMetaType::QVariantMap) {
                result += format_variant(item, level + 1);
                result += "\n";
            } else {
                result += indent + "- " + md_escape(item.toString()) + "\n";
            }
        }
    } else if (v.typeId() == QMetaType::QVariantMap) {
        auto map = v.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it.value().typeId() == QMetaType::QVariantMap || it.value().typeId() == QMetaType::QVariantList) {
                result += indent + "**" + md_escape(it.key()) + ":**\n";
                result += format_variant(it.value(), level + 1);
            } else {
                result += indent + "- **" + md_escape(it.key()) + ":** " + md_escape(it.value().toString()) + "\n";
            }
        }
    } else {
        result = indent + md_escape(v.toString()) + "\n";
    }

    return result;
}

} // namespace my_comp::infrastructure::exporters
