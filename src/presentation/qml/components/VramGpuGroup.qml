import QtQuick
import QtQuick.Layouts
import ".."

// Grupo de uma GPU na auditoria de VRAM: cabeçalho (modelo, VRAM total) + processos.
ColumnLayout {
    id: gpuGroup
    // Entrada de vramAuditViewModel.grupos.
    property var grupo: ({})
    Layout.fillWidth: true
    spacing: 6

    Rectangle {
        Layout.fillWidth: true
        color: Theme.cardColor
        border.color: Theme.borderColor
        radius: 8
        implicitHeight: header.implicitHeight + 18

        RowLayout {
            id: header
            anchors.fill: parent
            anchors.margins: 9
            spacing: 12
            Text {
                text: qsTr("GPU %1 — %2 · %3")
                      .arg(gpuGroup.grupo.gpuId).arg(gpuGroup.grupo.gpuFabricante).arg(gpuGroup.grupo.gpuModelo)
                color: Theme.textColor
                font.bold: true
                font.pixelSize: 14
                Layout.fillWidth: true
            }
            Text {
                text: gpuGroup.grupo.gpuVramTotalMb > 0
                      ? qsTr("VRAM total: %1 MiB").arg(gpuGroup.grupo.gpuVramTotalMb.toFixed(0))
                      : qsTr("VRAM total: —")
                color: Theme.highlightColor
                font.pixelSize: 12
                font.family: "monospace"
            }
        }
    }

    Repeater {
        model: gpuGroup.grupo.itens
        delegate: VramProcessItem { item: modelData }
    }

    Item { Layout.fillWidth: true; Layout.preferredHeight: 6 }
}
