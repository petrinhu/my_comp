#include "application/vram/configuracao/sublocators/localizador_browser_chromium.h"

#include "application/vram/configuracao/sublocators/tabela_paths_browser_chromium.h"

#include <QByteArray>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace vramopt::application {

namespace {

QString homePath() {
    return QDir::homePath();
}

QString construirCaminhoAbsoluto(const QString& sufixo_local_state) {
    return QStringLiteral("%1/%2").arg(homePath(), sufixo_local_state);
}

}

LocalizadorBrowserChromium::LocalizadorBrowserChromium(
    std::shared_ptr<domain::ILeitorArquivoConfig> leitor)
    : m_leitor{std::move(leitor)} {}

QVector<domain::OrigemConfig>
LocalizadorBrowserChromium::localizar(const domain::Processo& processo) const {
    const auto& tabela = tabelaPathsBrowserChromium();
    auto it = tabela.constFind(processo.nome().valor());
    if (it == tabela.constEnd()) {
        return {};
    }

    const QString caminho = construirCaminhoAbsoluto(it.value());
    auto conteudo_opt = m_leitor->ler(caminho);
    if (!conteudo_opt.has_value()) {
        return {};
    }

    QJsonParseError erro{};
    const auto doc = QJsonDocument::fromJson(*conteudo_opt, &erro);
    if (erro.error != QJsonParseError::NoError || !doc.isObject()) {
        return {};
    }

    const auto raiz = doc.object();
    const auto modo  = raiz.value(QStringLiteral("hardware_acceleration_mode"));
    bool ligada = true;
    bool default_aplicado = true;
    if (modo.isObject()) {
        const auto enabled = modo.toObject().value(QStringLiteral("enabled"));
        if (enabled.isBool()) {
            ligada = enabled.toBool();
            default_aplicado = false;
        }
    }
    if (!ligada) {
        return {};
    }

    const QString valor_atual = default_aplicado
        ? QStringLiteral("true (default)")
        : QStringLiteral("true");

    domain::OrigemConfig o{
        .categoria = domain::CategoriaOrigem::BrowserChromium,
        .tipo = QStringLiteral("browser-flag"),
        .caminho = caminho,
        .chave = QStringLiteral("hardware_acceleration_mode.enabled"),
        .valor_atual = valor_atual,
        .valor_sugerido = QStringLiteral("false"),
        .instrucao_humana = QStringLiteral(
            "Abra `%1` e troque `hardware_acceleration_mode.enabled` para `false`. "
            "Como alternativa, abra `chrome://settings/system` (ou equivalente) e desligue "
            "'Use hardware acceleration when available'. Reinicie o navegador.")
            .arg(caminho),
        .confianca = domain::Confianca::Alta,
        .requer_reinicio_app = true,
        .aplicavel_automatico = false,
    };
    return {o};
}

}
