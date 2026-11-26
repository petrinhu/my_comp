pragma Singleton
import QtQuick

// Paleta central do my_comp (Breeze Dark/Light) + estado do tema claro/escuro/automático.
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
    readonly property color highlightColor: isDark ? "#3daee9" : "#1b6acb"
    readonly property color criticalColor: isDark ? "#ff4d4d" : "#c0392b"

    // --- Paleta semântica ---
    readonly property color mutedTextColor: "#7f8c8d"        // texto secundário/legendas
    readonly property color successColor: "#2ecc71"          // OK, ativo, saudável
    readonly property color successStrongColor: "#27ae60"    // candidato seguro (VRAM: CPU basta)
    readonly property color warningColor: "#f39c12"          // atenção/inconclusivo
    readonly property color errorColor: "#e74c3c"            // erro (journal err, toast, precisa GPU)
    readonly property color infoColor: "#3498db"             // toast informativo
    readonly property color inactiveColor: "#95a5a6"         // indicador inativo/desligado
    readonly property color contrastTextColor: "#ffffff"     // texto sobre fundo colorido/escuro fixo
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
