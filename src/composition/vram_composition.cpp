#include "composition/vram_composition.h"

#include "application/vram/classificacao/classificador_em_cascata.h"
#include "application/vram/classificacao/regras/lista_libs_compute.h"
#include "application/vram/classificacao/regras/mapa_categorias_desktop.h"
#include "application/vram/classificacao/regras/regra_biblioteca_compute.h"
#include "application/vram/classificacao/regras/regra_catalogo.h"
#include "application/vram/classificacao/regras/regra_categoria_desktop.h"
#include "application/vram/classificacao/regras/regra_comportamento.h"
#include "application/vram/classificacao/regras/regra_feedback.h"
#include "application/vram/classificacao/servico_classificar_processo.h"
#include "application/vram/classificacao/servico_sugerir_acao.h"
#include "application/vram/configuracao/localizador_composite.h"
#include "application/vram/configuracao/servico_localizar_config.h"
#include "application/vram/configuracao/sublocators/localizador_browser_chromium.h"
#include "application/vram/configuracao/sublocators/localizador_desktop_wrapper.h"
#include "application/vram/configuracao/sublocators/localizador_env_var.h"
#include "application/vram/configuracao/sublocators/localizador_firefox.h"
#include "application/vram/feedback/servico_registrar_feedback.h"
#include "domain/vram/amostra/limiares_comportamento.h"
#include "infrastructure/collectors/vram/adaptadores_nulos.h"
#include "infrastructure/collectors/vram/catalogo_classificacao_json.h"
#include "infrastructure/collectors/vram/factory_gpu_backend.h"
#include "infrastructure/collectors/vram/gpu_backend_com_historico.h"
#include "infrastructure/collectors/vram/historico_amostras_vram.h"
#include "infrastructure/collectors/vram/leitor_arquivo_config_fs.h"
#include "infrastructure/collectors/vram/leitor_desktop_exec_xdg.h"
#include "infrastructure/collectors/vram/leitor_desktop_file_xdg.h"
#include "infrastructure/collectors/vram/leitor_environ_proc.h"
#include "infrastructure/collectors/vram/leitor_maps_proc.h"
#include "infrastructure/collectors/vram/repositorio_feedback_cifrado.h"
#include "infrastructure/security/openssl_vault.h"

#include <QStandardPaths>

#include <chrono>
#include <utility>

namespace vramopt::composition {

namespace {

constexpr std::chrono::seconds JANELA_COMPORTAMENTO{30};

struct RegraNomeada {
    QString nome;
    std::shared_ptr<domain::IRegraClassificacao> regra;
};

// Fontes de dados das regras que dependem de estado (arquivo, histórico): quem monta a
// cascata decide se são reais (auditoria) ou null objects (só para listar a ordem).
struct FontesDaCascata {
    std::shared_ptr<domain::IRepositorioFeedback>     repositorio_feedback;
    std::shared_ptr<domain::ICatalogoClassificacao>   catalogo;
    std::shared_ptr<domain::IAmostradorComportamento> amostrador;
};

// Tabela única: define a cascata e alimenta nomes_das_regras_da_cascata().
std::vector<RegraNomeada> montar_regras(const FontesDaCascata& fontes) {
    return {
        {QStringLiteral("feedback"),
         std::make_shared<application::RegraFeedback>(fontes.repositorio_feedback)},
        {QStringLiteral("catalogo"),
         std::make_shared<application::RegraCatalogo>(fontes.catalogo)},
        {QStringLiteral("categoria_desktop"),
         std::make_shared<application::RegraCategoriaDesktop>(
             std::make_shared<infrastructure::LeitorDesktopFileXdg>(),
             application::MapaCategoriasDesktop::padrao())},
        {QStringLiteral("biblioteca_compute"),
         std::make_shared<application::RegraBibliotecaCompute>(
             std::make_shared<infrastructure::LeitorMapsProc>(),
             application::ListaLibsCompute::padrao())},
        {QStringLiteral("comportamento"),
         std::make_shared<application::RegraComportamento>(
             fontes.amostrador, domain::LimiaresComportamento::padrao(), JANELA_COMPORTAMENTO)},
    };
}

} // namespace

std::shared_ptr<domain::ICatalogoClassificacao> compor_catalogo_classificacao() {
    return std::make_shared<infrastructure::CatalogoClassificacaoJson>(
        infrastructure::carregar_catalogo_empacotado().entradas);
}

std::shared_ptr<domain::IRepositorioFeedback> compor_repositorio_feedback() {
    // AppDataLocation depende de organização/aplicação: GUI e CLI fixam ambas em "my_comp".
    const QString diretorio =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/vram");
    return std::make_shared<infrastructure::RepositorioFeedbackCifrado>(
        std::make_shared<my_comp::infrastructure::security::OpenSslVault>(), diretorio);
}

BackendComAmostrador compor_backend_com_amostrador(std::shared_ptr<domain::IGpuBackend> backend_real) {
    auto historico = std::make_shared<infrastructure::HistoricoAmostrasVram>();
    return BackendComAmostrador{
        .backend    = std::make_shared<infrastructure::GpuBackendComHistorico>(
            std::move(backend_real), historico),
        .amostrador = historico,
    };
}

std::vector<QString> nomes_das_regras_da_cascata() {
    std::vector<QString> nomes;
    const FontesDaCascata nulas{
        .repositorio_feedback = std::make_shared<infrastructure::RepositorioFeedbackVazio>(),
        .catalogo             = std::make_shared<infrastructure::CatalogoClassificacaoVazio>(),
        .amostrador           = std::make_shared<infrastructure::AmostradorComportamentoNulo>(),
    };
    for (const auto& r : montar_regras(nulas)) {
        nomes.push_back(r.nome);
    }
    return nomes;
}

ServicosVram compor_servicos_vram() {
    auto leitor_arq     = std::make_shared<infrastructure::LeitorArquivoConfigFs>();
    auto leitor_environ = std::make_shared<infrastructure::LeitorEnvironProc>();
    auto leitor_dexec   = std::make_shared<infrastructure::LeitorDesktopExecXdg>();

    std::vector<std::shared_ptr<domain::IConfigLocator>> sublocs = {
        std::make_shared<application::LocalizadorBrowserChromium>(leitor_arq),
        std::make_shared<application::LocalizadorFirefox>(leitor_arq),
        std::make_shared<application::LocalizadorEnvVar>(leitor_environ),
        std::make_shared<application::LocalizadorDesktopWrapper>(leitor_dexec),
    };
    auto loc_composite = std::make_shared<application::LocalizadorComposite>(std::move(sublocs));
    auto svc_localizar = std::make_shared<application::ServicoLocalizarConfig>(loc_composite);

    // Mesma instância do repositório na regra (leitura) e no caso de uso (gravação).
    const auto repositorio_feedback = compor_repositorio_feedback();
    // O amostrador da regra é alimentado pelo próprio backend que a auditoria consulta.
    auto montado = compor_backend_com_amostrador(infrastructure::criar_backend_composito_do_sistema());
    const FontesDaCascata fontes{
        .repositorio_feedback = repositorio_feedback,
        .catalogo             = compor_catalogo_classificacao(),
        .amostrador           = montado.amostrador,
    };

    std::vector<std::shared_ptr<domain::IRegraClassificacao>> regras;
    for (auto& r : montar_regras(fontes)) {
        regras.push_back(std::move(r.regra));
    }
    auto classif     = std::make_shared<application::ClassificadorEmCascata>(std::move(regras));
    auto svc_classif = std::make_shared<application::ServicoClassificarProcesso>(classif);
    auto svc_sugerir = std::make_shared<application::ServicoSugerirAcao>();

    return ServicosVram{
        .auditar = std::make_shared<application::ServicoAuditarVramAtual>(
            std::move(montado.backend), svc_classif, svc_sugerir, svc_localizar),
        .registrar_feedback =
            std::make_shared<application::ServicoRegistrarFeedback>(repositorio_feedback),
    };
}

std::shared_ptr<application::ServicoAuditarVramAtual> compor_servico_auditar_vram() {
    return compor_servicos_vram().auditar;
}

}
