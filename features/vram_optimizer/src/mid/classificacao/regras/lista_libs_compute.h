#pragma once

#include <QString>
#include <QStringList>

#include <optional>

namespace vramopt::application {

class ListaLibsCompute {
public:
    explicit ListaLibsCompute(QStringList padroes_compute);

    [[nodiscard]] static ListaLibsCompute padrao();

    [[nodiscard]] std::optional<QString>
        primeiraQueCasa(const QStringList& bibliotecas_carregadas) const;

private:
    QStringList m_padroes_compute;
};

}
