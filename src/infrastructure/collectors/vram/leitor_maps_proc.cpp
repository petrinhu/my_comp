#include "infrastructure/collectors/vram/leitor_maps_proc.h"

#include "domain/vram/valueobjects/pid.h"

#include <QFile>
#include <QSet>

#include <utility>

namespace vramopt::infrastructure {

namespace {
constexpr int CAMPOS_ANTES_DO_CAMINHO = 5; // endereço perms offset dev inode
constexpr qint64 LIMITE_BYTES_MAPS = 16LL * 1024 * 1024; // entrada externa: teto defensivo
} // namespace

LeitorMapsProc::LeitorMapsProc(QString proc_root)
    : m_proc_root{std::move(proc_root)} {}

QStringList LeitorMapsProc::bibliotecasMapeadas(const domain::PID& pid) const {
    QStringList resultado;
    QFile f{QStringLiteral("%1/%2/maps").arg(m_proc_root).arg(pid.valor())};
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return resultado;
    }
    // Arquivos de /proc reportam tamanho 0: atEnd() seria true de cara. Lê até o teto.
    const QByteArray dados = f.read(LIMITE_BYTES_MAPS);
    QSet<QString> vistos;
    for (const QByteArray& linha : dados.split('\n')) {
        // Pula os 5 campos fixos; o resto (pode ter espaços) é o caminho.
        qsizetype pos = 0;
        int campo = 0;
        while (campo < CAMPOS_ANTES_DO_CAMINHO && pos < linha.size()) {
            while (pos < linha.size() && linha[pos] != ' ') ++pos;
            while (pos < linha.size() && linha[pos] == ' ') ++pos;
            ++campo;
        }
        if (campo < CAMPOS_ANTES_DO_CAMINHO || pos >= linha.size()) {
            continue;
        }
        const QString caminho = QString::fromUtf8(linha.mid(pos)).trimmed();
        if (!caminho.startsWith(QLatin1Char('/')) || !caminho.contains(QStringLiteral(".so"))) {
            continue; // anônimo, [stack]/[heap]/[vdso] ou executável principal
        }
        if (!vistos.contains(caminho)) {
            vistos.insert(caminho);
            resultado << caminho;
        }
    }
    return resultado;
}

}
