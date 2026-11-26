#include "application/vram/configuracao/sublocators/localizador_desktop_wrapper.h"

#include "application/vram/configuracao/sublocators/lista_wrappers_desktop.h"

#include <QRegularExpression>
#include <QStringList>

namespace my_comp::application::vram {

namespace {

QStringList tokenizar_simples(const QString& exec) {
    return exec.trimmed().split(QRegularExpression{QStringLiteral("\\s+")}, Qt::SkipEmptyParts);
}

QString reconstruir_sem_primeiro_token(const QStringList& tokens) {
    if (tokens.size() <= 1) {
        return QString{};
    }
    return tokens.mid(1).join(QLatin1Char(' '));
}

}

LocalizadorDesktopWrapper::LocalizadorDesktopWrapper(
    std::shared_ptr<domain::vram::ILeitorDesktopExec> leitor)
    : m_leitor{std::move(leitor)} {}

QVector<domain::vram::OrigemConfig>
LocalizadorDesktopWrapper::localizar(const domain::vram::Processo& processo) const {
    auto entrada_opt = m_leitor->entrada_de(processo);
    if (!entrada_opt.has_value()) {
        return {};
    }

    const auto tokens = tokenizar_simples(entrada_opt->exec);
    if (tokens.isEmpty()) {
        return {};
    }

    const auto& tabela = lista_wrappers_desktop();
    auto it = tabela.constFind(tokens.first());
    if (it == tabela.constEnd()) {
        return {};
    }

    const QString sem_wrapper = reconstruir_sem_primeiro_token(tokens);

    domain::vram::OrigemConfig o{
        .categoria = domain::vram::CategoriaOrigem::DesktopWrapper,
        .tipo = QStringLiteral("desktop-exec"),
        .caminho = entrada_opt->caminho_arquivo,
        .chave = QStringLiteral("Exec"),
        .valor_atual = entrada_opt->exec,
        .valor_sugerido = sem_wrapper,
        .instrucao_humana = QStringLiteral(
            "O `.desktop` em `%1` usa o wrapper `%2`, que afeta uso de GPU. "
            "Edite a linha `Exec=` removendo `%2` (sugerido: `%3`). Se o "
            "arquivo está em `/usr/share/applications/`, copie para "
            "`~/.local/share/applications/` antes de editar — assim o user "
            "override prevalece. Próxima abertura via launcher já não usará "
            "o wrapper.")
            .arg(entrada_opt->caminho_arquivo, tokens.first(), sem_wrapper),
        .confianca = it.value(),
        .requer_reinicio_app = true,
        .aplicavel_automatico = false,
    };
    return {o};
}

}
