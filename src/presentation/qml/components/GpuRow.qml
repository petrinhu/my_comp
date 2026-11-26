import QtQuick
import QtQuick.Layouts
import ".."

// Linha da lista de GPUs na aba Hardware (fabricante/modelo, temperatura, carga e VRAM).
ColumnLayout {
    id: gpuRow
    // Entrada de gpuViewModel.gpus.
    property var gpu: ({})
    spacing: 2
    RowLayout {
        Layout.fillWidth: true; spacing: 10
        Text { text: (gpuRow.gpu.vendor || "—") + " · " + (gpuRow.gpu.modelName || "—"); color: Theme.textColor; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
        Text { text: (gpuRow.gpu.temp > 0 ? gpuRow.gpu.temp.toFixed(0) + " °C" : "—"); color: Theme.mutedTextColor; font.family: "Monospace"; font.pixelSize: 11 }
    }
    RowLayout {
        Layout.fillWidth: true; spacing: 10
        Text { text: qsTr("Carga:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
        Text { text: gpuRow.gpu.load.toFixed(0) + " %"; color: Theme.textColor; font.pixelSize: 11; Layout.preferredWidth: 60 }
        Text { text: qsTr("VRAM:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
        Text {
            text: (gpuRow.gpu.vramTotal > 0)
                ? (gpuRow.gpu.vramUsed.toFixed(0) + " / " + gpuRow.gpu.vramTotal.toFixed(0) + " MiB (" + gpuRow.gpu.vramPercent.toFixed(0) + " %)")
                : "—"
            color: Theme.textColor; font.pixelSize: 11; Layout.fillWidth: true; elide: Text.ElideRight
        }
    }
    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.borderColor; opacity: 0.4 }
}
