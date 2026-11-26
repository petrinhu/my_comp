import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Processo auditado na aba VRAM: classificação (cor), VRAM usada, regra, sugestão e origens de config.
Rectangle {
    id: processItem
    // Entrada de grupo.itens.
    property var item: ({})
    Layout.fillWidth: true
    color: "transparent"
    border.color: Theme.borderColor
    radius: 6
    implicitHeight: itemCol.implicitHeight + 14

    ColumnLayout {
        id: itemCol
        anchors.fill: parent
        anchors.margins: 7
        spacing: 4

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Rectangle {
                width: 10; height: 10; radius: 5
                color: processItem.item.categoria === "PrecisaGpu" ? Theme.errorColor
                     : processItem.item.categoria === "CandidatoObvioCpu" ? Theme.successStrongColor
                     : Theme.warningColor
            }
            Text {
                text: qsTr("%1 (PID %2)").arg(processItem.item.nome).arg(processItem.item.pid)
                color: Theme.textColor
                font.bold: true
                Layout.fillWidth: true
            }
            Text {
                text: qsTr("VRAM: %1 MiB").arg(processItem.item.vramMb.toFixed(0))
                color: Theme.highlightColor
                font.family: "monospace"
                font.pixelSize: 11
            }
        }

        Text {
            text: qsTr("Categoria: %1  ·  Regra: %2").arg(processItem.item.categoria).arg(processItem.item.regra)
                  + (processItem.item.detalhe.length > 0 ? "  ·  " + processItem.item.detalhe : "")
            color: Theme.textColor
            opacity: 0.75
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Text {
            visible: processItem.item.sugestaoDescricao && processItem.item.sugestaoDescricao.length > 0
            text: "💡 " + processItem.item.sugestaoDescricao
            color: Theme.highlightColor
            opacity: 0.9
            font.pixelSize: 11
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Repeater {
            model: processItem.item.origensConfig
            delegate: VramConfigOrigin { origem: modelData }
        }

        // Feedback do usuário (VRAM-07): fixa o veredito deste binário nas próximas
        // auditorias; a RegraFeedback vence as demais regras da cascata.
        RowLayout {
            visible: vramAuditViewModel.feedbackDisponivel
                     && processItem.item.chaveBinario !== undefined
                     && processItem.item.chaveBinario.length > 0
            Layout.fillWidth: true
            spacing: 6
            Text {
                text: qsTr("Seu veredito:")
                color: Theme.textColor
                opacity: 0.75
                font.pixelSize: 11
            }
            Button {
                text: qsTr("Precisa de GPU")
                enabled: !vramAuditViewModel.isBusy
                         && !(processItem.item.regra === "feedback" && processItem.item.categoria === "PrecisaGpu")
                onClicked: vramAuditViewModel.registrarFeedback(processItem.item.chaveBinario, "PrecisaGpu", "")
            }
            Button {
                text: qsTr("Pode usar só CPU")
                enabled: !vramAuditViewModel.isBusy
                         && !(processItem.item.regra === "feedback" && processItem.item.categoria === "CandidatoObvioCpu")
                onClicked: vramAuditViewModel.registrarFeedback(processItem.item.chaveBinario, "CandidatoObvioCpu", "")
            }
            Item { Layout.fillWidth: true }
        }
    }
}
