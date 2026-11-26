import QtQuick
import QtQuick.Layouts
import ".."

// Origem de configuração sugerida para um processo (chave atual → sugerida + instrução).
Rectangle {
    id: configOrigin
    // Entrada de item.origensConfig.
    property var origem: ({})
    Layout.fillWidth: true
    Layout.leftMargin: 18
    Layout.topMargin: 4
    color: Theme.insetBg
    radius: 4
    implicitHeight: origCol.implicitHeight + 8

    ColumnLayout {
        id: origCol
        anchors.fill: parent
        anchors.margins: 6
        spacing: 2
        Text {
            text: qsTr("[%1] %2 = %3  →  sugerido: %4  (%5)")
                  .arg(configOrigin.origem.categoria).arg(configOrigin.origem.chave)
                  .arg(configOrigin.origem.valorAtual).arg(configOrigin.origem.valorSugerido)
                  .arg(configOrigin.origem.confianca)
            color: Theme.textColor
            font.family: "monospace"
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Text {
            textFormat: Text.PlainText
            text: configOrigin.origem.instrucao
            color: Theme.mutedTextColor
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
