#include "infrastructure/collectors/vram/desktop_xdg_search.h"

#include "domain/vram/valueobjects/chave_processo.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>
#include <QStringList>

namespace vramopt::infrastructure::desktop_xdg {

namespace {

QHash<QString, QString> parse_secao_desktop_entry(const QString& path) {
    QHash<QString, QString> kv;
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly)) {
        return kv;
    }
    const auto conteudo = QString::fromUtf8(f.readAll());
    const auto linhas = conteudo.split(QLatin1Char('\n'));
    bool dentro_da_secao = false;
    for (const auto& linha_raw : linhas) {
        const auto linha = linha_raw.trimmed();
        if (linha.isEmpty() || linha.startsWith(QLatin1Char('#'))) {
            continue;
        }
        if (linha.startsWith(QLatin1Char('[')) && linha.endsWith(QLatin1Char(']'))) {
            dentro_da_secao = (linha == QStringLiteral("[Desktop Entry]"));
            continue;
        }
        if (!dentro_da_secao) {
            continue;
        }
        const auto eq = linha.indexOf(QLatin1Char('='));
        if (eq <= 0) {
            continue;
        }
        kv.insert(linha.left(eq).trimmed(), linha.mid(eq + 1).trimmed());
    }
    return kv;
}

QString primeiro_token(const QString& exec) {
    const auto trimmed = exec.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }
    const auto tokens =
        trimmed.split(QRegularExpression{QStringLiteral("\\s+")}, Qt::SkipEmptyParts);
    if (tokens.isEmpty()) {
        return {};
    }
    return QFileInfo{tokens.first()}.fileName();
}

QStringList listar_desktop_files(const QStringList& xdg_dirs) {
    QStringList paths;
    for (const auto& d : xdg_dirs) {
        const QString applications = d + QStringLiteral("/applications");
        QDir dir{applications};
        if (!dir.exists()) {
            continue;
        }
        const auto entries = dir.entryList({QStringLiteral("*.desktop")}, QDir::Files);
        for (const auto& nome : entries) {
            paths.append(dir.absoluteFilePath(nome));
        }
    }
    return paths;
}

std::optional<ArquivoDesktopParseado>
buscar_por_desktop_id(const QStringList& xdg_dirs, const QString& desktop_id) {
    if (desktop_id.isEmpty()) {
        return std::nullopt;
    }
    QString filename = desktop_id;
    if (!filename.endsWith(QStringLiteral(".desktop"))) {
        filename += QStringLiteral(".desktop");
    }
    for (const auto& d : xdg_dirs) {
        const QString candidato = d + QStringLiteral("/applications/") + filename;
        if (QFile::exists(candidato)) {
            ArquivoDesktopParseado out;
            out.caminho = candidato;
            out.entrada_desktop = parse_secao_desktop_entry(candidato);
            return out;
        }
    }
    return std::nullopt;
}

}

QStringList xdg_dirs_do_sistema() {
    QStringList dirs;
    const QString user_data = qgetenv("XDG_DATA_HOME").isEmpty()
        ? QDir::homePath() + QStringLiteral("/.local/share")
        : QString::fromUtf8(qgetenv("XDG_DATA_HOME"));
    dirs.append(user_data);

    const auto sistema = qgetenv("XDG_DATA_DIRS");
    if (sistema.isEmpty()) {
        dirs.append(QStringLiteral("/usr/local/share"));
        dirs.append(QStringLiteral("/usr/share"));
    } else {
        const auto partes =
            QString::fromUtf8(sistema).split(QLatin1Char(':'), Qt::SkipEmptyParts);
        for (const auto& p : partes) {
            dirs.append(p);
        }
    }
    return dirs;
}

std::optional<ArquivoDesktopParseado>
encontrar_por_processo(const QStringList& xdg_dirs, const domain::Processo& processo) {
    for (const auto& chave : processo.chaves()) {
        if (chave.tipo() == domain::TipoChave::DesktopId) {
            auto found = buscar_por_desktop_id(xdg_dirs, chave.valor());
            if (found.has_value()) {
                return found;
            }
        }
    }

    const auto nome_proc = processo.nome().valor();
    if (nome_proc.isEmpty()) {
        return std::nullopt;
    }
    const auto paths = listar_desktop_files(xdg_dirs);
    for (const auto& path : paths) {
        const auto kv = parse_secao_desktop_entry(path);
        const auto exec = kv.value(QStringLiteral("Exec"));
        if (primeiro_token(exec).compare(nome_proc, Qt::CaseInsensitive) == 0) {
            return ArquivoDesktopParseado{.caminho = path, .entrada_desktop = kv};
        }
    }
    return std::nullopt;
}

}
