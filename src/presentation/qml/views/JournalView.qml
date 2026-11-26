import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 2 - Analisador de Journal: filtro, busca (journalctl -g), limite e lista de entradas.
ScrollPage {
    id: journalView

    // 0-2 emerg/alert/crit, 3 err, 4 warn, 5 notice, 6 info, 7 debug
    function priorityColor(p) {
        if (p <= 2) return Theme.criticalColor
        if (p === 3) return Theme.errorColor
        if (p === 4) return Theme.warningColor
        if (p === 5) return Theme.highlightColor
        return Theme.textColor
    }
    function priorityLabel(p) {
        if (p === 0) return qsTr("EMERG")
        if (p === 1) return qsTr("ALERT")
        if (p === 2) return qsTr("CRIT")
        if (p === 3) return qsTr("ERR")
        if (p === 4) return qsTr("WARN")
        if (p === 5) return qsTr("NOTICE")
        if (p === 6) return qsTr("INFO")
        return qsTr("DEBUG")
    }

    RowLayout {
        PageTitle { text: qsTr("Analisador de Journal") }
        Item { Layout.fillWidth: true }
        BusyIndicator { running: diagnosticViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
        IconButton { text: "🔄"; label: qsTr("Atualizar journal"); onClicked: diagnosticViewModel.refresh(); enabled: !diagnosticViewModel.isBusy }
    }
    RowLayout {
        Layout.fillWidth: true; spacing: 10
        Text { text: qsTr("Filtro:"); color: Theme.textColor }
        ComboBox {
            id: filterCombo
            Layout.preferredWidth: 200
            model: [qsTr("Erros críticos"), qsTr("Hardware/Firmware"), qsTr("Boot"), qsTr("Desktop (KDE/Wayland)"), qsTr("Segurança/Rede"), qsTr("Todos")]
            currentIndex: diagnosticViewModel.currentFilter
            onActivated: diagnosticViewModel.currentFilter = currentIndex
        }
        Text { text: qsTr("Buscar:"); color: Theme.textColor }
        TextField {
            id: journalSearchField
            Layout.fillWidth: true
            placeholderText: qsTr("termo (journalctl -g)…")
            text: diagnosticViewModel.searchQuery
            onAccepted: {
                diagnosticViewModel.searchQuery = text
                diagnosticViewModel.performSearch()
            }
        }
        Button {
            text: qsTr("Buscar")
            onClicked: {
                diagnosticViewModel.searchQuery = journalSearchField.text
                diagnosticViewModel.performSearch()
            }
        }
        Text { text: qsTr("Limite:"); color: Theme.textColor }
        SpinBox {
            from: 50; to: 2000; stepSize: 50
            value: diagnosticViewModel.logLimit
            onValueModified: diagnosticViewModel.logLimit = value
        }
    }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(600, journalListView.contentHeight + 20)
        ListView {
            id: journalListView
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            // QA-05: Tab foca a lista, setas navegam, Enter pesquisa (igual ao duplo-clique).
            activeFocusOnTab: true; keyNavigationEnabled: true
            model: diagnosticViewModel.logModel
            delegate: ItemDelegate {
                width: ListView.view.width; height: 46
                focus: ListView.isCurrentItem
                highlighted: ListView.isCurrentItem && ListView.view.activeFocus
                onDoubleClicked: diagnosticViewModel.searchOnline(source + " " + message)
                Keys.onReturnPressed: diagnosticViewModel.searchOnline(source + " " + message)
                contentItem: RowLayout {
                    spacing: 8
                    Text {
                        text: timestamp
                        color: Theme.mutedTextColor
                        font.pixelSize: 10
                        font.family: "monospace"
                        Layout.preferredWidth: 110
                    }
                    Text {
                        text: journalView.priorityLabel(priority)
                        color: journalView.priorityColor(priority)
                        font.pixelSize: 10
                        font.bold: true
                        Layout.preferredWidth: 54
                    }
                    Text {
                        text: source
                        color: Theme.highlightColor
                        font.pixelSize: 10
                        elide: Text.ElideRight
                        Layout.preferredWidth: 110
                    }
                    Text {
                        text: message
                        color: Theme.textColor
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
