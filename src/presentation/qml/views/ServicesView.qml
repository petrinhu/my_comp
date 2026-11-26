import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 3 - Serviços systemd: lista com iniciar/pausar/parar. "Criar Daemon" abre o diálogo global (Main.qml).
ScrollPage {
    id: servicesView
    signal createDaemonRequested()

    RowLayout {
        PageTitle { text: qsTr("Serviços Systemd") }
        Item { Layout.fillWidth: true }
        Button { text: qsTr("🆕 Criar Daemon"); highlighted: true; onClicked: servicesView.createDaemonRequested() }
        Button { text: "🔄"; onClicked: serviceViewModel.refresh() }
    }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(600, serviceList.contentHeight + 20)
        ListView {
            id: serviceList
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            model: serviceViewModel.services
            delegate: ItemDelegate {
                width: ListView.view.width; height: 60
                onDoubleClicked: (modelData.activeState === "active") ? serviceViewModel.pauseService(modelData.name) : serviceViewModel.startService(modelData.name)
                contentItem: RowLayout {
                    spacing: 15
                    StatusDot { size: 10; active: modelData.activeState === "active" }
                    ColumnLayout { spacing: 2; Layout.fillWidth: true
                        Text { text: modelData.name; color: Theme.textColor; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                        Text { text: modelData.description; color: Theme.mutedTextColor; font.pixelSize: 10; elide: Text.ElideRight; Layout.fillWidth: true }
                    }
                    RowLayout {
                        Button { text: "▶️"; flat: true; visible: modelData.activeState !== "active"; onClicked: serviceViewModel.startService(modelData.name) }
                        Button { text: "⏸️"; flat: true; visible: modelData.activeState === "active"; onClicked: serviceViewModel.pauseService(modelData.name) }
                        Button { text: "⏹️"; flat: true; onClicked: serviceViewModel.stopService(modelData.name) }
                    }
                }
            }
        }
        BusyIndicator { anchors.centerIn: parent; running: serviceViewModel.isBusy }
    }
}
