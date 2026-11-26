#pragma once

#include "domain/vram/interfaces/i_leitor_environ.h"

#include <QString>

namespace vramopt::infrastructure {

class LeitorEnvironProc final : public domain::ILeitorEnviron {
public:
    explicit LeitorEnvironProc(QString proc_root = QStringLiteral("/proc"));

    [[nodiscard]] QHash<QString, QString>
        environ_de(const domain::PID& pid) const override;

private:
    QString m_proc_root;
};

}
