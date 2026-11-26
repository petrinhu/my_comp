#include "proc_cmdline.h"

#include <QStringList>

namespace my_comp::infrastructure::collectors {

QString format_cmdline(const std::string& raw, const QString& fallback_comm) {
    QStringList args;
    std::string::size_type inicio = 0;
    while (inicio < raw.size()) {
        auto fim = raw.find('\0', inicio);
        if (fim == std::string::npos) {
            fim = raw.size();
        }
        if (fim > inicio) {
            args << QString::fromUtf8(raw.data() + inicio, static_cast<qsizetype>(fim - inicio));
        }
        inicio = fim + 1;
    }
    if (args.isEmpty()) {
        return fallback_comm;
    }
    return args.join(QLatin1Char(' '));
}

} // namespace my_comp::infrastructure::collectors
