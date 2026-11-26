#include "toolchain_catalog.h"

#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace my_comp::infrastructure::collectors {

namespace {
constexpr qint64 MAX_CATALOG_BYTES = 256 * 1024;

bool has_control_char(const QString& s) {
    for (const QChar c : s) {
        if (c.category() == QChar::Other_Control) return true;
    }
    return false;
}
} // namespace

std::vector<ToolchainSpec> default_toolchain_catalog() {
    return {
        // Compiladores e runtimes
        {"GCC", "gcc", {"--version"}},
        {"G++", "g++", {"--version"}},
        {"Clang", "clang", {"--version"}},
        {"Python", "python3", {"--version"}},
        {"Go", "go", {"version"}},
        {"Rust", "rustc", {"--version"}},
        {"Zig", "zig", {"version"}},
        {"Node.js", "node", {"--version"}},
        {"Deno", "deno", {"--version"}},
        {"Bun", "bun", {"--version"}},
        {"Java", "java", {"-version"}}, // imprime no stderr
        {"Ruby", "ruby", {"--version"}},
        {"PHP", "php", {"--version"}},
        {".NET", "dotnet", {"--version"}},
        // Build
        {"CMake", "cmake", {"--version"}},
        {"Ninja", "ninja", {"--version"}},
        {"Make", "make", {"--version"}},
        // Cluster
        {"kubectl", "kubectl", {"version", "--client"}},
        {"Helm", "helm", {"version", "--short"}},
    };
}

std::vector<ToolchainSpec> parse_toolchain_catalog(const QByteArray& json) {
    std::vector<ToolchainSpec> specs;
    const QJsonDocument doc = QJsonDocument::fromJson(json);
    if (!doc.isArray()) return specs;
    for (const QJsonValue& v : doc.array()) {
        const QJsonObject o = v.toObject();
        const QJsonValue name = o.value("name");
        const QJsonValue command = o.value("command");
        const QJsonValue args = o.value("args");
        if (!name.isString() || !command.isString() || (!args.isUndefined() && !args.isArray())) continue;
        ToolchainSpec spec{name.toString().trimmed(), command.toString().trimmed(), {}};
        bool ok = !spec.name.isEmpty() && !spec.command.isEmpty()
                  && !has_control_char(spec.name) && !has_control_char(spec.command);
        for (const QJsonValue& a : args.toArray()) {
            if (!a.isString() || has_control_char(a.toString())) { ok = false; break; }
            spec.args << a.toString();
        }
        if (ok) specs.push_back(std::move(spec));
    }
    return specs;
}

std::vector<ToolchainSpec> merge_toolchain_catalogs(std::vector<ToolchainSpec> base,
                                                    const std::vector<ToolchainSpec>& user) {
    for (const auto& u : user) {
        auto it = std::find_if(base.begin(), base.end(), [&u](const ToolchainSpec& b) { return b.name == u.name; });
        if (it != base.end()) *it = u;
        else base.push_back(u);
    }
    return base;
}

std::vector<ToolchainSpec> load_toolchain_catalog() {
    auto catalog = default_toolchain_catalog();
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/toolchains.json";
    QFile f(path);
    if (!f.exists()) return catalog;
    if (!f.open(QIODevice::ReadOnly) || f.size() > MAX_CATALOG_BYTES) {
        qWarning() << "[DevStack] ignorando catálogo do usuário ilegível ou grande demais:" << path;
        return catalog;
    }
    return merge_toolchain_catalogs(std::move(catalog), parse_toolchain_catalog(f.readAll()));
}

} // namespace my_comp::infrastructure::collectors
