#include "linux_disk_inventory_provider.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStorageInfo>

namespace my_comp::infrastructure::collectors {

namespace {

using my_comp::domain::models::Partition;

// LUKS/LVM aninham `children`; achatar todas as folhas type="part" num único vetor.
void collect_partitions(const QJsonArray& nodes, std::vector<Partition>& out) {
    for (const auto& v : nodes) {
        const auto obj = v.toObject();
        const auto type = obj.value("type").toString();
        if (type == "part" || type == "crypt" || type == "lvm") {
            Partition p;
            p.name = obj.value("name").toString();
            p.mount = obj.value("mountpoint").toString();
            p.fs = obj.value("fstype").toString();
            p.size_bytes = obj.value("size").toVariant().toLongLong();
            p.label = obj.value("label").toString();
            p.uuid = obj.value("uuid").toString();

            if (!p.mount.isEmpty()) {
                const QStorageInfo info(p.mount);
                if (info.isValid() && info.isReady()) {
                    const qint64 total = info.bytesTotal();
                    const qint64 free = info.bytesAvailable();
                    p.free_bytes = free;
                    p.used_bytes = total - free;
                    p.used_percent = total > 0 ? (100.0 * static_cast<double>(total - free) / static_cast<double>(total)) : 0.0;
                }
            }
            out.push_back(std::move(p));
        }
        if (obj.contains("children")) {
            collect_partitions(obj.value("children").toArray(), out);
        }
    }
}

} // namespace

domain::models::DiskInventory LinuxDiskInventoryProvider::collect_inventory() {
    domain::models::DiskInventory inv;
    QString out;
    const QStringList args{
        "-J", "-b",
        "-o", "NAME,SIZE,TYPE,VENDOR,MODEL,ROTA,MOUNTPOINT,FSTYPE,LABEL,UUID"
    };
    if (m_shell->execute("lsblk", args, out) != 0 || out.isEmpty()) return inv;

    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(out.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return inv;

    const auto devices = doc.object().value("blockdevices").toArray();
    for (const auto& v : devices) {
        const auto obj = v.toObject();
        if (obj.value("type").toString() != "disk") continue;

        domain::models::DiskDrive d;
        d.name = obj.value("name").toString();
        d.size_bytes = obj.value("size").toVariant().toLongLong();
        d.type = obj.value("type").toString();
        d.vendor = obj.value("vendor").toString().trimmed();
        d.model = obj.value("model").toString().trimmed();
        d.rotational = obj.value("rota").toBool();

        if (obj.contains("children")) {
            collect_partitions(obj.value("children").toArray(), d.partitions);
        }
        inv.drives.push_back(std::move(d));
    }
    return inv;
}

} // namespace my_comp::infrastructure::collectors
