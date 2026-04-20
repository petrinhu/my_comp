import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("my_comp v2.0 Alpha")
    
    // --- Sistema de Temas ---
    property int themeMode: 2 // Auto Mode
    readonly property bool isDark: themeMode === 1 || (themeMode === 2 && systemThemeIsDark)
    property bool systemThemeIsDark: isSystemDarkInitial 

    readonly property color lightBg: "#eff0f1"
    readonly property color lightSidebar: "#fcfcfc"
    readonly property color lightCard: "#ffffff"
    readonly property color lightText: "#232629"
    readonly property color lightBorder: "#bcbebf"

    readonly property color darkBg: "#232629"
    readonly property color darkSidebar: "#31363b"
    readonly property color darkCard: "#31363b"
    readonly property color darkText: "#eff0f1"
    readonly property color darkBorder: "#4d4d4d"

    color: isDark ? darkBg : lightBg
    readonly property color textColor: isDark ? darkText : lightText
    readonly property color sidebarColor: isDark ? darkSidebar : lightSidebar
    readonly property color cardColor: isDark ? darkCard : lightCard
    readonly property color borderColor: isDark ? darkBorder : lightBorder
    readonly property color highlightColor: isDark ? "#3daee9" : "#1b6acb"
    readonly property color criticalColor: isDark ? "#ff4d4d" : "#c0392b"

    property int currentView: 0

    // Sequenciador de Carga Inicial (Item 43)
    Timer {
        id: startupSequencer
        interval: 300 // Aumentado para 300ms para maior estabilidade
        repeat: true
        property int step: 0
        onTriggered: {
            switch(step) {
                case 0: systemInfoViewModel.refresh(); break;
                case 1: userMetadataViewModel.refresh(); break;
                case 2: cpuViewModel.refresh(); break;
                case 3: memoryViewModel.refresh(); break;
                case 4: storageViewModel.refresh(); break;
                case 5: networkViewModel.refresh(); break;
                case 6: processViewModel.refresh(); break;
                case 7: serviceViewModel.refresh(); break;
                case 8: devStackViewModel.refresh(); break;
                case 9: updateViewModel.refresh(); break;
                case 10: if (firmwareViewModel) firmwareViewModel.refresh(); break;
                case 11: 
                    if (audioViewModel) {
                        console.log("[Main] Starting audio...");
                        audioViewModel.start();
                    }
                    break;
                default: 
                    stop();
                    console.log("[Main] Startup sequence completed.");
                    return;
            }
            step++;
        }
    }

    Component.onCompleted: {
        startupSequencer.start();
    }

    CpuDetailDialog { id: cpuDetailDialog }

    // Dialog para Criar Daemon
    Dialog {
        id: daemonDialog
        title: "Criar Novo Daemon de Usuário"
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
                    placeholderText: "Caminho do executável..."
                    Layout.fillWidth: true
                    readOnly: true
                }
                Button { 
                    text: "Selecionar"
                    onClicked: fileDialog.open()
                }
            }
            
            TextField {
                id: paramsField
                placeholderText: "Parâmetros opcionais (ex: --verbose)"
                Layout.fillWidth: true
            }
            
            Text {
                text: "O daemon será criado em ~/.config/systemd/user/"
                font.pixelSize: 10
                color: "#7f8c8d"
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

    FileDialog {
        id: fileDialog
        title: "Selecionar Executável"
        currentFolder: StandardPaths.writableLocation(StandardPaths.ApplicationsLocation)
        onAccepted: {
            let path = selectedFile.toString().replace("file://", "")
            exePathField.text = decodeURIComponent(path)
        }
    }

    FolderDialog {
        id: exportFolderDialog
        title: "Selecionar Pasta para Exportação"
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            let path = selectedFolder.toString().replace("file://", "")
            exportPathField.text = decodeURIComponent(path)
        }
    }

    Dialog {
        id: killConfirmDialog
        title: "Encerrar Processo?"
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
            Text { text: "Deseja realmente encerrar o processo?"; color: root.textColor }
            Text { text: "<b>" + killConfirmDialog.targetName + "</b> (PID: " + killConfirmDialog.targetPid + ")"; color: root.criticalColor; textFormat: Text.StyledText }
        }

        onAccepted: processViewModel.killProcess(targetPid)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 220
            color: root.sidebarColor
            border.color: root.borderColor
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 8

                Text {
                    text: "my_comp"
                    color: root.highlightColor
                    font.pixelSize: 26
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 20
                }

                Button {
                    id: themeToggle
                    Layout.fillWidth: true
                    flat: true
                    onClicked: root.themeMode = (root.themeMode + 1) % 3
                    background: Rectangle {
                        color: themeToggle.pressed ? (root.isDark ? "#454a4f" : "#d0d0d0") : 
                               themeToggle.hovered ? (root.isDark ? "#3e444a" : "#e8e8e8") : 
                               (root.isDark ? "#2a2e32" : "#f5f5f5")
                        radius: 6
                    }
                    contentItem: RowLayout {
                        spacing: 10
                        Text { text: root.themeMode === 0 ? "☀️" : root.themeMode === 1 ? "🌙" : "🌓"; color: root.textColor }
                        Text { text: root.themeMode === 0 ? "Light" : root.themeMode === 1 ? "Dark" : "Auto"; color: root.textColor; font.bold: true }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: root.borderColor; Layout.margins: 5 }

                Repeater {
                    model: [
                        {name: "Dashboard", icon: "📊"},
                        {name: "Updates", icon: "📦"},
                        {name: "Journal", icon: "📜"},
                        {name: "Services", icon: "⚙️"},
                        {name: "Network", icon: "🌐"},
                        {name: "Hardware", icon: "💻"},
                        {name: "Dev Tools", icon: "🛠️"},
                        {name: "Security", icon: "🛡️"},
                        {name: "Export", icon: "📤"}
                    ]
                    delegate: ItemDelegate {
                        id: navDelegate
                        Layout.fillWidth: true
                        highlighted: root.currentView === index
                        onClicked: root.currentView = index
                        ToolTip.visible: hovered
                        ToolTip.text: modelData.name
                        background: Rectangle {
                            color: navDelegate.highlighted ? root.highlightColor : (navDelegate.hovered ? (root.isDark ? "#3e444a" : "#e8e8e8") : "transparent")
                            opacity: navDelegate.highlighted ? 0.2 : 1.0
                            radius: 6
                        }
                        contentItem: RowLayout { 
                            spacing: 12
                            Text { text: modelData.icon; font.pixelSize: 18; color: navDelegate.highlighted ? root.highlightColor : root.textColor }
                            Text { text: modelData.name; color: navDelegate.highlighted ? root.highlightColor : root.textColor; font.bold: navDelegate.highlighted } 
                        }
                    }
                }
                Item { Layout.fillHeight: true }
                Text { text: "v2.0 Alpha"; color: root.highlightColor; font.pixelSize: 10; font.bold: true; Layout.alignment: Qt.AlignHCenter } 
            }
        }

        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.currentView

            // ABA 0 - Dashboard
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    anchors.margins: 25; spacing: 25
                    Text { text: "Mission Control"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    GridLayout {
                        columns: root.width > 800 ? 3 : 2
                        Layout.fillWidth: true; rowSpacing: 15; columnSpacing: 15
                        DashboardCard { title: "CPU"; value: cpuViewModel.totalLoad.toFixed(1) + "%"; onClicked: cpuDetailDialog.open() }
                        DashboardCard { title: "MEM"; value: memoryViewModel.ramUsagePercent.toFixed(1) + "%"; onClicked: root.currentView = 5 /* Hardware */ }
                        DashboardCard { title: "DISK"; value: storageViewModel.totalThroughput; onClicked: root.currentView = 5 /* Hardware */ }
                        DashboardCard { title: "NET"; value: networkViewModel.latency.toFixed(0) + "ms"; onClicked: root.currentView = 4 /* Network */ }
                        DashboardCard { title: "POWER"; value: powerViewModel.batteryPercent.toFixed(0) + "%"; onClicked: console.log("Power details clicked") }
                        DashboardCard { title: "CORE"; value: systemInfoViewModel.kernel.split("-")[0]; onClicked: root.currentView = 2 /* Journal */ }
                    }

                    // Audio Waveform & Spectrum Visualizer
                    Rectangle {
                        Layout.fillWidth: true; height: 140; color: root.cardColor; radius: 8; border.color: root.borderColor
                        clip: true
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 15; spacing: 15
                            
                            ColumnLayout {
                                Layout.preferredWidth: 80
                                Text { text: "AUDIO"; color: root.highlightColor; font.bold: true; font.pixelSize: 10 }
                                RowLayout {
                                    spacing: 4
                                    Rectangle { width: 6; height: 50; color: "#2ecc71"; opacity: 0.2; radius: 2 
                                        Rectangle { anchors.bottom: parent.bottom; width: 6; height: parent.height * Math.min(1.0, audioViewModel.rms * 2); color: "#2ecc71"; radius: 2 }
                                    }
                                    Rectangle { width: 6; height: 50; color: root.criticalColor; opacity: 0.2; radius: 2 
                                        Rectangle { anchors.bottom: parent.bottom; width: 6; height: parent.height * Math.min(1.0, audioViewModel.peak); color: root.criticalColor; radius: 2 }
                                    }
                                }
                                Text { text: (audioViewModel.rms * 100).toFixed(0) + "%"; color: root.textColor; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
                            }

                            // Waveform
                            Canvas {
                                id: audioCanvas
                                Layout.fillWidth: true; Layout.fillHeight: true
                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.clearRect(0, 0, width, height);
                                    ctx.strokeStyle = root.highlightColor;
                                    ctx.globalAlpha = 0.8;
                                    ctx.lineWidth = 2.0;
                                    ctx.beginPath();
                                    
                                    var data = audioViewModel.waveform;
                                    if (!data || data.length === 0) return;
                                    
                                    var sliceWidth = width / data.length;
                                    var x = 0;
                                    for (var i = 0; i < data.length; i++) {
                                        // Aplicar ganho visual de 2x para a onda
                                        var v = data[i] * 2.0; 
                                        if (v > 1.0) v = 1.0; else if (v < -1.0) v = -1.0;
                                        
                                        var y = (v + 1) / 2 * height;
                                        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
                                        x += sliceWidth;
                                    }
                                    ctx.stroke();
                                }
                                
                                Connections {
                                    target: audioViewModel
                                    function onAudioDataChanged() { 
                                        audioCanvas.requestPaint(); 
                                        spectrumCanvas.requestPaint(); 
                                    }
                                }
                            }

                            // Spectrum (FFT)
                            Canvas {
                                id: spectrumCanvas
                                Layout.fillWidth: true; Layout.fillHeight: true
                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.clearRect(0, 0, width, height);
                                    var data = audioViewModel.spectrum;
                                    if (!data || data.length === 0) return;
                                    
                                    var barWidth = width / data.length;
                                    ctx.fillStyle = root.highlightColor;
                                    
                                    for (var i = 0; i < data.length; i++) {
                                        // Sensibilidade aumentada para o espectro (x10 e sqrt para compressão dinâmica)
                                        var magnitude = Math.sqrt(data[i]) * 10; 
                                        var h = Math.min(height, magnitude * height);
                                        ctx.fillRect(i * barWidth, height - h, barWidth - 1, h);
                                    }
                                }
                            }
                        }
                    }

                    // Process Explorer (Estilo htop)
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 400
                        spacing: 10
                        
                        RowLayout {
                            Text { text: "Explorador de Processos"; color: root.textColor; font.pixelSize: 18; font.bold: true }
                            Item { Layout.fillWidth: true }
                            TextField {
                                placeholderText: "Filtrar (Nome/User)..."
                                onTextChanged: processViewModel.filter = text
                                Layout.preferredWidth: 200
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                            ColumnLayout {
                                anchors.fill: parent; spacing: 0
                                
                                // Header manual simplificado
                                RowLayout {
                                    Layout.fillWidth: true; height: 30
                                    Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: root.isDark ? "#2a2e32" : "#f5f5f5"; border.color: root.borderColor
                                        RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                                            Text { text: "PID"; font.bold: true; Layout.preferredWidth: 60; color: root.textColor }
                                            Text { text: "USER"; font.bold: true; Layout.preferredWidth: 100; color: root.textColor }
                                            Text { text: "CPU%"; font.bold: true; Layout.preferredWidth: 60; color: root.textColor }
                                            Text { text: "MEM%"; font.bold: true; Layout.preferredWidth: 60; color: root.textColor }
                                            Text { text: "STATE"; font.bold: true; Layout.preferredWidth: 50; color: root.textColor }
                                            Text { text: "COMMAND"; font.bold: true; Layout.fillWidth: true; color: root.textColor }
                                        }
                                    }
                                }

                                ScrollView {
                                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                                    ListView {
                                        id: processList
                                        model: processViewModel.processes
                                        delegate: ItemDelegate {
                                            width: processList.width; height: 30
                                            padding: 0
                                            contentItem: RowLayout {
                                                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                                                Text { text: modelData.pid; Layout.preferredWidth: 60; color: root.highlightColor; font.family: "Monospace" }
                                                Text { text: modelData.user; Layout.preferredWidth: 100; color: root.textColor; font.pixelSize: 11 }
                                                Text { text: modelData.cpu.toFixed(1); Layout.preferredWidth: 60; color: modelData.cpu > 50 ? root.criticalColor : "#2ecc71"; font.bold: true; font.family: "Monospace" }
                                                Text { text: modelData.mem.toFixed(1); Layout.preferredWidth: 60; color: root.highlightColor; font.family: "Monospace" }
                                                Text { text: modelData.state; Layout.preferredWidth: 50; color: modelData.state === "R" ? "#2ecc71" : "#7f8c8d"; font.bold: true }
                                                Text { text: modelData.command; Layout.fillWidth: true; color: root.textColor; elide: Text.ElideRight; font.family: "Monospace"; font.pixelSize: 11 }
                                            }
                                            
                                            onDoubleClicked: killConfirmDialog.openWith(modelData.pid, modelData.command)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ABA 1 - Updates
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                RowLayout {
                    Text { text: "Central de Atualizações"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { 
                        text: "Atualizar Tudo"
                        highlighted: true
                        onClicked: {
                            installationViewModel.startUpgradeAll()
                            root.currentView = 8 // ABA de Instalação
                        }
                    }
                    Button { text: "🔄"; onClicked: updateViewModel.refresh() }
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ListView {
                            model: updateViewModel.updateModel
                            delegate: ItemDelegate {
                                width: parent.width; height: 50
                                onDoubleClicked: {
                                    installationViewModel.startInstallation(name, provider)
                                    root.currentView = 8 // ABA de Instalação
                                }
                                contentItem: RowLayout {
                                    Text { text: name; color: root.textColor; font.bold: true; Layout.fillWidth: true }
                                    Text { text: source; color: "#7f8c8d"; font.pixelSize: 10; Layout.preferredWidth: 60 }
                                    Text { text: newVersion; color: root.highlightColor }
                                }
                            }
                        }
                    }
                }
            }

            // ABA 2 - Journal
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                Text { text: "Analisador de Journal"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ListView {
                            model: diagnosticViewModel.logModel
                            delegate: ItemDelegate {
                                width: parent.width; height: 40
                                onDoubleClicked: diagnosticViewModel.searchOnline(source + " " + message)
                                contentItem: Text { text: message; color: root.textColor; elide: Text.ElideRight }
                            }
                        }
                    }
                }
            }

            // ABA 3 - Services
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                RowLayout { 
                    Text { text: "Serviços Systemd"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { text: "🆕 Criar Daemon"; highlighted: true; onClicked: daemonDialog.open() }
                    Button { text: "🔄"; onClicked: serviceViewModel.refresh() } 
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ListView {
                            id: serviceList
                            model: serviceViewModel.services
                            delegate: ItemDelegate {
                                width: serviceList.width; height: 60
                                onDoubleClicked: (modelData.activeState === "active") ? serviceViewModel.pauseService(modelData.name) : serviceViewModel.startService(modelData.name)
                                contentItem: RowLayout {
                                    spacing: 15
                                    Rectangle { width: 10; height: 10; radius: 5; color: modelData.activeState === "active" ? "#2ecc71" : "#95a5a6" }
                                    ColumnLayout { spacing: 2; Layout.fillWidth: true
                                        Text { text: modelData.name; color: root.textColor; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                                        Text { text: modelData.description; color: "#7f8c8d"; font.pixelSize: 10; elide: Text.ElideRight; Layout.fillWidth: true }
                                    }
                                    RowLayout {
                                        Button { text: "▶️"; flat: true; visible: modelData.activeState !== "active"; onClicked: serviceViewModel.startService(modelData.name) }
                                        Button { text: "⏸️"; flat: true; visible: modelData.activeState === "active"; onClicked: serviceViewModel.pauseService(modelData.name) }
                                        Button { text: "⏹️"; flat: true; onClicked: serviceViewModel.stopService(modelData.name) }
                                    }
                                }
                            }
                        }
                        BusyIndicator { anchors.centerIn: parent; running: serviceViewModel.isBusy }
                    }
                }
            }

            // ABA 4 - Network
            PlaceholderView { title: "Rede"; icon: "🌐"; desc: "Tráfego e latência." }

            // ABA 5 - Hardware
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                Text { text: "Hardware"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ListView {
                            model: firmwareViewModel.devices
                            delegate: ItemDelegate {
                                width: parent.width; height: 40
                                contentItem: Text { text: modelData.name; color: root.textColor }
                            }
                        }
                    }
                }
            }

            // ABA 6 - Dev Tools
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                RowLayout {
                    Text { text: "Desenvolvimento & Containers"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { text: "🔄"; onClicked: devStackViewModel.refresh() }
                }
                
                ScrollView {
                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                    ColumnLayout {
                        width: parent.width; spacing: 20
                        
                        Text { text: "🛠️ COMPILADORES & RUNTIMES"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; height: 200; color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                anchors.fill: parent; anchors.margins: 10; clip: true; model: devStackViewModel.toolchains
                                delegate: RowLayout {
                                    width: parent.width; height: 30
                                    Text { text: modelData.name; font.bold: true; color: root.textColor; Layout.preferredWidth: 100 }
                                    Text { text: modelData.version; color: "#7f8c8d"; Layout.fillWidth: true; elide: Text.ElideRight; font.family: "Monospace"; font.pixelSize: 11 }
                                }
                            }
                        }
                        
                        Text { text: "🐳 CONTAINERS (PODMAN/DOCKER)"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 300; color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                id: containerList
                                anchors.fill: parent; anchors.margins: 10; clip: true; model: devStackViewModel.containers
                                delegate: ItemDelegate {
                                    width: containerList.width; height: 50
                                    contentItem: RowLayout {
                                        spacing: 15
                                        Rectangle { width: 8; height: 8; radius: 4; color: modelData.status.includes("Up") ? "#2ecc71" : "#95a5a6" }
                                        ColumnLayout {
                                            spacing: 2
                                            Text { text: modelData.name; color: root.textColor; font.bold: true }
                                            Text { text: modelData.runtime + " • " + modelData.image; color: "#7f8c8d"; font.pixelSize: 10 }
                                        }
                                        Item { Layout.fillWidth: true }
                                        Text { text: modelData.status; color: root.textColor; font.pixelSize: 11 }
                                    }
                                }
                                Text { anchors.centerIn: parent; text: "Nenhum container detectado."; color: "#7f8c8d"; visible: parent.count === 0 }
                            }
                        }
                    }
                }
            }

            // ABA 7 - Security
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                Text { text: "Segurança"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ListView {
                            model: userMetadataViewModel.loginHistory
                            delegate: Text { text: modelData; color: root.textColor; font.family: "Monospace"; font.pixelSize: 10 }
                        }
                    }
                }
            }

            // ABA 8 - Export
            ColumnLayout {
                anchors.margins: 25; spacing: 20
                Text { text: "Centro de Exportação"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                
                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 300; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ColumnLayout {
                        anchors.centerIn: parent; width: parent.width * 0.8; spacing: 15
                        
                        Text { text: "Gerar Relatório Completo do Sistema"; color: root.highlightColor; font.bold: true }
                        
                        RowLayout {
                            spacing: 10
                            TextField {
                                id: fileNameField
                                placeholderText: "Nome do arquivo (ex: relatorio_hoje)"
                                Layout.fillWidth: true
                                text: "my_comp_report_" + Qt.formatDateTime(new Date(), "yyyyMMdd")
                            }
                            ComboBox {
                                id: formatCombo
                                model: ["JSON", "Markdown", "HTML"]
                                Layout.preferredWidth: 120
                            }
                        }
                        
                        RowLayout {
                            spacing: 10
                            TextField {
                                id: exportPathField
                                placeholderText: "Pasta de destino..."
                                Layout.fillWidth: true
                                readOnly: true
                                text: StandardPaths.writableLocation(StandardPaths.DocumentsLocation).toString().replace("file://", "")
                            }
                            Button {
                                text: "Escolher Pasta"
                                onClicked: exportFolderDialog.open()
                            }
                        }
                        
                        Item { Layout.preferredHeight: 20 }
                        
                        Button {
                            text: "🚀 Gerar Relatório Agora"
                            Layout.alignment: Qt.AlignHCenter
                            highlighted: true
                            enabled: !exportViewModel.isBusy && exportPathField.text !== ""
                            onClicked: {
                                let ext = formatCombo.currentText === "JSON" ? ".json" : (formatCombo.currentText === "Markdown" ? ".md" : ".html")
                                let fullPath = exportPathField.text + "/" + fileNameField.text + ext
                                exportViewModel.exportReport(formatCombo.currentIndex, fullPath)
                            }
                        }
                        
                        Text {
                            id: statusMsg
                            text: ""
                            color: "#2ecc71"
                            Layout.alignment: Qt.AlignHCenter
                            font.pixelSize: 12
                        }
                        
                        BusyIndicator {
                            running: exportViewModel.isBusy
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
                
                Connections {
                    target: exportViewModel
                    function onExportFinished(success, message) {
                        statusMsg.text = message
                        statusMsg.color = success ? "#2ecc71" : root.criticalColor
                    }
                }
                
                Item { Layout.fillHeight: true }
            }

            // ABA 9 - Installation
            InstallationView { id: installView; anchors.margins: 25 }
        }
    }

    component DashboardCard : Rectangle {
        property string title; property string value; signal clicked
        Layout.fillWidth: true; Layout.preferredHeight: 100; radius: 10
        color: ma.pressed ? (root.isDark ? "#3e444a" : "#e8e8e8") : root.cardColor
        border.color: root.borderColor
        scale: ma.pressed ? 0.98 : 1.0
        Behavior on scale { NumberAnimation { duration: 50 } }
        Behavior on color { ColorAnimation { duration: 150 } }

        MouseArea { 
            id: ma; anchors.fill: parent; onClicked: parent.clicked(); hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            ToolTip.visible: containsMouse
            ToolTip.text: "Clique para detalhes de " + title
        }
        ColumnLayout {
            anchors.centerIn: parent
            Text { text: title; color: root.textColor; font.pixelSize: 12; Layout.alignment: Qt.AlignHCenter }
            Text { text: value; color: root.highlightColor; font.pixelSize: 24; font.bold: true; Layout.alignment: Qt.AlignHCenter }
        }
    }

    component PlaceholderView : ColumnLayout {
        property string title; property string icon; property string desc
        Layout.fillWidth: true; Layout.fillHeight: true; spacing: 10
        Text { text: icon; font.pixelSize: 48; Layout.alignment: Qt.AlignHCenter }
        Text { text: title; color: root.textColor; font.pixelSize: 20; font.bold: true; Layout.alignment: Qt.AlignHCenter }
        Text { text: desc; color: "#7f8c8d"; Layout.alignment: Qt.AlignHCenter }
    }
}
