import QtQuick
import QtQuick.Layouts
import ".."

// Cabeçalho clicável do explorador de processos (dispara processViewModel.setSortField
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
        Item { Layout.preferredWidth: 60; Layout.fillHeight: true
            Text { anchors.verticalCenter: parent.verticalCenter; text: qsTr("PID") + processSortHeader.arrow("pid"); font.bold: true; color: Theme.textColor }
            MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: processSortHeader.applySort("pid") }
        }
        Item { Layout.preferredWidth: 100; Layout.fillHeight: true
            Text { anchors.verticalCenter: parent.verticalCenter; text: qsTr("USER") + processSortHeader.arrow("user"); font.bold: true; color: Theme.textColor }
            MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: processSortHeader.applySort("user") }
        }
        Item { Layout.preferredWidth: 60; Layout.fillHeight: true
            Text { anchors.verticalCenter: parent.verticalCenter; text: qsTr("CPU%") + processSortHeader.arrow("cpu"); font.bold: true; color: Theme.textColor }
            MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: processSortHeader.applySort("cpu") }
        }
        Item { Layout.preferredWidth: 60; Layout.fillHeight: true
            Text { anchors.verticalCenter: parent.verticalCenter; text: qsTr("MEM%") + processSortHeader.arrow("mem"); font.bold: true; color: Theme.textColor }
            MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: processSortHeader.applySort("mem") }
        }
        Text { text: qsTr("STATE"); font.bold: true; Layout.preferredWidth: 50; color: Theme.textColor }
        Text { text: qsTr("COMMAND"); font.bold: true; Layout.fillWidth: true; color: Theme.textColor }
    }
}
