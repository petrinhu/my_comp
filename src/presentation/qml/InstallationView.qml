import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    spacing: 10
    
    // Card de Informações do Sistema
    Rectangle {
        Layout.fillWidth: true
        height: 80
        color: root.consolePanelBg
        radius: 8
        RowLayout {
            anchors.fill: parent; anchors.margins: 15
            Column { 
                Text { text: qsTr("Sistema"); color: root.consoleMutedTextColor; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.distro; color: root.contrastTextColor; font.bold: true }
            }
            Column { 
                Text { text: qsTr("Kernel"); color: root.consoleMutedTextColor; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.kernel; color: root.contrastTextColor; font.bold: true }
            }
            Column { 
                Text { text: qsTr("Driver GPU"); color: root.consoleMutedTextColor; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.driver; color: root.contrastTextColor; font.bold: true }
            }
        }
    }

    // Log Terminal
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: root.consoleBg
        radius: 4
        ScrollView {
            anchors.fill: parent
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
            TextArea {
                id: logArea
                text: installationViewModel.logOutput
                readOnly: true
                color: root.consoleTextColor
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
        background: Rectangle { color: root.consolePanelBg; radius: 8 }
        
        ColumnLayout {
            spacing: 10
            Text { text: qsTr("Confirmação necessária:"); color: root.contrastTextColor; font.bold: true }
            Text { text: installationViewModel.question; color: root.consoleMutedTextColor; wrapMode: Text.WordWrap; width: 300 }
            Row {
                spacing: 10
                Button { text: qsTr("Sim (y)"); onClicked: installationViewModel.respond(true) }
                Button { text: qsTr("Não (n)"); onClicked: installationViewModel.respond(false) }
            }
        }
    }
}
