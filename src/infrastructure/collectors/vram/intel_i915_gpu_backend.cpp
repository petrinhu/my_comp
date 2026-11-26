#include "infrastructure/collectors/vram/intel_i915_gpu_backend.h"

#include "domain/vram/valueobjects/chave_processo.h"
#include "domain/vram/valueobjects/nome_processo.h"
#include "domain/vram/valueobjects/pid.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <algorithm>
#include <cstdint>
#include <utility>

namespace my_comp::infrastructure::vram {

namespace {

QString ler_arquivo_trim(const QString& path) {
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return QString::fromUtf8(f.readAll()).trimmed();
}

QHash<QString, QString> parse_uevent(const QString& path) {
    QHash<QString, QString> resultado;
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly)) {
        return resultado;
    }
    const auto conteudo = QString::fromUtf8(f.readAll());
    const auto linhas = conteudo.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const auto& linha : linhas) {
        const auto eq = linha.indexOf(QLatin1Char('='));
        if (eq <= 0) {
            continue;
        }
        resultado.insert(linha.left(eq), linha.mid(eq + 1));
    }
    return resultado;
}

bool is_card_dir(const QString& nome) {
    static const QRegularExpression re{QStringLiteral("^card\\d+$")};
    return re.match(nome).hasMatch();
}

QString modelo_do_pci_id(const QString& pci_id) {
    if (pci_id.isEmpty()) {
        return QStringLiteral("Intel GPU");
    }
    const auto partes = pci_id.split(QLatin1Char(':'));
    if (partes.size() != 2) {
        return QStringLiteral("Intel GPU %1").arg(pci_id);
    }
    return QStringLiteral("Intel GPU %1").arg(partes.at(1));
}

domain::vram::BytesDeMemoria ler_vram_total(const QString& device_dir) {
    const QString lmem = ler_arquivo_trim(device_dir + QStringLiteral("/mem_info_lmem_total"));
    if (lmem.isEmpty()) {
        return domain::vram::BytesDeMemoria{0};
    }
    bool ok = false;
    const auto valor = lmem.toULongLong(&ok);
    if (!ok) {
        return domain::vram::BytesDeMemoria{0};
    }
    return domain::vram::BytesDeMemoria{valor};
}

QHash<QString, QString> parse_fdinfo(const QString& path) {
    QHash<QString, QString> kv;
    QFile f{path};
    if (!f.open(QIODevice::ReadOnly)) {
        return kv;
    }
    const auto conteudo = QString::fromUtf8(f.readAll());
    const auto linhas = conteudo.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const auto& linha : linhas) {
        if (!linha.startsWith(QStringLiteral("drm-"))) {
            continue;
        }
        const auto sep = linha.indexOf(QLatin1Char(':'));
        if (sep <= 0) {
            continue;
        }
        kv.insert(linha.left(sep), linha.mid(sep + 1).trimmed());
    }
    return kv;
}

std::uint64_t parse_bytes_field(const QString& value) {
    if (value.isEmpty()) {
        return 0;
    }
    const auto partes = value.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (partes.isEmpty()) {
        return 0;
    }
    bool ok = false;
    const auto numero = partes.first().toULongLong(&ok);
    if (!ok) {
        return 0;
    }
    if (partes.size() == 1) {
        return numero;
    }
    const auto unidade = partes.at(1).toLower();
    if (unidade == QStringLiteral("kib")) {
        return numero * 1024U;
    }
    if (unidade == QStringLiteral("mib")) {
        return numero * 1024U * 1024U;
    }
    if (unidade == QStringLiteral("gib")) {
        return numero * 1024U * 1024U * 1024U;
    }
    return numero;
}

QString ler_nome_processo(const QString& proc_pid_dir) {
    QString comm = ler_arquivo_trim(proc_pid_dir + QStringLiteral("/comm"));
    if (!comm.isEmpty()) {
        return comm;
    }
    return QStringLiteral("?");
}

}

IntelI915GpuBackend::IntelI915GpuBackend(QString drm_root, QString proc_root)
    : m_drm_root{std::move(drm_root)},
      m_proc_root{std::move(proc_root)} {}

bool IntelI915GpuBackend::sistema_tem_gpu_intel(const QString& drm_root) {
    QDir dir{drm_root};
    if (!dir.exists()) {
        return false;
    }
    const auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto& nome : entries) {
        if (!is_card_dir(nome)) {
            continue;
        }
        const QString vendor = ler_arquivo_trim(
            dir.absoluteFilePath(nome) + QStringLiteral("/device/vendor"));
        if (vendor.compare(QStringLiteral("0x8086"), Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

std::vector<IntelI915GpuBackend::CardIntel>
IntelI915GpuBackend::descobrir_cards() const {
    std::vector<CardIntel> cards;
    QDir dir{m_drm_root};
    if (!dir.exists()) {
        return cards;
    }
    auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    std::sort(entries.begin(), entries.end());

    int proximo_id = 0;
    for (const auto& nome : entries) {
        if (!is_card_dir(nome)) {
            continue;
        }
        const QString card_dir = dir.absoluteFilePath(nome);
        const QString device_dir = card_dir + QStringLiteral("/device");
        const QString vendor = ler_arquivo_trim(device_dir + QStringLiteral("/vendor"));
        if (vendor.compare(QStringLiteral("0x8086"), Qt::CaseInsensitive) != 0) {
            continue;
        }
        const auto uevent = parse_uevent(device_dir + QStringLiteral("/uevent"));
        cards.push_back(CardIntel{
            .id_local   = proximo_id++,
            .card_dir   = card_dir,
            .pci_slot   = uevent.value(QStringLiteral("PCI_SLOT_NAME")),
            .modelo     = modelo_do_pci_id(uevent.value(QStringLiteral("PCI_ID"))),
            .vram_total = ler_vram_total(device_dir),
        });
    }
    return cards;
}

std::vector<my_comp::domain::vram::GpuInfo>
IntelI915GpuBackend::enumerar_gpus() const {
    std::vector<domain::vram::GpuInfo> gpus;
    const auto cards = descobrir_cards();
    gpus.reserve(cards.size());
    for (const auto& c : cards) {
        gpus.emplace_back(c.id_local, domain::vram::Fabricante::Intel, c.modelo, c.vram_total);
    }
    return gpus;
}

std::vector<my_comp::domain::vram::Processo>
IntelI915GpuBackend::enumerar_processos_vram(int gpu_id) const {
    const auto cards = descobrir_cards();
    auto it = std::find_if(cards.begin(), cards.end(),
                           [gpu_id](const CardIntel& c) { return c.id_local == gpu_id; });
    if (it == cards.end() || it->pci_slot.isEmpty()) {
        return {};
    }
    const QString pdev = it->pci_slot;

    QDir proc{m_proc_root};
    if (!proc.exists()) {
        return {};
    }
    const auto entries = proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QHash<int, std::uint64_t> bytes_por_pid;
    QHash<int, QString> nome_por_pid;

    for (const auto& nome : entries) {
        bool ok = false;
        const int pid_n = nome.toInt(&ok);
        if (!ok || pid_n <= 0) {
            continue;
        }
        const QString fdinfo_dir = proc.absoluteFilePath(nome) + QStringLiteral("/fdinfo");
        QDir fd{fdinfo_dir};
        if (!fd.exists()) {
            continue;
        }
        const auto fds = fd.entryList(QDir::Files);
        for (const auto& fd_nome : fds) {
            const auto kv = parse_fdinfo(fd.absoluteFilePath(fd_nome));
            if (kv.value(QStringLiteral("drm-pdev")) != pdev) {
                continue;
            }
            const std::uint64_t vram =
                parse_bytes_field(kv.value(QStringLiteral("drm-total-vram")));
            const std::uint64_t systmem =
                parse_bytes_field(kv.value(QStringLiteral("drm-total-system")));
            bytes_por_pid[pid_n] += (vram > 0 ? vram : systmem);
            if (!nome_por_pid.contains(pid_n)) {
                nome_por_pid.insert(pid_n,
                    ler_nome_processo(proc.absoluteFilePath(nome)));
            }
        }
    }

    std::vector<domain::vram::Processo> processos;
    processos.reserve(static_cast<std::size_t>(bytes_por_pid.size()));
    auto pids = bytes_por_pid.keys();
    std::sort(pids.begin(), pids.end());
    for (const int pid_n : pids) {
        const QString nome = nome_por_pid.value(pid_n, QStringLiteral("?"));
        std::vector<domain::vram::ChaveProcesso> chaves;
        chaves.push_back(domain::vram::ChaveProcesso::de_binario(nome));
        processos.emplace_back(domain::vram::PID{pid_n},
                               domain::vram::NomeProcesso{nome},
                               domain::vram::BytesDeMemoria{bytes_por_pid.value(pid_n)},
                               gpu_id,
                               std::move(chaves));
    }
    return processos;
}

}
