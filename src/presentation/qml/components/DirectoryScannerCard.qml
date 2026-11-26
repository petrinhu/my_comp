import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Seção Scanner de Diretório (ST-05): varredura recursiva e top 20 subdiretórios.
Rectangle {
    Layout.fillWidth: true
    Layout.preferredHeight: scannerColumn.implicitHeight + 30
    color: Theme.cardColor; border.color: Theme.borderColor; radius: 8

    ColumnLayout {
        id: scannerColumn
        anchors.fill: parent; anchors.margins: 15; spacing: 10

        Text { text: qsTr("🔎 Scanner de Diretório"); color: Theme.highlightColor; font.pixelSize: 15; font.bold: true }
        Text {
            text: qsTr("Varre recursivamente uma pasta e lista os 20 subdiretórios mais pesados. Pseudo-filesystems (/proc, /sys, /run, /dev) e symlinks são ignorados.")
            color: Theme.textColor; opacity: 0.7; font.pixelSize: 10; wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: scanPathInput
                Layout.fillWidth: true
                placeholderText: "/home/" + Qt.application.arguments[0].split("/")[2]
                text: "/home"
            }
            Button {
                text: directoryScannerViewModel.isScanning ? qsTr("Cancelar") : qsTr("Varrer")
                onClicked: {
                    if (directoryScannerViewModel.isScanning) {
                        directoryScannerViewModel.cancel();
                    } else if (scanPathInput.text.length > 0) {
                        directoryScannerViewModel.scan(scanPathInput.text);
                    }
                }
            }
        }
        RowLayout {
            visible: directoryScannerViewModel.isScanning
            Layout.fillWidth: true
            BusyIndicator { running: true; implicitWidth: 20; implicitHeight: 20 }
            Text {
                text: qsTr("%1 arquivos · %2")
                    .arg(directoryScannerViewModel.filesScanned)
                    .arg(Format.formatBytes(directoryScannerViewModel.bytesScanned))
                color: Theme.textColor; font.pixelSize: 11
            }
            Text {
                text: "▸ " + directoryScannerViewModel.currentPath
                color: Theme.textColor; opacity: 0.6; font.pixelSize: 10
                elide: Text.ElideLeft; Layout.fillWidth: true
            }
        }
        Text {
            visible: !directoryScannerViewModel.isScanning && directoryScannerViewModel.scannedRoot.length > 0
            text: directoryScannerViewModel.scannedRoot + ": " + Format.formatBytes(directoryScannerViewModel.totalSize)
            color: Theme.textColor; font.pixelSize: 12; font.bold: true
        }
        Repeater {
            model: directoryScannerViewModel.topEntries
            delegate: ScanEntryRow { entry: modelData }
        }
    }
}
