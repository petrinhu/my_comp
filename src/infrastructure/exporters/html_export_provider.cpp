#include "html_export_provider.h"
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QDebug>

namespace my_comp::infrastructure::exporters {

namespace {

QString html_escape(const QString& raw) {
    QString out;
    out.reserve(raw.size());
    for (QChar c : raw) {
        if (c == QLatin1Char('&')) out += QStringLiteral("&amp;");
        else if (c == QLatin1Char('<')) out += QStringLiteral("&lt;");
        else if (c == QLatin1Char('>')) out += QStringLiteral("&gt;");
        else if (c == QLatin1Char('"')) out += QStringLiteral("&quot;");
        else if (c == QLatin1Char('\'')) out += QStringLiteral("&#39;");
        else out += c;
    }
    return out;
}

QString escape_variant(const QVariant& v) {
    return html_escape(v.toString());
}

} // namespace

bool HtmlExportProvider::export_report(const domain::models::ReportData& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[HTMLExporter] Failed to open file:" << path;
        return false;
    }

    QTextStream out(&file);
    out << "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>" << html_escape(data.title) << "</title>";
    out << "<style>"
        << "body { font-family: 'Segoe UI', sans-serif; background: #232629; color: #eff0f1; padding: 40px; line-height: 1.6; }"
        << "h1 { color: #3daee9; border-bottom: 2px solid #3daee9; padding-bottom: 10px; }"
        << "h2 { color: #3daee9; margin-top: 40px; border-left: 5px solid #3daee9; padding-left: 15px; }"
        << ".meta { background: #31363b; padding: 20px; border-radius: 8px; margin-bottom: 30px; border: 1px solid #4d4d4d; }"
        << "table { width: 100%; border-collapse: collapse; margin: 10px 0; background: #2a2e32; }"
        << "th, td { padding: 12px; border: 1px solid #4d4d4d; text-align: left; }"
        << "th { background: #31363b; color: #3daee9; }"
        << "tr:nth-child(even) { background: #31363b; }"
        << "pre { background: #1e1e1e; padding: 10px; border-radius: 4px; overflow-x: auto; }"
        << "</style></head><body>";

    out << "<h1>" << html_escape(data.title) << "</h1>";
    out << "<div class='meta'>";
    out << "<strong>Gerado em:</strong> " << html_escape(data.timestamp) << "<br>";
    out << "<strong>Sistema:</strong> " << html_escape(data.distro) << " (" << html_escape(data.kernel) << ")<br>";
    out << "<strong>Usuário:</strong> " << html_escape(data.user);
    out << "</div>";

    for (auto it = data.sections.begin(); it != data.sections.end(); ++it) {
        out << "<h2>" << html_escape(it.key()) << "</h2>";
        out << to_html_table(it.value());
    }

    out << "</body></html>";
    file.close();
    return true;
}

QString HtmlExportProvider::to_html_table(const QVariant& v) {
    if (v.typeId() == QMetaType::QVariantList) {
        auto list = v.toList();
        if (list.isEmpty()) return "<p>Nenhum dado disponível.</p>";

        QString html = "<table>";
        bool headers_done = false;

        for (const auto& item : list) {
            if (item.typeId() == QMetaType::QVariantMap) {
                auto map = item.toMap();
                if (!headers_done) {
                    html += "<tr>";
                    for (auto it = map.begin(); it != map.end(); ++it) html += "<th>" + html_escape(it.key()) + "</th>";
                    html += "</tr>";
                    headers_done = true;
                }
                html += "<tr>";
                for (auto it = map.begin(); it != map.end(); ++it) html += "<td>" + escape_variant(it.value()) + "</td>";
                html += "</tr>";
            } else {
                html += "<tr><td>" + escape_variant(item) + "</td></tr>";
            }
        }
        html += "</table>";
        return html;
    } else if (v.typeId() == QMetaType::QVariantMap) {
        auto map = v.toMap();
        QString html = "<table>";
        for (auto it = map.begin(); it != map.end(); ++it) {
            html += "<tr><th>" + html_escape(it.key()) + "</th><td>";
            if (it.value().typeId() == QMetaType::QVariantMap || it.value().typeId() == QMetaType::QVariantList) {
                html += to_html_table(it.value());
            } else {
                html += escape_variant(it.value());
            }
            html += "</td></tr>";
        }
        html += "</table>";
        return html;
    }
    return "<p>" + escape_variant(v) + "</p>";
}

} // namespace my_comp::infrastructure::exporters
