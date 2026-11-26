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
    // starttime do processo na coleta: o provedor só sinaliza se ainda for o mesmo
    // processo (PID reciclado entre a lista e o "Sim" é recusado).
    property real targetStartTime: -1

    function openWith(pid, name, startTime) {
        targetPid = pid
        targetName = name
        targetStartTime = startTime === undefined ? -1 : startTime
        open()
    }

    ColumnLayout {
        spacing: 10
        Text { text: qsTr("Deseja realmente encerrar o processo?"); color: Theme.textColor }
        // O nome vem do cmdline do processo, que qualquer programa local controla: texto
        // puro, para uma marcação no nome não falsificar o que está sendo confirmado (MC-10).
        // O cmdline pode ser longo: quebra e corta em 4 linhas.
        Text {
            objectName: "killTargetName"
            text: qsTr("%1 (PID: %2)").arg(killConfirmDialog.targetName).arg(killConfirmDialog.targetPid)
            textFormat: Text.PlainText
            color: Theme.criticalColor
            font.bold: true
            wrapMode: Text.WrapAnywhere
            maximumLineCount: 4
            elide: Text.ElideRight
            Layout.maximumWidth: 480
        }
    }

    onAccepted: processViewModel.killProcess(targetPid, targetStartTime)
}
