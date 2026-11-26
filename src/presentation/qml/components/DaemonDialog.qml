import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Diálogo para criar um daemon systemd de usuário.
// O seletor de arquivo fica em Main.qml (FileDialog precisa da janela como pai):
// ele escuta browseRequested e grava o caminho em exePath.
Dialog {
    id: daemonDialog
    property alias exePath: exePathField.text
    signal browseRequested()

    title: qsTr("Criar Novo Daemon de Usuário")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 400
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    ColumnLayout {
        anchors.fill: parent
        spacing: 15

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: exePathField
                placeholderText: qsTr("Caminho do executável...")
                Layout.fillWidth: true
                readOnly: true
            }
            Button {
                text: qsTr("Selecionar")
                onClicked: daemonDialog.browseRequested()
            }
        }

        TextField {
            id: paramsField
            placeholderText: qsTr("Parâmetros opcionais (ex: --verbose)")
            Layout.fillWidth: true
        }

        Text {
            text: qsTr("O daemon será criado em ~/.config/systemd/user/")
            font.pixelSize: 10
            color: Theme.mutedTextColor
        }
    }

    onAccepted: {
        if (exePathField.text !== "") {
            serviceViewModel.createDaemon(exePathField.text, paramsField.text)
            exePathField.text = ""
            paramsField.text = ""
        }
    }
}
