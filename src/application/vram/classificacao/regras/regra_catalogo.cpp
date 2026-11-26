#include "application/vram/classificacao/regras/regra_catalogo.h"

#include <QString>

namespace vramopt::application {

namespace {

QString tipo_chave_as_string(domain::TipoChave t) {
    switch (t) {
        case domain::TipoChave::Binario:    return QStringLiteral("binario");
        case domain::TipoChave::WmClass:    return QStringLiteral("wmClass");
        case domain::TipoChave::DesktopId:  return QStringLiteral("desktopId");
    }
    return QStringLiteral("?");
}

}

RegraCatalogo::RegraCatalogo(std::shared_ptr<domain::ICatalogoClassificacao> cat)
    : m_catalogo{std::move(cat)} {}

std::optional<domain::ResultadoClassificacao>
RegraCatalogo::avaliar(const domain::Processo& processo) const {
    for (const auto& chave : processo.chaves()) {
        auto entrada = m_catalogo->buscar(chave);
        if (entrada.has_value()) {
            return domain::ResultadoClassificacao{
                .categoria = entrada->veredito,
                .justificativa = domain::Justificativa{
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
