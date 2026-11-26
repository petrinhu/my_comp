import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 6 - Desenvolvimento & Containers: toolchains detectados e containers podman/docker.
ScrollPage {
    id: devToolsView

    RowLayout {
        Layout.fillWidth: true
        PageTitle { text: qsTr("Desenvolvimento & Containers") }
        Item { Layout.fillWidth: true }
        BusyIndicator { running: devStackViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
        Button { text: "🔄"; onClicked: devStackViewModel.refresh(); enabled: !devStackViewModel.isBusy }
    }

    SectionTitle { text: qsTr("🛠️ COMPILADORES & RUNTIMES") }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(200, toolchainList.contentHeight + 20)
        ListView {
            id: toolchainList
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            model: devStackViewModel.toolchains
            delegate: RowLayout {
                width: ListView.view.width; height: 30
                Text { text: modelData.name; font.bold: true; color: Theme.textColor; Layout.preferredWidth: 100 }
                Text { text: modelData.version; color: Theme.mutedTextColor; Layout.fillWidth: true; elide: Text.ElideRight; font.family: "Monospace"; font.pixelSize: 11 }
            }
        }
    }

    SectionTitle { text: qsTr("🐳 CONTAINERS (PODMAN/DOCKER)") }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(300, containerList.contentHeight + 20)
        ListView {
            id: containerList
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            model: devStackViewModel.containers
            delegate: ItemDelegate {
                width: ListView.view.width; height: 50
                contentItem: RowLayout {
                    spacing: 15
                    StatusDot { size: 8; active: modelData.status.includes("Up") }
                    ColumnLayout {
                        spacing: 2
                        Text { text: modelData.name; color: Theme.textColor; font.bold: true }
                        Text { text: modelData.runtime + " • " + modelData.image; color: Theme.mutedTextColor; font.pixelSize: 10 }
                    }
                    Item { Layout.fillWidth: true }
                    Text { text: modelData.status; color: Theme.textColor; font.pixelSize: 11 }
                }
            }
            Text { anchors.centerIn: parent; text: qsTr("Nenhum container detectado."); color: Theme.mutedTextColor; visible: parent.count === 0 }
        }
    }
}
