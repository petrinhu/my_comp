#include "infrastructure/collectors/vram/leitor_arquivo_config_fs.h"

#include <QFile>
#include <QString>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using my_comp::infrastructure::vram::LeitorArquivoConfigFs;

TEST(LeitorArquivoConfigFsTest, RetornaConteudoQuandoExiste) {
    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString path = tmp.path() + QStringLiteral("/cfg.json");
    QFile f{path};
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(R"({"hardware_acceleration_mode":{"enabled":true}})");
    f.close();

    LeitorArquivoConfigFs leitor;
    auto opt = leitor.ler(path);
    ASSERT_TRUE(opt.has_value());
    EXPECT_TRUE(opt->contains("hardware_acceleration_mode"));
}

TEST(LeitorArquivoConfigFsTest, RetornaNulloptQuandoArquivoAusente) {
    LeitorArquivoConfigFs leitor;
    auto opt = leitor.ler(QStringLiteral("/tmp/nao_existe_xyz_98765.json"));
    EXPECT_FALSE(opt.has_value());
}
