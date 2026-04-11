#include "json_export_provider.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

namespace my_comp::infrastructure::exporters {

bool JsonExportProvider::export_report(const domain::models::ReportData& data, const QString& path) {
    QJsonObject root;
    root["title"] = data.title;
    root["timestamp"] = data.timestamp;
    root["distro"] = data.distro;
    root["kernel"] = data.kernel;
    root["user"] = data.user;
    root["data"] = QJsonObject::fromVariantMap(data.sections);

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "[JSONExporter] Failed to open file for writing:" << path;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

} // namespace my_comp::infrastructure::exporters
