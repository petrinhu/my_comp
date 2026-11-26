import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 9 - Storage: um cartão por disco (SMART, partições, benchmark) + scanner de diretório.
ScrollPage {
    id: storageView

    RowLayout {
        PageTitle { text: qsTr("Storage") }
        Item { Layout.fillWidth: true }
        IconButton { text: "🔄"; label: qsTr("Atualizar discos"); onClicked: storageViewModel.refreshInventory() }
    }

    Repeater {
        model: storageViewModel.drives
        delegate: DriveCard { drive: modelData }
    }

    DirectoryScannerCard {}

    Item { Layout.fillHeight: true }
}
