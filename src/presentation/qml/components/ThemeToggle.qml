import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Botão que alterna o tema: claro → escuro → automático (grava em Theme.themeMode).
Button {
    id: themeToggle
    Layout.fillWidth: true
    flat: true
    onClicked: Theme.themeMode = (Theme.themeMode + 1) % 3
    background: Rectangle {
        color: themeToggle.pressed ? Theme.pressedColor :
               themeToggle.hovered ? Theme.hoverColor :
               Theme.subtleBg
        radius: 6
    }
    contentItem: RowLayout {
        spacing: 10
        Text { text: Theme.themeMode === 0 ? "☀️" : Theme.themeMode === 1 ? "🌙" : "🌓"; color: Theme.textColor }
        Text { text: Theme.themeMode === 0 ? qsTr("Light") : Theme.themeMode === 1 ? qsTr("Dark") : qsTr("Auto"); color: Theme.textColor; font.bold: true }
    }
}
