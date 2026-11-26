import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 4 - Rede: status, latência, interface e taxas RX/TX.
ScrollPage {
    id: networkView
    // Largura da janela (define 2 ou 3 colunas nos cartões).
    property real windowWidth: 0

    RowLayout {
        PageTitle { text: qsTr("Rede") }
        Item { Layout.fillWidth: true }
        IconButton { text: "🔄"; label: qsTr("Atualizar rede"); onClicked: networkViewModel.refresh() }
    }

    GridLayout {
        columns: networkView.windowWidth > 800 ? 3 : 2
        Layout.fillWidth: true; rowSpacing: 15; columnSpacing: 15
        DashboardCard { title: qsTr("STATUS"); value: networkViewModel.status }
        DashboardCard { title: qsTr("LATÊNCIA"); value: networkViewModel.latency >= 0 ? networkViewModel.latency.toFixed(0) + " ms" : "—" }
        DashboardCard { title: qsTr("INTERFACE"); value: networkViewModel.activeInterface || "—" }
        DashboardCard { title: qsTr("RX"); value: networkViewModel.totalRx.toFixed(2) + " Mbps" }
        DashboardCard { title: qsTr("TX"); value: networkViewModel.totalTx.toFixed(2) + " Mbps" }
    }

    // MC-10: a medição de latência pinga um host da internet; desligada por padrão.
    RowLayout {
        Layout.fillWidth: true
        Switch {
            id: externalProbeSwitch
            checked: networkViewModel.externalProbeEnabled
            onToggled: networkViewModel.externalProbeEnabled = checked
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            color: Theme.mutedTextColor
            text: qsTr("Medir latência pingando 1.1.1.1 (Cloudflare). Envia um pacote ICMP para a internet a cada atualização; desligado, nada sai da rede local.")
        }
    }

    Item { Layout.fillHeight: true }
}
