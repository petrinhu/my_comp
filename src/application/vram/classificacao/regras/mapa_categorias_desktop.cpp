#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"

namespace my_comp::application::vram {

MapaCategoriasDesktop::MapaCategoriasDesktop(QSet<QString> candidato_cpu, QSet<QString> precisa_gpu)
    : m_candidato_cpu{std::move(candidato_cpu)},
      m_precisa_gpu{std::move(precisa_gpu)} {}

MapaCategoriasDesktop MapaCategoriasDesktop::padrao() {
    QSet<QString> candidato = {
        QStringLiteral("WebBrowser"),
        QStringLiteral("InstantMessaging"), QStringLiteral("IRCClient"),
        QStringLiteral("Chat"),             QStringLiteral("Email"),
        QStringLiteral("Office"),           QStringLiteral("WordProcessor"),
        QStringLiteral("Spreadsheet"),      QStringLiteral("Presentation"),
        QStringLiteral("Calendar"),         QStringLiteral("ContactManagement"),
        QStringLiteral("TextEditor"),       QStringLiteral("Documentation"),
        QStringLiteral("Dictionary"),
        QStringLiteral("Audio"),            QStringLiteral("Music"),
        QStringLiteral("Mixer"),
        QStringLiteral("FileTransfer"),     QStringLiteral("P2P"),
        QStringLiteral("RemoteAccess"),     QStringLiteral("News"),
        QStringLiteral("RSS"),
        QStringLiteral("Calculator"),       QStringLiteral("Finance"),
        QStringLiteral("Utility"),
    };
    QSet<QString> gpu = {
        QStringLiteral("Game"),             QStringLiteral("ActionGame"),
        QStringLiteral("AdventureGame"),    QStringLiteral("RolePlaying"),
        QStringLiteral("Simulation"),       QStringLiteral("StrategyGame"),
        QStringLiteral("ArcadeGame"),
        QStringLiteral("3DGraphics"),       QStringLiteral("VideoEditor"),
        QStringLiteral("Science"),          QStringLiteral("ArtificialIntelligence"),
    };
    return MapaCategoriasDesktop{std::move(candidato), std::move(gpu)};
}

std::optional<domain::vram::Categoria>
MapaCategoriasDesktop::classificar_por(const QStringList& categorias) const {
    bool tem_cpu = false;
    bool tem_gpu = false;
    for (const auto& c : categorias) {
        if (m_candidato_cpu.contains(c)) {
            tem_cpu = true;
        }
        if (m_precisa_gpu.contains(c)) {
            tem_gpu = true;
        }
    }
    if (tem_cpu && tem_gpu) {
        return std::nullopt;
    }
    if (tem_cpu) {
        return domain::vram::Categoria::CandidatoObvioCpu;
    }
    if (tem_gpu) {
        return domain::vram::Categoria::PrecisaGpu;
    }
    return std::nullopt;
}

}
