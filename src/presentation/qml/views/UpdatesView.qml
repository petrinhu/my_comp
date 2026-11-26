import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 1 - Central de Atualizações. Instalar/atualizar pede navegação para a aba 11 (Instalação).
ScrollPage {
    id: updatesView
    signal navigateRequested(int index)

    RowLayout {
        Layout.fillWidth: true
        PageTitle { text: qsTr("Central de Atualizações") }
        Rectangle {
            visible: updateViewModel.totalUpdates > 0
            Layout.preferredHeight: 22
            Layout.preferredWidth: Math.max(32, countText.implicitWidth + 16)
            radius: 11
            color: Theme.highlightColor
            Text {
                id: countText
                anchors.centerIn: parent
                text: updateViewModel.totalUpdates
                color: Theme.contrastTextColor
                font.bold: true
                font.pixelSize: 12
            }
        }
        Item { Layout.fillWidth: true }
        Button {
            text: qsTr("Atualizar Tudo")
            highlighted: true
            onClicked: {
                installationViewModel.startUpgradeAll()
                updatesView.navigateRequested(11) // ABA de Instalação
            }
        }
        BusyIndicator { running: updateViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
        Button { text: "🔄"; onClicked: updateViewModel.refresh(); enabled: !updateViewModel.isBusy }
    }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(600, updateListView.contentHeight + 20)
        ListView {
            id: updateListView
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            model: updateViewModel.updateModel
            delegate: ItemDelegate {
                width: ListView.view.width; height: 50
                onDoubleClicked: {
                    installationViewModel.startInstallation(name, provider)
                    updatesView.navigateRequested(11)
                }
                background: Rectangle {
                    color: isCritical ? Qt.rgba(Theme.criticalColor.r, Theme.criticalColor.g, Theme.criticalColor.b, 0.08) : "transparent"
                    border.color: isCritical ? Theme.criticalColor : "transparent"
                    border.width: isCritical ? 1 : 0
                    radius: 4
                }
                contentItem: RowLayout {
                    Text {
                        text: isCritical ? "⚠" : ""
                        color: Theme.criticalColor
                        font.pixelSize: 16
                        Layout.preferredWidth: 20
                        visible: isCritical
                    }
                    Text { text: name; color: Theme.textColor; font.bold: true; Layout.fillWidth: true }
                    Text { text: source; color: Theme.mutedTextColor; font.pixelSize: 10; Layout.preferredWidth: 60 }
                    Text {
                        text: (currentVersion && currentVersion.length > 0)
                              ? (currentVersion + " → " + newVersion)
                              : ("→ " + newVersion)
                        color: Theme.highlightColor
                        font.family: "Monospace"
                        font.pixelSize: 12
                    }
                }
            }
        }
    }
}
