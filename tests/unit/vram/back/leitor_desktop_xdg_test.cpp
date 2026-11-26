#include "domain/vram/entidades/processo.h"
#include "domain/vram/valueobjects/bytes_de_memoria.h"
#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"
#include "infrastructure/collectors/vram/leitor_desktop_exec_xdg.h"
#include "infrastructure/collectors/vram/leitor_desktop_file_xdg.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryDir>
#include <QTextStream>

#include <gtest/gtest.h>

using vramopt::domain::BytesDeMemoria;
using vramopt::domain::ChaveProcesso;
using vramopt::domain::NomeProcesso;
using vramopt::domain::PID;
using vramopt::domain::Processo;
using vramopt::infrastructure::LeitorDesktopExecXdg;
using vramopt::infrastructure::LeitorDesktopFileXdg;

namespace {

void escreverDesktop(const QString& xdg_root, const QString& filename,
                     const QString& exec, const QString& categories,
                     const QString& nome_human = QStringLiteral("App")) {
    const QString dir = xdg_root + QStringLiteral("/applications");
    QDir{}.mkpath(dir);
    QFile f{dir + QStringLiteral("/") + filename};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out{&f};
    out << "[Desktop Entry]\n";
    out << "Type=Application\n";
    out << "Name=" << nome_human << "\n";
    out << "Exec=" << exec << "\n";
    if (!categories.isEmpty()) {
        out << "Categories=" << categories << "\n";
    }
}

Processo fazerProcesso(int pid, const QString& nome,
                       std::vector<ChaveProcesso> chaves = {}) {
    return Processo{PID{pid}, NomeProcesso{nome}, BytesDeMemoria{0}, 0,
                    std::move(chaves)};
}

}

TEST(LeitorDesktopExecXdgTest, EncontraPorPrefixDoExecQuandoNomeBate) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    escreverDesktop(xdg, QStringLiteral("blender.desktop"),
                    QStringLiteral("blender %f"),
                    QStringLiteral("Graphics;3DGraphics;"));
    escreverDesktop(xdg, QStringLiteral("kmail.desktop"),
                    QStringLiteral("kmail"), QStringLiteral("Office;"));

    LeitorDesktopExecXdg leitor{{xdg}};
    const auto resultado =
        leitor.entrada_de(fazerProcesso(100, QStringLiteral("blender")));

    ASSERT_TRUE(resultado.has_value());
    EXPECT_EQ(resultado->exec, QStringLiteral("blender %f"));
    EXPECT_TRUE(resultado->caminho_arquivo.endsWith(QStringLiteral("blender.desktop")));
}

TEST(LeitorDesktopExecXdgTest, ResolvePrimeiroPorChaveDesktopId) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    escreverDesktop(xdg, QStringLiteral("org.example.foo.desktop"),
                    QStringLiteral("/usr/bin/wrapper /usr/bin/foo --opt"),
                    QStringLiteral("Application;"));

    LeitorDesktopExecXdg leitor{{xdg}};
    auto resultado = leitor.entrada_de(fazerProcesso(101, QStringLiteral("foo"),
        {ChaveProcesso::de_desktop_id(QStringLiteral("org.example.foo"))}));
    ASSERT_TRUE(resultado.has_value());
    EXPECT_TRUE(resultado->exec.contains(QStringLiteral("wrapper")));
}

TEST(LeitorDesktopExecXdgTest, RetornaNulloptQuandoNaoCasa) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    escreverDesktop(xdg, QStringLiteral("krita.desktop"),
                    QStringLiteral("krita"),
                    QStringLiteral("Graphics;"));

    LeitorDesktopExecXdg leitor{{xdg}};
    EXPECT_FALSE(leitor.entrada_de(fazerProcesso(1, QStringLiteral("blender"))).has_value());
}

TEST(LeitorDesktopFileXdgTest, ExtraiCategoriasDoArquivoDesktop) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    escreverDesktop(xdg, QStringLiteral("inkscape.desktop"),
                    QStringLiteral("inkscape %F"),
                    QStringLiteral("Graphics;VectorGraphics;"));

    LeitorDesktopFileXdg leitor{{xdg}};
    const auto cats = leitor.categorias_de(fazerProcesso(1, QStringLiteral("inkscape")));
    EXPECT_TRUE(cats.contains(QStringLiteral("Graphics")));
    EXPECT_TRUE(cats.contains(QStringLiteral("VectorGraphics")));
}

TEST(LeitorDesktopFileXdgTest, RetornaVazioQuandoNaoEncontra) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    LeitorDesktopFileXdg leitor{{xdg}};
    EXPECT_TRUE(leitor.categorias_de(fazerProcesso(1, QStringLiteral("ausente"))).isEmpty());
}

TEST(LeitorDesktopFileXdgTest, IgnoraSecoesAlternativasDoArquivo) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString xdg = tmp.path() + QStringLiteral("/share");
    QDir{}.mkpath(xdg + QStringLiteral("/applications"));
    QFile f{xdg + QStringLiteral("/applications/multi.desktop")};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out{&f};
    out << "[Desktop Entry]\nName=Multi\nExec=multi\nCategories=Real;\n\n";
    out << "[Desktop Action Outra]\nName=Outra\nExec=multi --x\nCategories=Falsa;\n";
    f.close();

    LeitorDesktopFileXdg leitor{{xdg}};
    const auto cats = leitor.categorias_de(fazerProcesso(1, QStringLiteral("multi")));
    EXPECT_TRUE(cats.contains(QStringLiteral("Real")));
    EXPECT_FALSE(cats.contains(QStringLiteral("Falsa")));
}
