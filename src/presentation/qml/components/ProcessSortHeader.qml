import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Cabeçalho do explorador de processos (botões: Tab/Enter ordenam, QA-05) (dispara processViewModel.setSortField
// via sortField/sortAscending). Funções chamadas qualificadas por id: lookup não-qualificado
// do QML não alcança objetos intermediários.
Rectangle {
    id: processSortHeader
    Layout.fillWidth: true; Layout.fillHeight: true; color: Theme.subtleBg; border.color: Theme.borderColor

    function applySort(field) {
        if (processViewModel.sortField === field) {
            processViewModel.sortAscending = !processViewModel.sortAscending
        } else {
            processViewModel.sortField = field
            processViewModel.sortAscending = false
        }
        processViewModel.refresh()
    }
    function arrow(field) {
        return processViewModel.sortField === field
            ? (processViewModel.sortAscending ? " ▲" : " ▼") : ""
    }

    RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 0
        IconButton {
            Layout.preferredWidth: 60; Layout.fillHeight: true; flat: true; padding: 0
            label: qsTr("Ordenar por %1").arg(qsTr("PID"))
            onClicked: processSortHeader.applySort("pid")
            contentItem: Text { text: qsTr("PID") + processSortHeader.arrow("pid"); font.bold: true; color: Theme.textColor; verticalAlignment: Text.AlignVCenter }
        }
        IconButton {
            Layout.preferredWidth: 100; Layout.fillHeight: true; flat: true; padding: 0
            label: qsTr("Ordenar por %1").arg(qsTr("USER"))
            onClicked: processSortHeader.applySort("user")
            contentItem: Text { text: qsTr("USER") + processSortHeader.arrow("user"); font.bold: true; color: Theme.textColor; verticalAlignment: Text.AlignVCenter }
        }
        IconButton {
            Layout.preferredWidth: 60; Layout.fillHeight: true; flat: true; padding: 0
            label: qsTr("Ordenar por %1").arg(qsTr("CPU%"))
            onClicked: processSortHeader.applySort("cpu")
            contentItem: Text { text: qsTr("CPU%") + processSortHeader.arrow("cpu"); font.bold: true; color: Theme.textColor; verticalAlignment: Text.AlignVCenter }
        }
        IconButton {
            Layout.preferredWidth: 60; Layout.fillHeight: true; flat: true; padding: 0
            label: qsTr("Ordenar por %1").arg(qsTr("MEM%"))
            onClicked: processSortHeader.applySort("mem")
            contentItem: Text { text: qsTr("MEM%") + processSortHeader.arrow("mem"); font.bold: true; color: Theme.textColor; verticalAlignment: Text.AlignVCenter }
        }
        Text { text: qsTr("STATE"); font.bold: true; Layout.preferredWidth: 50; color: Theme.textColor }
        Text { text: qsTr("COMMAND"); font.bold: true; Layout.fillWidth: true; color: Theme.textColor }
    }
}
