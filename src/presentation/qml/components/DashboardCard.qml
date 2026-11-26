import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Cartão clicável com título e valor grande (Dashboard e Rede).
Rectangle {
    id: card
    property string title
    property string value
    signal clicked

    Layout.fillWidth: true; Layout.preferredHeight: 100; radius: 10
    color: ma.pressed ? Theme.hoverColor : Theme.cardColor
    // QA-05: focável por Tab, Enter/Espaço abrem o detalhe; borda destaca o foco.
    activeFocusOnTab: true
    border.color: activeFocus ? Theme.highlightColor : Theme.borderColor
    border.width: activeFocus ? 2 : 1
    Accessible.role: Accessible.Button
    Accessible.name: qsTr("%1: %2").arg(card.title).arg(card.value)
    Keys.onReturnPressed: card.clicked()
    Keys.onEnterPressed: card.clicked()
    Keys.onSpacePressed: card.clicked()
    scale: ma.pressed ? 0.98 : 1.0
    Behavior on scale { NumberAnimation { duration: 50 } }
    Behavior on color { ColorAnimation { duration: 150 } }

    MouseArea {
        id: ma; anchors.fill: parent; onClicked: card.clicked(); hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        ToolTip.visible: containsMouse
        ToolTip.text: qsTr("Clique para detalhes de %1").arg(card.title)
    }
    ColumnLayout {
        anchors.centerIn: parent
        Text { textFormat: Text.PlainText; text: card.title; color: Theme.textColor; font.pixelSize: 12; Layout.alignment: Qt.AlignHCenter }
        Text { textFormat: Text.PlainText; text: card.value; color: Theme.highlightColor; font.pixelSize: 24; font.bold: true; Layout.alignment: Qt.AlignHCenter }
    }
}
