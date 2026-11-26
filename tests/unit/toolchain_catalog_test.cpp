#include <gtest/gtest.h>
#include "../../src/infrastructure/collectors/linux_dev_stack_provider.h"
#include "../../src/infrastructure/collectors/toolchain_catalog.h"
#include "../../src/infrastructure/collectors/i_shell_executor.h"

#include <QSet>
#include <map>

using namespace my_comp::infrastructure::collectors;

namespace {
QSet<QString> names_of(const std::vector<ToolchainSpec>& specs) {
    QSet<QString> names;
    for (const auto& s : specs) names.insert(s.name);
    return names;
}

// Responde por comando; stderr separado para simular ferramentas como o Java.
struct CapturingShell : IShellExecutor {
    std::map<QString, ShellResult> replies;
    int execute(const QString&, const QStringList&, QString&) override { return 1; }
    ShellResult run_capturing(const QString& command, const QStringList& args, int) override {
        auto it = replies.find((QStringList{command} + args).join(' '));
        return it == replies.end() ? ShellResult{} : it->second;
    }
};
} // namespace

// DT-01a: a lista tinha 8 ferramentas fixas no código.
TEST(ToolchainCatalogTest, DefaultCatalogCoversCommonStacks) {
    const auto names = names_of(default_toolchain_catalog());
    for (const QString n : {"GCC", "G++", "Clang", "Python", "Go", "Rust", "Node.js", "Java",
                            "Ruby", "PHP", ".NET", "Zig", "Deno", "Bun", "CMake", "Ninja",
                            "Make", "kubectl", "Helm"}) {
        EXPECT_TRUE(names.contains(n)) << n.toStdString();
    }
}

// Arquivo do usuário é entrada externa: entradas malformadas são ignoradas.
TEST(ToolchainCatalogTest, ParsesUserJsonAndSkipsInvalidEntries) {
    const QByteArray json = R"([
        {"name": "Elixir", "command": "elixir", "args": ["--version"]},
        {"name": "SemComando", "args": ["--version"]},
        {"name": "ArgsRuins", "command": "x", "args": "--version"},
        {"name": "Ru\nim", "command": "y", "args": []},
        {"name": "Nim", "command": "nim", "args": ["--version"]}
    ])";
    const auto specs = parse_toolchain_catalog(json);
    ASSERT_EQ(specs.size(), 2u);
    EXPECT_EQ(specs[0].name, QString("Elixir"));
    EXPECT_EQ(specs[1].command, QString("nim"));
}

TEST(ToolchainCatalogTest, MalformedJsonYieldsEmpty) {
    EXPECT_TRUE(parse_toolchain_catalog("{not json").empty());
    EXPECT_TRUE(parse_toolchain_catalog(R"({"name":"obj, não array"})").empty());
}

// Usuário pode acrescentar ou sobrescrever (mesmo nome) entradas do padrão.
TEST(ToolchainCatalogTest, MergeAddsAndOverridesByName) {
    std::vector<ToolchainSpec> base = {{"Python", "python3", {"--version"}}, {"Go", "go", {"version"}}};
    std::vector<ToolchainSpec> user = {{"Python", "python3.13", {"--version"}}, {"Nim", "nim", {"--version"}}};
    const auto merged = merge_toolchain_catalogs(base, user);
    ASSERT_EQ(merged.size(), 3u);
    EXPECT_EQ(merged[0].command, QString("python3.13"));
    EXPECT_EQ(merged[2].name, QString("Nim"));
}

// Java imprime a versão no stderr; antes precisava de `sh -c "java -version 2>&1"`.
TEST(ToolchainCatalogTest, VersionIsReadFromStderrWhenStdoutIsEmpty) {
    auto shell = std::make_shared<CapturingShell>();
    shell->replies["java -version"] = {.exit_code = 0, .out = {}, .err = "openjdk version \"21.0.2\" 2024-01-16"};
    shell->replies["kubectl version --client"] = {.exit_code = 0, .out = "Client Version: v1.30.1\n", .err = {}};
    LinuxDevStackProvider provider(shell, {{"Java", "java", {"-version"}}, {"kubectl", "kubectl", {"version", "--client"}},
                                           {"Ausente", "nao-instalado", {"--version"}}});
    const auto info = provider.collect_dev_stacks();
    ASSERT_EQ(info.toolchains.size(), 2u);
    EXPECT_EQ(info.toolchains[0].name, QString("Java"));
    EXPECT_EQ(info.toolchains[0].version, QString("21.0.2"));
    EXPECT_EQ(info.toolchains[1].version, QString("1.30.1"));
}
