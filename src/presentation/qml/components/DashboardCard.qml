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
    border.color: Theme.borderColor
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
        Text { text: card.title; color: Theme.textColor; font.pixelSize: 12; Layout.alignment: Qt.AlignHCenter }
        Text { text: card.value; color: Theme.highlightColor; font.pixelSize: 24; font.bold: true; Layout.alignment: Qt.AlignHCenter }
    }
}
