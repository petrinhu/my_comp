import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Confirmação antes de encerrar um processo (aberto pelo explorador de processos).
Dialog {
    id: killConfirmDialog
    title: qsTr("Encerrar Processo?")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    modal: true
    standardButtons: Dialog.Yes | Dialog.No

    property int targetPid: 0
    property string targetName: ""

    function openWith(pid, name) {
        targetPid = pid
        targetName = name
        open()
    }

    ColumnLayout {
        spacing: 10
        Text { text: qsTr("Deseja realmente encerrar o processo?"); color: Theme.textColor }
        Text { text: qsTr("<b>%1</b> (PID: %2)").arg(killConfirmDialog.targetName).arg(killConfirmDialog.targetPid); color: Theme.criticalColor; textFormat: Text.StyledText }
    }

    onAccepted: processViewModel.killProcess(targetPid)
}
