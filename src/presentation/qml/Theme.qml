pragma Singleton
import QtQuick

// Paleta central do my_comp (Breeze Dark/Light) + estado do tema claro/escuro/automático.
// Cores semânticas têm variante por tema para passar WCAG AA (>= 4.5:1) sobre todas as
// superfícies; scripts/check_contrast.py é o gate (QA-05, MC-07 A3).
// Único lugar com literais de cor; os demais arquivos QML referenciam Theme.<nome> (MC-05).
// Registrado como singleton em ./qmldir (mesmo diretório do qrc).
QtObject {
    // --- Estado do tema ---
    // 0 = claro, 1 = escuro, 2 = automático (segue o sistema). Gravável: o toggle da sidebar escreve aqui.
    property int themeMode: 2
    // Preferência do sistema; Main.qml injeta a context property isSystemDarkInitial (via Binding).
    property bool systemThemeIsDark: false
    readonly property bool isDark: themeMode === 1 || (themeMode === 2 && systemThemeIsDark)

    // --- Bases claro/escuro ---
    readonly property color lightBg: "#eff0f1"
    readonly property color lightSidebar: "#fcfcfc"
    readonly property color lightCard: "#ffffff"
    readonly property color lightText: "#232629"
    readonly property color lightBorder: "#bcbebf"

    readonly property color darkBg: "#232629"
    readonly property color darkSidebar: "#31363b"
    readonly property color darkCard: "#31363b"
    readonly property color darkText: "#eff0f1"
    readonly property color darkBorder: "#4d4d4d"

    // --- Cores derivadas do tema ativo ---
    readonly property color windowColor: isDark ? darkBg : lightBg
    readonly property color textColor: isDark ? darkText : lightText
    readonly property color sidebarColor: isDark ? darkSidebar : lightSidebar
    readonly property color cardColor: isDark ? darkCard : lightCard
    readonly property color borderColor: isDark ? darkBorder : lightBorder
    readonly property color highlightColor: isDark ? "#62bded" : "#1a65c2"
    // Opacidade do fundo do item ativo da navegação (lido também por scripts/check_contrast.py).
    readonly property real navActiveOpacity: 0.12
    readonly property color criticalColor: isDark ? "#ff9494" : "#b03428"

    // --- Paleta semântica ---
    readonly property color mutedTextColor: isDark ? "#aab3b3" : "#5e6969"        // texto secundário/legendas
    readonly property color successColor: isDark ? "#2ecc71" : "#1a7541"          // OK, ativo, saudável
    readonly property color successStrongColor: isDark ? "#58d68d" : "#15603a"    // candidato seguro (VRAM: CPU basta)
    readonly property color warningColor: isDark ? "#f5b041" : "#905b07"          // atenção/inconclusivo
    readonly property color errorColor: isDark ? "#f29b92" : "#c42818"            // erro (journal err, precisa GPU)
    readonly property color infoColor: isDark ? "#75b9e7" : "#1d6ca1"             // informativo
    readonly property color inactiveColor: isDark ? "#a7b4b5" : "#5a6a6b"         // indicador inativo/desligado
    readonly property color contrastTextColor: "#ffffff"     // texto sobre fundo colorido/escuro fixo
    // Fundos dos toasts: escuros o bastante para texto branco >= 4.5:1 (WCAG AA, QA-05).
    readonly property color toastSuccessBg: "#1e854a"
    readonly property color toastErrorBg: "#c42818"
    readonly property color toastInfoBg: "#207ab7"
    readonly property color hoverColor: isDark ? "#3e444a" : "#e8e8e8"
    readonly property color pressedColor: isDark ? "#454a4f" : "#d0d0d0"
    readonly property color subtleBg: isDark ? "#2a2e32" : "#f5f5f5"           // botões discretos, cabeçalhos de tabela
    readonly property color insetBg: isDark ? "#2b3036" : "#f0f2f5"            // sub-cartões aninhados
    readonly property color progressTrackColor: isDark ? "#2a2e32" : "#e0e0e0" // trilho de ProgressBar
    readonly property color coreLoadTrackColor: isDark ? "#3e444a" : "#e0e0e0" // trilho da carga por core
    readonly property color dialogBg: isDark ? "#2a2e32" : "#ffffff"
    readonly property color dialogHeaderBg: isDark ? "#1a1c1e" : "#f8f9fa"
    readonly property color dialogSectionBg: isDark ? "#1a1c1e" : "#fdfdfd"
    // Console de instalação: esquema escuro fixo, independente de isDark.
    readonly property color consoleBg: "#1e1e1e"
    readonly property color consolePanelBg: "#2c3e50"
    readonly property color consoleTextColor: "#dcdcdc"
    readonly property color consoleMutedTextColor: "#bdc3c7"
}
