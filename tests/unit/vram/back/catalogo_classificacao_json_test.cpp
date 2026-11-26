#include "infrastructure/collectors/vram/catalogo_classificacao_json.h"

#include "domain/vram/classificacao/categoria.h"
#include "domain/vram/valueobjects/chave_processo.h"

#include <QByteArray>
#include <QString>

#include <gtest/gtest.h>

using my_comp::domain::vram::Categoria;
using my_comp::domain::vram::ChaveProcesso;
using my_comp::infrastructure::vram::CatalogoClassificacaoJson;
using my_comp::infrastructure::vram::carregar_catalogo_empacotado;
using my_comp::infrastructure::vram::parsear_catalogo_json;

namespace {

const QByteArray k_catalogo_minimo = R"({
  "versao": 1,
  "entradas": [
    {
      "id": "firefox",
      "binarios": ["firefox", "firefox-bin"],
      "wm_class": ["firefox"],
      "desktop_id": ["org.mozilla.firefox.desktop"],
      "veredito": "CandidatoObvioCpu",
      "justificativa": "Navegador."
    },
    {
      "id": "kwin",
      "binarios": ["kwin_wayland"],
      "veredito": "PrecisaGpu",
      "justificativa": "Compositor."
    }
  ]
})";

}

TEST(CatalogoClassificacaoJsonTest, ParseiaEntradasValidas) {
    const auto r = parsear_catalogo_json(k_catalogo_minimo);
    ASSERT_EQ(r.entradas.size(), 2U);
    EXPECT_EQ(r.rejeitadas, 0);
    EXPECT_EQ(r.entradas[0].id, QStringLiteral("firefox"));
    EXPECT_EQ(r.entradas[0].binarios.size(), 2);
    EXPECT_EQ(r.entradas[0].veredito, Categoria::CandidatoObvioCpu);
    EXPECT_EQ(r.entradas[1].veredito, Categoria::PrecisaGpu);
    EXPECT_TRUE(r.entradas[1].wm_class.isEmpty());
}

TEST(CatalogoClassificacaoJsonTest, BuscaPorBinarioWmClassEDesktopId) {
    const CatalogoClassificacaoJson cat{parsear_catalogo_json(k_catalogo_minimo).entradas};

    auto por_bin = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("firefox-bin")));
    ASSERT_TRUE(por_bin.has_value());
    EXPECT_EQ(por_bin->id, QStringLiteral("firefox"));

    // WM_CLASS e desktop id variam de caixa entre toolkits/distros.
    auto por_wm = cat.buscar(ChaveProcesso::de_wm_class(QStringLiteral("Firefox")));
    ASSERT_TRUE(por_wm.has_value());
    EXPECT_EQ(por_wm->id, QStringLiteral("firefox"));

    auto por_desk = cat.buscar(ChaveProcesso::de_desktop_id(QStringLiteral("org.mozilla.firefox.desktop")));
    ASSERT_TRUE(por_desk.has_value());

    auto kwin = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("kwin_wayland")));
    ASSERT_TRUE(kwin.has_value());
    EXPECT_EQ(kwin->veredito, Categoria::PrecisaGpu);
}

TEST(CatalogoClassificacaoJsonTest, BinarioCasaSoComCaixaExata) {
    const CatalogoClassificacaoJson cat{parsear_catalogo_json(k_catalogo_minimo).entradas};
    // Nomes de executável no Linux diferenciam caixa.
    EXPECT_FALSE(cat.buscar(ChaveProcesso::de_binario(QStringLiteral("Firefox"))).has_value());
}

TEST(CatalogoClassificacaoJsonTest, TipoDeChaveNaoSeMistura) {
    const CatalogoClassificacaoJson cat{parsear_catalogo_json(k_catalogo_minimo).entradas};
    EXPECT_FALSE(cat.buscar(ChaveProcesso::de_wm_class(QStringLiteral("kwin_wayland"))).has_value());
    EXPECT_FALSE(cat.buscar(ChaveProcesso::de_binario(QStringLiteral("desconhecido"))).has_value());
}

TEST(CatalogoClassificacaoJsonTest, JsonMalformadoDevolveVazioSemLancar) {
    const auto r = parsear_catalogo_json(QByteArray{"{ isto nao e json"});
    EXPECT_TRUE(r.entradas.empty());
    EXPECT_TRUE(parsear_catalogo_json(QByteArray{}).entradas.empty());
    EXPECT_TRUE(parsear_catalogo_json(QByteArray{"[1,2,3]"}).entradas.empty());
}

TEST(CatalogoClassificacaoJsonTest, VersaoDesconhecidaRejeitaCatalogoInteiro) {
    const auto r = parsear_catalogo_json(R"({"versao": 2, "entradas": [
        {"id": "a", "binarios": ["a"], "veredito": "PrecisaGpu", "justificativa": "x"}]})");
    EXPECT_TRUE(r.entradas.empty());
}

TEST(CatalogoClassificacaoJsonTest, EntradasInvalidasSaoIgnoradasEContadas) {
    const auto r = parsear_catalogo_json(R"({"versao": 1, "entradas": [
        {"id": "ok", "binarios": ["ok"], "veredito": "PrecisaGpu", "justificativa": "valida"},
        "nao sou objeto",
        {"binarios": ["sem_id"], "veredito": "PrecisaGpu", "justificativa": "x"},
        {"id": "sem_veredito", "binarios": ["a"], "justificativa": "x"},
        {"id": "veredito_ruim", "binarios": ["b"], "veredito": "Talvez", "justificativa": "x"},
        {"id": "indeterminado", "binarios": ["c"], "veredito": "Indeterminado", "justificativa": "x"},
        {"id": "sem_chaves", "veredito": "PrecisaGpu", "justificativa": "x"},
        {"id": "chave_nao_string", "binarios": [42], "veredito": "PrecisaGpu", "justificativa": "x"},
        {"id": "chave_vazia", "binarios": [""], "veredito": "PrecisaGpu", "justificativa": "x"},
        {"id": "binarios_nao_lista", "binarios": "d", "veredito": "PrecisaGpu", "justificativa": "x"},
        {"id": "sem_justificativa", "binarios": ["e"], "veredito": "PrecisaGpu"},
        {"id": "ok", "binarios": ["duplicado"], "veredito": "PrecisaGpu", "justificativa": "id repetido"}
    ]})");
    ASSERT_EQ(r.entradas.size(), 1U);
    EXPECT_EQ(r.entradas[0].id, QStringLiteral("ok"));
    EXPECT_EQ(r.rejeitadas, 11);
}

TEST(CatalogoClassificacaoJsonTest, ChaveRepetidaEntreEntradasMantemAPrimeira) {
    const auto r = parsear_catalogo_json(R"({"versao": 1, "entradas": [
        {"id": "primeira", "binarios": ["x"], "veredito": "PrecisaGpu", "justificativa": "1"},
        {"id": "segunda", "binarios": ["x", "y"], "veredito": "CandidatoObvioCpu", "justificativa": "2"}
    ]})");
    const CatalogoClassificacaoJson cat{r.entradas};
    auto x = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("x")));
    ASSERT_TRUE(x.has_value());
    EXPECT_EQ(x->id, QStringLiteral("primeira"));
    auto y = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("y")));
    ASSERT_TRUE(y.has_value());
    EXPECT_EQ(y->id, QStringLiteral("segunda"));
}

TEST(CatalogoClassificacaoJsonTest, EntradaExcessivamenteGrandeERejeitada) {
    const QByteArray enorme(2 * 1024 * 1024, ' ');
    EXPECT_TRUE(parsear_catalogo_json(enorme).entradas.empty());
}

// O catálogo empacotado no .qrc precisa estar íntegro: nenhuma entrada rejeitada
// e as entradas óbvias presentes (navegador → CPU, compositor → GPU).
TEST(CatalogoClassificacaoJsonTest, CatalogoEmpacotadoCarregaSemRejeicoes) {
    const auto r = carregar_catalogo_empacotado();
    EXPECT_GE(r.entradas.size(), 10U);
    EXPECT_EQ(r.rejeitadas, 0);

    const CatalogoClassificacaoJson cat{r.entradas};
    auto firefox = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("firefox")));
    ASSERT_TRUE(firefox.has_value());
    EXPECT_EQ(firefox->veredito, Categoria::CandidatoObvioCpu);

    auto kwin = cat.buscar(ChaveProcesso::de_binario(QStringLiteral("kwin_wayland")));
    ASSERT_TRUE(kwin.has_value());
    EXPECT_EQ(kwin->veredito, Categoria::PrecisaGpu);
}
