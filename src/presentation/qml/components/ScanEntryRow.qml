import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Linha do ranking do scanner de diretório: nome, barra proporcional, tamanho e contagens.
RowLayout {
    id: scanEntryRow
    // Entrada de directoryScannerViewModel.topEntries.
    property var entry: ({})
    Layout.fillWidth: true
    Text {
        text: scanEntryRow.entry.error ? "⚠ " + scanEntryRow.entry.name : scanEntryRow.entry.name
        color: scanEntryRow.entry.error ? Theme.criticalColor : Theme.textColor
        font.pixelSize: 11; font.bold: !scanEntryRow.entry.error
        Layout.preferredWidth: 200; elide: Text.ElideMiddle
    }
    ProgressBar {
        Layout.fillWidth: true
        from: 0
        to: directoryScannerViewModel.totalSize > 0 ? directoryScannerViewModel.totalSize : 1
        value: scanEntryRow.entry.sizeBytes
        background: Rectangle { color: Theme.progressTrackColor; radius: 3; implicitHeight: 6 }
        contentItem: Item {
            Rectangle {
                width: parent.width * (directoryScannerViewModel.totalSize > 0 ? (scanEntryRow.entry.sizeBytes / directoryScannerViewModel.totalSize) : 0)
                height: parent.height
                radius: 3; color: Theme.highlightColor
            }
        }
    }
    Text {
        text: Format.formatBytes(scanEntryRow.entry.sizeBytes)
        color: Theme.textColor; font.pixelSize: 10
        Layout.preferredWidth: 90; horizontalAlignment: Text.AlignRight
    }
    Text {
        text: qsTr("%1 arq · %2 dir").arg(scanEntryRow.entry.fileCount).arg(scanEntryRow.entry.dirCount)
        color: Theme.mutedTextColor; font.pixelSize: 10 // sem opacity: texto translúcido perde contraste (WCAG)
        Layout.preferredWidth: 120
    }
}
