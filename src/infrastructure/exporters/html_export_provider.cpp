#include "html_export_provider.h"
#include <QFile>
#include <QTextStream>
#include <QVariantList>
#include <QDebug>

namespace my_comp::infrastructure::exporters {

bool HtmlExportProvider::export_report(const domain::models::ReportData& data, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[HTMLExporter] Failed to open file:" << path;
        return false;
    }

    QTextStream out(&file);
    out << "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>" << data.title << "</title>";
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

    out << "<h1>" << data.title << "</h1>";
    out << "<div class='meta'>";
    out << "<strong>Gerado em:</strong> " << data.timestamp << "<br>";
    out << "<strong>Sistema:</strong> " << data.distro << " (" << data.kernel << ")<br>";
    out << "<strong>Usuário:</strong> " << data.user;
    out << "</div>";

    for (auto it = data.sections.begin(); it != data.sections.end(); ++it) {
        out << "<h2>" << it.key() << "</h2>";
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
                    for (auto it = map.begin(); it != map.end(); ++it) html += "<th>" + it.key() + "</th>";
                    html += "</tr>";
                    headers_done = true;
                }
                html += "<tr>";
                for (auto it = map.begin(); it != map.end(); ++it) html += "<td>" + it.value().toString() + "</td>";
                html += "</tr>";
            } else {
                html += "<tr><td>" + item.toString() + "</td></tr>";
            }
        }
        html += "</table>";
        return html;
    } else if (v.typeId() == QMetaType::QVariantMap) {
        auto map = v.toMap();
        QString html = "<table>";
        for (auto it = map.begin(); it != map.end(); ++it) {
            html += "<tr><th>" + it.key() + "</th><td>";
            if (it.value().typeId() == QMetaType::QVariantMap || it.value().typeId() == QMetaType::QVariantList) {
                html += to_html_table(it.value());
            } else {
                html += it.value().toString();
            }
            html += "</td></tr>";
        }
        html += "</table>";
        return html;
    }
    return "<p>" + v.toString() + "</p>";
}

} // namespace my_comp::infrastructure::exporters
