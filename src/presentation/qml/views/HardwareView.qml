import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 5 - Hardware: SMBIOS, USB, Bluetooth, GPU e firmware (fwupd).
ScrollPage {
    id: hardwareView

    RowLayout {
        Layout.fillWidth: true
        PageTitle { text: qsTr("Hardware") }
        Item { Layout.fillWidth: true }
        BusyIndicator { running: firmwareViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
        IconButton {
            text: "🔄"; label: qsTr("Atualizar hardware")
            enabled: !firmwareViewModel.isBusy
            onClicked: { hardwareViewModel.refresh(); firmwareViewModel.refresh() }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true; spacing: 20

        SectionTitle { text: qsTr("🖥️ SISTEMA (SMBIOS)") }
        Card {
            Layout.fillWidth: true; Layout.preferredHeight: smbiosGrid.implicitHeight + 20
            SmbiosGrid { id: smbiosGrid; anchors.fill: parent; anchors.margins: 12 }
        }

        SectionTitle { text: qsTr("🔌 USB") }
        Card {
            Layout.fillWidth: true; Layout.preferredHeight: 180
            ListView {
                id: usbList
                anchors.fill: parent; anchors.margins: 10; clip: true
                model: hardwareViewModel.usbDevices
                ScrollBar.vertical: ScrollBar {}
                delegate: RowLayout {
                    width: usbList.width; height: 28; spacing: 12
                    Text { textFormat: Text.PlainText; text: (modelData.vendorId || "") + ":" + (modelData.productId || ""); color: Theme.mutedTextColor; font.family: "Monospace"; font.pixelSize: 11; Layout.preferredWidth: 110 }
                    Text { textFormat: Text.PlainText; text: modelData.name || qsTr("(sem nome)"); color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                    Text { textFormat: Text.PlainText; text: qsTr("bus %1").arg(modelData.bus || "—"); color: Theme.mutedTextColor; font.pixelSize: 11 }
                }
            }
        }

        SectionTitle { text: qsTr("📡 BLUETOOTH") }
        Card {
            Layout.fillWidth: true; Layout.preferredHeight: 140
            ListView {
                id: btList
                anchors.fill: parent; anchors.margins: 10; clip: true
                model: hardwareViewModel.bluetoothDevices
                ScrollBar.vertical: ScrollBar {}
                delegate: RowLayout {
                    width: btList.width; height: 28; spacing: 12
                    StatusDot { size: 8; active: modelData.connected }
                    Text { textFormat: Text.PlainText; text: modelData.name || qsTr("(sem nome)"); color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                    Text {
                        textFormat: Text.PlainText
                        text: modelData.connected ? qsTr("conectado") : (modelData.paired ? qsTr("pareado") : qsTr("não pareado"))
                        color: Theme.mutedTextColor; font.pixelSize: 11
                    }
                    Text { textFormat: Text.PlainText; text: modelData.address || ""; color: Theme.mutedTextColor; font.family: "Monospace"; font.pixelSize: 11 }
                }
            }
        }

        SectionTitle { text: qsTr("🎮 GPU") }
        Card {
            Layout.fillWidth: true; Layout.preferredHeight: 160
            ListView {
                id: gpuList
                anchors.fill: parent; anchors.margins: 10; clip: true
                model: gpuViewModel.gpus
                ScrollBar.vertical: ScrollBar {}
                delegate: GpuRow { width: gpuList.width; gpu: modelData }
            }
        }

        SectionTitle { text: qsTr("🔧 FIRMWARE (fwupd)") }
        Card {
            Layout.fillWidth: true; Layout.preferredHeight: 140
            ListView {
                id: fwList
                anchors.fill: parent; anchors.margins: 10; clip: true
                model: firmwareViewModel.devices
                ScrollBar.vertical: ScrollBar {}
                delegate: Text {
                    textFormat: Text.PlainText
                    width: fwList.width; height: 28
                    text: modelData.name; color: Theme.textColor
                    verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
                }
            }
        }
    }
}
