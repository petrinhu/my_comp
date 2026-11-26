import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Explorador de processos (estilo htop): filtro, cabeçalho ordenável e lista.
// Duplo clique numa linha emite killRequested; Main.qml abre a confirmação.
ColumnLayout {
    id: processExplorer
    signal killRequested(var pid, var name)

    Layout.fillWidth: true
    Layout.preferredHeight: 400
    spacing: 10

    RowLayout {
        Text { text: qsTr("Explorador de Processos"); color: Theme.textColor; font.pixelSize: 18; font.bold: true }
        Item { Layout.fillWidth: true }
        TextField {
            placeholderText: qsTr("Filtrar (Nome/User)...")
            onTextChanged: processViewModel.filter = text
            Layout.preferredWidth: 200
        }
    }

    Card {
        Layout.fillWidth: true; Layout.fillHeight: true
        ColumnLayout {
            anchors.fill: parent; spacing: 0

            RowLayout {
                Layout.fillWidth: true; height: 30
                ProcessSortHeader { id: processSortHeader }
            }

            ScrollView {
                Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

                ListView {
                    id: processList
                    // QA-05: Tab foca a lista, setas navegam, Delete pede para encerrar.
                    activeFocusOnTab: true; keyNavigationEnabled: true
                    model: processViewModel.processes
                    delegate: ItemDelegate {
                        width: processList.width; height: 30
                        padding: 0
                        focus: ListView.isCurrentItem
                        highlighted: ListView.isCurrentItem && ListView.view.activeFocus
                        Keys.onDeletePressed: processExplorer.killRequested(modelData.pid, modelData.command)
                        contentItem: RowLayout {
                            anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                            Text { text: modelData.pid; Layout.preferredWidth: 60; color: Theme.highlightColor; font.family: "Monospace" }
                            Text { text: modelData.user; Layout.preferredWidth: 100; color: Theme.textColor; font.pixelSize: 11 }
                            Text { text: modelData.cpu.toFixed(1); Layout.preferredWidth: 60; color: modelData.cpu > 50 ? Theme.criticalColor : Theme.successColor; font.bold: true; font.family: "Monospace" }
                            Text { text: modelData.mem.toFixed(1); Layout.preferredWidth: 60; color: Theme.highlightColor; font.family: "Monospace" }
                            Text { text: modelData.state; Layout.preferredWidth: 50; color: modelData.state === "R" ? Theme.successColor : Theme.mutedTextColor; font.bold: true }
                            Text { text: modelData.command; Layout.fillWidth: true; color: Theme.textColor; elide: Text.ElideRight; font.family: "Monospace"; font.pixelSize: 11 }
                        }

                        onDoubleClicked: processExplorer.killRequested(modelData.pid, modelData.command)
                    }
                }
            }
        }
    }
}
