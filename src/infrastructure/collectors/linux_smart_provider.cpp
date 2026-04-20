#include "linux_smart_provider.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace my_comp::infrastructure::collectors {

domain::models::SmartReport LinuxSmartProvider::collect_report(const QString& device) {
    domain::models::SmartReport report;

    QString out;
    const QStringList args{"-A", "-H", "-j", "/dev/" + device};
    const int rc = m_shell->execute("smartctl", args, out);

    if (out.isEmpty()) {
        report.error_message = "smartctl ausente ou falha na execução";
        return report;
    }

    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(out.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        report.error_message = "JSON inválido: " + err.errorString();
        return report;
    }
    const auto root = doc.object();

    // Mensagens de erro do próprio smartctl (permission denied, device não suportado)
    if (rc != 0 && !root.contains("smart_status")) {
        const auto msgs = root.value("smartctl").toObject().value("messages").toArray();
        for (const auto& v : msgs) {
            const auto s = v.toObject().value("string").toString();
            if (!s.isEmpty()) { report.error_message = s; break; }
        }
        if (report.error_message.isEmpty()) report.error_message = "smartctl retornou código " + QString::number(rc);
        return report;
    }

    report.available = true;
    report.passed = root.value("smart_status").toObject().value("passed").toBool();
    report.temp_c = root.value("temperature").toObject().value("current").toInt();

    // power_on_hours: SATA = power_on_time.hours, NVMe idem
    report.power_on_hours = root.value("power_on_time").toObject().value("hours").toVariant().toLongLong();

    // wear_pct: NVMe expõe percentage_used diretamente
    const auto nvme_log = root.value("nvme_smart_health_information_log").toObject();
    if (!nvme_log.isEmpty()) {
        report.wear_pct = nvme_log.value("percentage_used").toInt();
        if (report.temp_c == 0) report.temp_c = nvme_log.value("temperature").toInt();
    }

    // Atributos SATA/ATA
    const auto attrs_table = root.value("ata_smart_attributes").toObject().value("table").toArray();
    report.attrs.reserve(static_cast<size_t>(attrs_table.size()));
    for (const auto& v : attrs_table) {
        const auto obj = v.toObject();
        domain::models::SmartAttribute a;
        a.id = obj.value("id").toInt();
        a.name = obj.value("name").toString();
        a.raw_value = obj.value("raw").toObject().value("value").toVariant().toLongLong();
        a.threshold = obj.value("thresh").toInt();
        report.attrs.push_back(std::move(a));
    }

    return report;
}

} // namespace my_comp::infrastructure::collectors
