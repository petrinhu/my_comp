#include "infrastructure/collectors/vram/leitor_arquivo_config_fs.h"

#include <QFile>

namespace vramopt::infrastructure {

std::optional<QByteArray>
LeitorArquivoConfigFs::ler(const QString& path) const {
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }
    return f.readAll();
}

}
