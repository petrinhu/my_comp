import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 10 - Auditoria de uso de VRAM: processos por GPU classificados pela cascata
// Catálogo → Categoria desktop → Comportamento.
ScrollPage {
    id: vramView
    contentSpacing: 14

    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        Text {
            text: qsTr("🎮 Auditoria de uso de VRAM")
            color: Theme.textColor
            font.pixelSize: 22
            font.bold: true
            Layout.fillWidth: true
        }
        Button {
            text: vramAuditViewModel.isBusy ? qsTr("⏳ Auditando...") : qsTr("🔄 Atualizar")
            enabled: !vramAuditViewModel.isBusy
            onClicked: vramAuditViewModel.refresh()
        }
    }

    Text {
        text: vramAuditViewModel.grupos.length === 0
              ? qsTr("Nenhuma GPU detectada ou auditoria ainda não executada. Clique em \"Atualizar\".")
              : qsTr("Cada processo é classificado pela cascata Catálogo → Categoria desktop → Comportamento. Verde = candidato a desligar GPU; Amarelo = inconclusivo; Vermelho = precisa GPU.")
        color: Theme.textColor
        opacity: 0.85
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }

    Repeater {
        model: vramAuditViewModel.grupos
        delegate: VramGpuGroup { grupo: modelData }
    }
}
