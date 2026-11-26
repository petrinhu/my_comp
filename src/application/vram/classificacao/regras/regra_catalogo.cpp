#include "application/vram/classificacao/regras/regra_catalogo.h"

#include <QString>

namespace my_comp::application::vram {

namespace {

QString tipo_chave_as_string(domain::vram::TipoChave t) {
    switch (t) {
        case domain::vram::TipoChave::Binario:    return QStringLiteral("binario");
        case domain::vram::TipoChave::WmClass:    return QStringLiteral("wmClass");
        case domain::vram::TipoChave::DesktopId:  return QStringLiteral("desktopId");
    }
    return QStringLiteral("?");
}

}

RegraCatalogo::RegraCatalogo(std::shared_ptr<domain::vram::ICatalogoClassificacao> cat)
    : m_catalogo{std::move(cat)} {}

std::optional<domain::vram::ResultadoClassificacao>
RegraCatalogo::avaliar(const domain::vram::Processo& processo) const {
    for (const auto& chave : processo.chaves()) {
        auto entrada = m_catalogo->buscar(chave);
        if (entrada.has_value()) {
            return domain::vram::ResultadoClassificacao{
                .categoria = entrada->veredito,
                .justificativa = domain::vram::Justificativa{
                    .regra = QStringLiteral("catalogo"),
                    .detalhe = QStringLiteral("casou %1=%2 (id=%3)")
                        .arg(tipo_chave_as_string(chave.tipo()))
                        .arg(chave.valor())
                        .arg(entrada->id),
                },
            };
        }
    }
    return std::nullopt;
}

}
