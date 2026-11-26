import QtQuick
import QtQuick.Layouts
import ".."
import "../components"

// ABA 0 - Dashboard ("Mission Control"): cartões-resumo, áudio e explorador de processos.
ScrollPage {
    id: dashboardView
    // Largura da janela (define 2 ou 3 colunas nos cartões).
    property real windowWidth: 0
    // Navegação e diálogos ficam em Main.qml; a view só pede.
    signal navigateRequested(int index)
    signal cpuDetailsRequested()
    signal killProcessRequested(var pid, var name, var startTime)

    clip: false
    contentSpacing: 25

    PageTitle { text: qsTr("Mission Control") }
    GridLayout {
        columns: dashboardView.windowWidth > 800 ? 3 : 2
        Layout.fillWidth: true; rowSpacing: 15; columnSpacing: 15
        DashboardCard { title: qsTr("CPU"); value: cpuViewModel.totalLoad.toFixed(1) + "%"; onClicked: dashboardView.cpuDetailsRequested() }
        DashboardCard { title: qsTr("MEM"); value: memoryViewModel.ramUsagePercent.toFixed(1) + "%"; onClicked: dashboardView.navigateRequested(5) /* Hardware */ }
        DashboardCard { title: qsTr("DISK"); value: storageViewModel.totalThroughput; onClicked: dashboardView.navigateRequested(5) /* Hardware */ }
        DashboardCard { title: qsTr("NET"); value: networkViewModel.latency >= 0 ? networkViewModel.latency.toFixed(0) + "ms" : networkViewModel.status; onClicked: dashboardView.navigateRequested(4) /* Network */ }
        DashboardCard { title: qsTr("POWER"); value: powerViewModel.batteryPercent.toFixed(0) + "%"; onClicked: dashboardView.navigateRequested(5) /* Hardware */ }
        DashboardCard { title: qsTr("CORE"); value: systemInfoViewModel.kernelVersion; onClicked: dashboardView.navigateRequested(2) /* Journal */ }
    }

    AudioVisualizer {}

    ProcessExplorer {
        onKillRequested: (pid, name, startTime) => dashboardView.killProcessRequested(pid, name, startTime)
    }
}
