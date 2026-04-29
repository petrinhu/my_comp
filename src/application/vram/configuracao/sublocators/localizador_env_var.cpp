#include "application/vram/configuracao/sublocators/localizador_env_var.h"

namespace vramopt::application {

namespace {

struct EnvAlvo {
    QString nome;
    QStringList valores_ativos;
};

const QVector<EnvAlvo>& envsAlvo() {
    static const QVector<EnvAlvo> v = {
        {QStringLiteral("__NV_PRIME_RENDER_OFFLOAD"), {QStringLiteral("1")}},
        {QStringLiteral("__VK_LAYER_NV_optimus"),     {QStringLiteral("NVIDIA_only")}},
        {QStringLiteral("DRI_PRIME"),                 {QStringLiteral("1")}},
    };
    return v;
}

QString instrucaoPara(const QString& nome, const QString& valor) {
    return QStringLiteral(
        "A variável `%1=%2` está setada no processo. Possíveis origens: `~/.bashrc`, "
        "`~/.profile`, `~/.zshrc`, `~/.zshenv` ou `~/.config/environment.d/*.conf`. "
        "Se o app foi lançado via launcher, verifique também o `.desktop` (categoria "
        "DesktopWrapper). Comente/remova a definição e reinicie a sessão.")
        .arg(nome, valor);
}

}

LocalizadorEnvVar::LocalizadorEnvVar(std::shared_ptr<domain::ILeitorEnviron> leitor)
    : m_leitor{std::move(leitor)} {}

QVector<domain::OrigemConfig>
LocalizadorEnvVar::localizar(const domain::Processo& processo) const {
    const auto env = m_leitor->environDe(processo.pid());
    if (env.isEmpty()) {
        return {};
    }

    QVector<domain::OrigemConfig> origens;
    for (const auto& alvo : envsAlvo()) {
        const auto valor = env.value(alvo.nome);
        if (valor.isEmpty() || !alvo.valores_ativos.contains(valor)) {
            continue;
        }
        domain::OrigemConfig o{
            .categoria = domain::CategoriaOrigem::PrimeEnv,
            .tipo = QStringLiteral("env-var"),
            .caminho = QStringLiteral("/proc/%1/environ").arg(processo.pid().valor()),
            .chave = alvo.nome,
            .valor_atual = valor,
            .valor_sugerido = QStringLiteral("(remover)"),
            .instrucao_humana = instrucaoPara(alvo.nome, valor),
            .confianca = domain::Confianca::Alta,
            .requer_reinicio_app = true,
            .aplicavel_automatico = false,
        };
        origens.append(o);
    }
    return origens;
}

}
