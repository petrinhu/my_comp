import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 3 - Serviços systemd: habilitar+iniciar / parar (sessão) / desabilitar+parar. "Criar Daemon" abre o diálogo global (Main.qml).
ScrollPage {
    id: servicesView
    signal createDaemonRequested()

    RowLayout {
        PageTitle { text: qsTr("Serviços Systemd") }
        Item { Layout.fillWidth: true }
        Button { text: qsTr("🆕 Criar Daemon"); highlighted: true; onClicked: servicesView.createDaemonRequested() }
        IconButton { text: "🔄"; label: qsTr("Atualizar serviços"); onClicked: serviceViewModel.refresh() }
    }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(600, serviceList.contentHeight + 20)
        ListView {
            id: serviceList
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            // QA-05: Tab foca a lista, setas navegam, Enter alterna (igual ao duplo-clique).
            activeFocusOnTab: true; keyNavigationEnabled: true
            model: serviceViewModel.services
            delegate: ItemDelegate {
                width: ListView.view.width; height: 60
                focus: ListView.isCurrentItem
                highlighted: ListView.isCurrentItem && ListView.view.activeFocus
                Keys.onReturnPressed: (modelData.activeState === "active") ? serviceViewModel.stopNow(modelData.name, modelData.scope) : serviceViewModel.enableAndStart(modelData.name, modelData.scope)
                onDoubleClicked: (modelData.activeState === "active") ? serviceViewModel.stopNow(modelData.name, modelData.scope) : serviceViewModel.enableAndStart(modelData.name, modelData.scope)
                contentItem: RowLayout {
                    spacing: 15
                    StatusDot { size: 10; active: modelData.activeState === "active" }
                    ColumnLayout { spacing: 2; Layout.fillWidth: true
                        Text { text: modelData.scope === "user" ? qsTr("%1 (usuário)").arg(modelData.name) : modelData.name; color: Theme.textColor; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                        Text {
                            text: modelData.isEnabled ? qsTr("%1 · inicia no boot").arg(modelData.description) : modelData.description
                            color: Theme.mutedTextColor; font.pixelSize: 10; elide: Text.ElideRight; Layout.fillWidth: true
                        }
                    }
                    RowLayout {
                        IconButton {
                            text: "▶️"; flat: true; visible: modelData.activeState !== "active"
                            label: qsTr("Habilitar %1 no boot e iniciar (enable --now)").arg(modelData.name)
                            onClicked: serviceViewModel.enableAndStart(modelData.name, modelData.scope)
                        }
                        IconButton {
                            text: "⏸️"; flat: true; visible: modelData.activeState === "active"
                            label: qsTr("Parar %1 só nesta sessão (stop)").arg(modelData.name)
                            onClicked: serviceViewModel.stopNow(modelData.name, modelData.scope)
                        }
                        IconButton {
                            text: "⏹️"; flat: true
                            label: qsTr("Desabilitar %1 no boot e parar (disable --now)").arg(modelData.name)
                            onClicked: serviceViewModel.disableAndStop(modelData.name, modelData.scope)
                        }
                    }
                }
            }
        }
        BusyIndicator { anchors.centerIn: parent; running: serviceViewModel.isBusy }
    }
}
