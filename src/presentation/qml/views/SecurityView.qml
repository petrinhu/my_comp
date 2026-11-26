import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 7 - Segurança: histórico de logins (userMetadataViewModel.loginHistory).
ScrollPage {
    id: securityView

    PageTitle { text: qsTr("Segurança") }
    Card {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(600, loginHistoryList.contentHeight + 20)
        ListView {
            id: loginHistoryList
            anchors.fill: parent; anchors.margins: 10; clip: true
            interactive: false
            model: userMetadataViewModel.loginHistory
            delegate: Text { text: modelData; color: Theme.textColor; font.family: "Monospace"; font.pixelSize: 10 }
        }
    }
}
