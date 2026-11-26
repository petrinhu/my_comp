import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// ABA 11 - Instalação: console do instalador e confirmação interativa.

ColumnLayout {
    spacing: 10
    
    // Card de Informações do Sistema
    Rectangle {
        Layout.fillWidth: true
        height: 80
        color: Theme.consolePanelBg
        radius: 8
        RowLayout {
            anchors.fill: parent; anchors.margins: 15
            Column { 
                Text { text: qsTr("Sistema"); color: Theme.consoleMutedTextColor; font.pixelSize: 12 }
                Text { textFormat: Text.PlainText; text: systemInfoViewModel.distro; color: Theme.contrastTextColor; font.bold: true }
            }
            Column { 
                Text { text: qsTr("Kernel"); color: Theme.consoleMutedTextColor; font.pixelSize: 12 }
                Text { textFormat: Text.PlainText; text: systemInfoViewModel.kernel; color: Theme.contrastTextColor; font.bold: true }
            }
            Column { 
                Text { text: qsTr("Driver GPU"); color: Theme.consoleMutedTextColor; font.pixelSize: 12 }
                Text { textFormat: Text.PlainText; text: systemInfoViewModel.driver; color: Theme.contrastTextColor; font.bold: true }
            }
        }
    }

    // Log Terminal
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: Theme.consoleBg
        radius: 4
        ScrollView {
            anchors.fill: parent
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            TextArea {
                id: logArea
                text: installationViewModel.logOutput
                readOnly: true
                color: Theme.consoleTextColor
                font.family: "Monospace"
                font.pixelSize: 12
                onTextChanged: cursorPosition = length
            }
        }
    }

    // Modal de Interação
    Popup {
        id: interactionPopup
        visible: installationViewModel.isWaiting
        modal: true
        closePolicy: Popup.NoAutoClose
        anchors.centerIn: parent
        padding: 20
        background: Rectangle { color: Theme.consolePanelBg; radius: 8 }
        
        ColumnLayout {
            spacing: 10
            Text { text: qsTr("Confirmação necessária:"); color: Theme.contrastTextColor; font.bold: true }
            Text { textFormat: Text.PlainText; text: installationViewModel.question; color: Theme.consoleMutedTextColor; wrapMode: Text.WordWrap; width: 300 }
            Row {
                spacing: 10
                Button { text: qsTr("Sim (y)"); onClicked: installationViewModel.respond(true) }
                Button { text: qsTr("Não (n)"); onClicked: installationViewModel.respond(false) }
            }
        }
    }
}
