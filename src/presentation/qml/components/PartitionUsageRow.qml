import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Partição de um disco: nome/ponto de montagem, filesystem e barra de uso.
ColumnLayout {
    id: partitionRow
    // Entrada de drive.partitions.
    property var partition: ({})
    Layout.fillWidth: true; spacing: 2

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: partitionRow.partition.mount.length > 0 ? partitionRow.partition.name + "  @  " + partitionRow.partition.mount : qsTr("%1  (não montado)").arg(partitionRow.partition.name)
            color: Theme.textColor; font.pixelSize: 12; font.bold: true
        }
        Item { Layout.fillWidth: true }
        Text {
            text: partitionRow.partition.fs.length > 0 ? partitionRow.partition.fs : "—"
            color: Theme.textColor; opacity: 0.6; font.pixelSize: 11
        }
    }
    RowLayout {
        Layout.fillWidth: true
        ProgressBar {
            Layout.fillWidth: true
            from: 0; to: 100; value: partitionRow.partition.usedPercent
            background: Rectangle { color: Theme.progressTrackColor; radius: 4; implicitHeight: 8 }
            contentItem: Item {
                Rectangle {
                    width: parent.width * (partitionRow.partition.usedPercent / 100.0)
                    height: parent.height
                    radius: 4
                    color: partitionRow.partition.usedPercent >= 90 ? Theme.criticalColor
                         : partitionRow.partition.usedPercent >= 70 ? Theme.warningColor
                         : Theme.successColor
                }
            }
        }
        Text {
            text: partitionRow.partition.mount.length > 0
                ? Format.formatBytes(partitionRow.partition.usedBytes) + " / " + Format.formatBytes(partitionRow.partition.sizeBytes) + "  (" + partitionRow.partition.usedPercent.toFixed(1) + "%)"
                : Format.formatBytes(partitionRow.partition.sizeBytes)
            color: Theme.textColor; opacity: 0.75; font.pixelSize: 11
        }
    }
}
