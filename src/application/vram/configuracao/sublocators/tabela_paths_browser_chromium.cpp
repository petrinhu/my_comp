#include "application/vram/configuracao/sublocators/tabela_paths_browser_chromium.h"

namespace my_comp::application::vram {

const QHash<QString, QString>& tabela_paths_browser_chromium() {
    static const QHash<QString, QString> tabela = {
        {QStringLiteral("brave-browser"),        QStringLiteral(".config/BraveSoftware/Brave-Browser/Local State")},
        {QStringLiteral("brave"),                QStringLiteral(".config/BraveSoftware/Brave-Browser/Local State")},
        {QStringLiteral("chromium"),             QStringLiteral(".config/chromium/Local State")},
        {QStringLiteral("chromium-browser"),     QStringLiteral(".config/chromium/Local State")},
        {QStringLiteral("chrome"),               QStringLiteral(".config/google-chrome/Local State")},
        {QStringLiteral("google-chrome"),        QStringLiteral(".config/google-chrome/Local State")},
        {QStringLiteral("google-chrome-stable"), QStringLiteral(".config/google-chrome/Local State")},
        {QStringLiteral("vivaldi"),              QStringLiteral(".config/vivaldi/Local State")},
        {QStringLiteral("microsoft-edge"),       QStringLiteral(".config/microsoft-edge/Local State")},
        {QStringLiteral("microsoft-edge-stable"),QStringLiteral(".config/microsoft-edge/Local State")},
        {QStringLiteral("opera"),                QStringLiteral(".config/opera/Local State")},
    };
    return tabela;
}

}
