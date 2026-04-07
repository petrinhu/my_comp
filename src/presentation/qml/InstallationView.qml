import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    spacing: 10
    
    // Card de Informações do Sistema
    Rectangle {
        Layout.fillWidth: true
        height: 80
        color: "#2c3e50"
        radius: 8
        RowLayout {
            anchors.fill: parent; anchors.margins: 15
            Column { 
                Text { text: "Sistema"; color: "#bdc3c7"; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.distro; color: "white"; font.bold: true }
            }
            Column { 
                Text { text: "Kernel"; color: "#bdc3c7"; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.kernel; color: "white"; font.bold: true }
            }
            Column { 
                Text { text: "Driver GPU"; color: "#bdc3c7"; font.pixelSize: 12 }
                Text { text: systemInfoViewModel.driver; color: "white"; font.bold: true }
            }
        }
    }

    // Log Terminal
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: "#1e1e1e"
        radius: 4
        ScrollView {
            anchors.fill: parent
            TextArea {
                id: logArea
                text: installationViewModel.logOutput
                readOnly: true
                color: "#dcdcdc"
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
        background: Rectangle { color: "#2c3e50"; radius: 8 }
        
        ColumnLayout {
            spacing: 10
            Text { text: "Confirmação necessária:"; color: "white"; font.bold: true }
            Text { text: installationViewModel.question; color: "#bdc3c7"; wrapMode: Text.WordWrap; width: 300 }
            Row {
                spacing: 10
                Button { text: "Sim (y)"; onClicked: installationViewModel.respond(true) }
                Button { text: "Não (n)"; onClicked: installationViewModel.respond(false) }
            }
        }
    }
}
