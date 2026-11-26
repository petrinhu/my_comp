#include "infrastructure/collectors/vram/leitor_environ_proc.h"

#include <QFile>

#include <utility>

namespace my_comp::infrastructure::vram {

LeitorEnvironProc::LeitorEnvironProc(QString proc_root)
    : m_proc_root{std::move(proc_root)} {}

QHash<QString, QString>
LeitorEnvironProc::environ_de(const domain::vram::PID& pid) const {
    QHash<QString, QString> resultado;
    const QString path = QStringLiteral("%1/%2/environ").arg(m_proc_root).arg(pid.valor());
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly)) {
        return resultado;
    }
    const QByteArray dados = f.readAll();
    int inicio = 0;
    for (int i = 0; i <= dados.size(); ++i) {
        if (i == dados.size() || dados[i] == '\0') {
            if (i > inicio) {
                const QByteArray entrada = dados.mid(inicio, i - inicio);
                const auto eq = entrada.indexOf('=');
                if (eq > 0) {
                    resultado.insert(QString::fromUtf8(entrada.left(eq)),
                                     QString::fromUtf8(entrada.mid(eq + 1)));
                }
            }
            inicio = i + 1;
        }
    }
    return resultado;
}

}
