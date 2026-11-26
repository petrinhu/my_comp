import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import "."
import "views"
import "components"

// Janela principal: sidebar de navegação, StackLayout com uma view por aba e
// overlays/diálogos globais. Cores e estado do tema vivem no singleton Theme.
Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("my_comp v2.0 Alpha")
    color: Theme.windowColor

    // Aba ativa (índices do StackLayout abaixo; mesma ordem da NavigationSidebar).
    property int currentView: 0

    // Preferência de tema do sistema, vinda do C++ (context property).
    Binding { target: Theme; property: "systemThemeIsDark"; value: isSystemDarkInitial }

    // Sequenciador de Carga Inicial (AR-09: cadeia por event-loop, sem Timer artificial)
    QtObject {
        id: startupSequencer
        property int step: 0
        readonly property var actions: [
            () => systemInfoViewModel.refresh(),
            () => userMetadataViewModel.refresh(),
            () => cpuViewModel.initialize(),
            () => memoryViewModel.refresh(),
            () => storageViewModel.refresh(),
            () => storageViewModel.initialize(),
            () => networkViewModel.refresh(),
            () => processViewModel.refresh(),
            () => serviceViewModel.refresh(),
            () => devStackViewModel.refresh(),
            () => updateViewModel.refresh(),
            () => {
                if (firmwareViewModel) firmwareViewModel.refresh();
                if (hardwareViewModel) hardwareViewModel.initialize();
                if (gpuViewModel) gpuViewModel.initialize();
                if (powerViewModel) powerViewModel.initialize();
            },
            () => { if (audioViewModel) audioViewModel.start(); }
        ]
        function advance() {
            if (step >= actions.length) {
                console.log("[Main] Startup sequence completed.");
                return;
            }
            actions[step]();
            step++;
            Qt.callLater(advance);
        }
    }

    Component.onCompleted: {
        startupSequencer.advance();
    }

    // --- Overlays e diálogos globais ---
    CpuDetailDialog { id: cpuDetailDialog }

    ToastOverlay { id: toastOverlay }

    Connections {
        target: exportViewModel
        function onExportFinished(success, message) {
            toastOverlay.notify(message, success ? "success" : "error")
        }
    }

    Connections {
        target: serviceViewModel
        function onOperationFinished(success, message) {
            toastOverlay.notify(message, success ? "success" : "error")
        }
    }

    DaemonDialog {
        id: daemonDialog
        onBrowseRequested: fileDialog.open()
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Selecionar Executável")
        currentFolder: StandardPaths.writableLocation(StandardPaths.ApplicationsLocation)
        onAccepted: {
            let path = selectedFile.toString().replace("file://", "")
            daemonDialog.exePath = decodeURIComponent(path)
        }
    }

    FolderDialog {
        id: exportFolderDialog
        title: qsTr("Selecionar Pasta para Exportação")
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            let path = selectedFolder.toString().replace("file://", "")
            exportView.exportPath = decodeURIComponent(path)
        }
    }

    KillProcessDialog { id: killConfirmDialog }

    // --- Layout principal ---
    RowLayout {
        anchors.fill: parent
        spacing: 0

        NavigationSidebar {
            Layout.fillHeight: true
            Layout.preferredWidth: 220
            currentIndex: root.currentView
            onNavigateRequested: (index) => root.currentView = index
        }

        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.currentView

            // ABA 0 - Dashboard
            DashboardView {
                windowWidth: root.width
                onNavigateRequested: (index) => root.currentView = index
                onCpuDetailsRequested: cpuDetailDialog.open()
                onKillProcessRequested: (pid, name) => killConfirmDialog.openWith(pid, name)
            }
            // ABA 1 - Updates
            UpdatesView { onNavigateRequested: (index) => root.currentView = index }
            // ABA 2 - Journal
            JournalView {}
            // ABA 3 - Services
            ServicesView { onCreateDaemonRequested: daemonDialog.open() }
            // ABA 4 - Network
            NetworkView { windowWidth: root.width }
            // ABA 5 - Hardware
            HardwareView {}
            // ABA 6 - Dev Tools
            DevToolsView {}
            // ABA 7 - Security
            SecurityView {}
            // ABA 8 - Export
            ExportView { id: exportView; onChooseFolderRequested: exportFolderDialog.open() }
            // ABA 9 - Storage
            StorageView {}
            // ABA 10 - VRAM
            VramView {}
            // ABA 11 - Installation
            InstallationView { id: installView; anchors.margins: 25 }
        }
    }
}
