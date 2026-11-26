#include "application/vram/configuracao/sublocators/localizador_firefox.h"

#include <QDir>
#include <QHash>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <optional>

namespace vramopt::application {

namespace {

bool nome_eh_firefox(const QString& nome) {
    static const QStringList aliases = {
        QStringLiteral("firefox"),
        QStringLiteral("firefox-esr"),
        QStringLiteral("firefox-bin"),
        QStringLiteral("librewolf"),
    };
    return aliases.contains(nome);
}

std::optional<QString> profile_default_de_ini(const QByteArray& ini) {
    QString secao_atual;
    QHash<QString, QHash<QString, QString>> tudo;
    const auto linhas = QString::fromUtf8(ini).split(QLatin1Char('\n'));
    for (const auto& linha : linhas) {
        const auto trimmed = linha.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith(QLatin1Char('#'))
            || trimmed.startsWith(QLatin1Char(';'))) {
            continue;
        }
        if (trimmed.startsWith(QLatin1Char('[')) && trimmed.endsWith(QLatin1Char(']'))) {
            secao_atual = trimmed.mid(1, trimmed.size() - 2);
            continue;
        }
        const qsizetype eq = trimmed.indexOf(QLatin1Char('='));
        if (eq <= 0) {
            continue;
        }
        const auto k = trimmed.left(eq).trimmed();
        const auto v = trimmed.mid(eq + 1).trimmed();
        tudo[secao_atual][k] = v;
    }
    for (auto it = tudo.constBegin(); it != tudo.constEnd(); ++it) {
        if (it.value().value(QStringLiteral("Default")) == QStringLiteral("1")) {
            const auto path = it.value().value(QStringLiteral("Path"));
            if (!path.isEmpty()) {
                return path;
            }
        }
    }
    return std::nullopt;
}

std::optional<QString> ler_user_pref(const QByteArray& conteudo, const QString& chave) {
    static const QString tpl = QStringLiteral(R"(user_pref\s*\(\s*"%1"\s*,\s*([^)]+?)\s*\)\s*;)");
    QRegularExpression re{tpl.arg(QRegularExpression::escape(chave))};
    auto match = re.match(QString::fromUtf8(conteudo));
    if (!match.hasMatch()) {
        return std::nullopt;
    }
    return match.captured(1).trimmed();
}

struct Chave {
    QString nome;
    QString valor_sugerido;
    QString descricao_humana;
};

const QVector<Chave>& chaves_firefox() {
    static const QVector<Chave> v = {
        {QStringLiteral("layers.acceleration.disabled"), QStringLiteral("true"),
         QStringLiteral("desabilita aceleração de camadas (forço CPU compositing)")},
        {QStringLiteral("webgl.disabled"), QStringLiteral("true"),
         QStringLiteral("desabilita WebGL (sem renderização 3D em páginas)")},
        {QStringLiteral("gfx.webrender.all"), QStringLiteral("false"),
         QStringLiteral("desliga WebRender (compositor por GPU)")},
    };
    return v;
}

}

LocalizadorFirefox::LocalizadorFirefox(std::shared_ptr<domain::ILeitorArquivoConfig> leitor)
    : m_leitor{std::move(leitor)} {}

QVector<domain::OrigemConfig>
LocalizadorFirefox::localizar(const domain::Processo& processo) const {
    if (!nome_eh_firefox(processo.nome().valor())) {
        return {};
    }

    const QString home = QDir::homePath();
    const QString profiles_ini = QStringLiteral("%1/.mozilla/firefox/profiles.ini").arg(home);
    auto ini_opt = m_leitor->ler(profiles_ini);
    if (!ini_opt.has_value()) {
        return {};
    }
    auto profile_opt = profile_default_de_ini(*ini_opt);
    if (!profile_opt.has_value()) {
        return {};
    }

    const QString profile_dir =
        QStringLiteral("%1/.mozilla/firefox/%2").arg(home, *profile_opt);
    const QString user_js  = profile_dir + QStringLiteral("/user.js");
    const QString prefs_js = profile_dir + QStringLiteral("/prefs.js");

    auto user_opt  = m_leitor->ler(user_js);
    auto prefs_opt = m_leitor->ler(prefs_js);
    const QByteArray user_conteudo  = user_opt.value_or(QByteArray{});
    const QByteArray prefs_conteudo = prefs_opt.value_or(QByteArray{});

    QVector<domain::OrigemConfig> origens;
    for (const auto& chave : chaves_firefox()) {
        auto valor_user  = ler_user_pref(user_conteudo, chave.nome);
        auto valor_prefs = ler_user_pref(prefs_conteudo, chave.nome);
        std::optional<QString> valor_lido = valor_user.has_value() ? valor_user : valor_prefs;

        if (valor_lido.has_value() && *valor_lido == chave.valor_sugerido) {
            continue;
        }

        const QString valor_atual = valor_lido.has_value()
            ? *valor_lido
            : QStringLiteral("<não-definida> (default GPU)");

        const QString caminho_principal = user_opt.has_value() ? user_js : prefs_js;

        domain::OrigemConfig o{
            .categoria = domain::CategoriaOrigem::Firefox,
            .tipo = QStringLiteral("browser-flag"),
            .caminho = caminho_principal,
            .chave = chave.nome,
            .valor_atual = valor_atual,
            .valor_sugerido = chave.valor_sugerido,
            .instrucao_humana = QStringLiteral(
                "Em `about:config` (ou edite `%1`), ajuste `%2` para `%3` (%4). "
                "Reinicie o Firefox.")
                .arg(caminho_principal, chave.nome, chave.valor_sugerido, chave.descricao_humana),
            .confianca = domain::Confianca::Alta,
            .requer_reinicio_app = true,
            .aplicavel_automatico = false,
        };
        origens.append(o);
    }
    return origens;
}

}
