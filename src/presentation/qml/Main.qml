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

    // Helper para formatação humana de bytes (KiB/MiB/GiB/TiB).
    function formatBytes(n) {
        if (n === undefined || n === null || n <= 0) return "—";
        var units = ["B", "KiB", "MiB", "GiB", "TiB", "PiB"];
        var i = 0;
        var v = n;
        while (v >= 1024 && i < units.length - 1) { v /= 1024; i++; }
        return v.toFixed(i === 0 ? 0 : 1) + " " + units[i];
    }

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
                        {name: "Export", icon: "📤"},
                        {name: "Storage", icon: "💾"}
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
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
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
                        DashboardCard { title: "POWER"; value: powerViewModel.batteryPercent.toFixed(0) + "%"; onClicked: root.currentView = 5 /* Hardware */ }
                        DashboardCard { title: "CORE"; value: systemInfoViewModel.kernelVersion; onClicked: root.currentView = 2 /* Journal */ }
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
                                        // Ganho visual vem do ViewModel (CLAUDE.md §5).
                                        var v = data[i] * audioViewModel.waveformGain;
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
                                        // Ganho do espectro vem do ViewModel; sqrt para compressão dinâmica.
                                        var magnitude = Math.sqrt(data[i]) * audioViewModel.spectrumGain;
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
                                
                                // Header clicável para ordenação (dispara processViewModel.setSortField)
                                RowLayout {
                                    Layout.fillWidth: true; height: 30
                                    Rectangle { Layout.fillWidth: true; Layout.fillHeight: true; color: root.isDark ? "#2a2e32" : "#f5f5f5"; border.color: root.borderColor
                                        function applySort(field) {
                                            if (processViewModel.sortField === field) {
                                                processViewModel.sortAscending = !processViewModel.sortAscending
                                            } else {
                                                processViewModel.sortField = field
                                                processViewModel.sortAscending = false
                                            }
                                            processViewModel.refresh()
                                        }
                                        function arrow(field) {
                                            return processViewModel.sortField === field
                                                ? (processViewModel.sortAscending ? " ▲" : " ▼") : ""
                                        }
                                        RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 0
                                            Item { Layout.preferredWidth: 60; Layout.fillHeight: true
                                                Text { anchors.verticalCenter: parent.verticalCenter; text: "PID" + arrow("pid"); font.bold: true; color: root.textColor }
                                                MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: applySort("pid") }
                                            }
                                            Item { Layout.preferredWidth: 100; Layout.fillHeight: true
                                                Text { anchors.verticalCenter: parent.verticalCenter; text: "USER" + arrow("user"); font.bold: true; color: root.textColor }
                                                MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: applySort("user") }
                                            }
                                            Item { Layout.preferredWidth: 60; Layout.fillHeight: true
                                                Text { anchors.verticalCenter: parent.verticalCenter; text: "CPU%" + arrow("cpu"); font.bold: true; color: root.textColor }
                                                MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: applySort("cpu") }
                                            }
                                            Item { Layout.preferredWidth: 60; Layout.fillHeight: true
                                                Text { anchors.verticalCenter: parent.verticalCenter; text: "MEM%" + arrow("mem"); font.bold: true; color: root.textColor }
                                                MouseArea { anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: applySort("mem") }
                                            }
                                            Text { text: "STATE"; font.bold: true; Layout.preferredWidth: 50; color: root.textColor }
                                            Text { text: "COMMAND"; font.bold: true; Layout.fillWidth: true; color: root.textColor }
                                        }
                                    }
                                }

                                ScrollView {
                                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                                    ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }

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
                            root.currentView = 10 // ABA de Instalação
                        }
                    }
                    BusyIndicator { running: updateViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
                    Button { text: "🔄"; onClicked: updateViewModel.refresh(); enabled: !updateViewModel.isBusy }
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                        ScrollBar.horizontal: ScrollBar { policy: ScrollBar.AsNeeded }
                        ListView {
                            model: updateViewModel.updateModel
                            delegate: ItemDelegate {
                                width: parent.width; height: 50
                                onDoubleClicked: {
                                    installationViewModel.startInstallation(name, provider)
                                    root.currentView = 10 // ABA de Instalação (via duplo-clique)
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
                function priorityColor(p) {
                    // 0-2 emerg/alert/crit, 3 err, 4 warn, 5 notice, 6 info, 7 debug
                    if (p <= 2) return root.criticalColor
                    if (p === 3) return "#e74c3c"
                    if (p === 4) return "#f39c12"
                    if (p === 5) return root.highlightColor
                    return root.textColor
                }
                function priorityLabel(p) {
                    if (p === 0) return "EMERG"
                    if (p === 1) return "ALERT"
                    if (p === 2) return "CRIT"
                    if (p === 3) return "ERR"
                    if (p === 4) return "WARN"
                    if (p === 5) return "NOTICE"
                    if (p === 6) return "INFO"
                    return "DEBUG"
                }
                RowLayout {
                    Text { text: "Analisador de Journal"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    BusyIndicator { running: diagnosticViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
                    Button { text: "🔄"; onClicked: diagnosticViewModel.refresh(); enabled: !diagnosticViewModel.isBusy }
                }
                RowLayout {
                    Layout.fillWidth: true; spacing: 10
                    Text { text: "Filtro:"; color: root.textColor }
                    ComboBox {
                        id: filterCombo
                        Layout.preferredWidth: 200
                        model: ["Erros críticos", "Hardware/Firmware", "Boot", "Desktop (KDE/Wayland)", "Segurança/Rede", "Todos"]
                        currentIndex: diagnosticViewModel.currentFilter
                        onActivated: diagnosticViewModel.currentFilter = currentIndex
                    }
                    Text { text: "Buscar:"; color: root.textColor }
                    TextField {
                        id: journalSearchField
                        Layout.fillWidth: true
                        placeholderText: "termo (journalctl -g)…"
                        text: diagnosticViewModel.searchQuery
                        onAccepted: {
                            diagnosticViewModel.searchQuery = text
                            diagnosticViewModel.performSearch()
                        }
                    }
                    Button {
                        text: "Buscar"
                        onClicked: {
                            diagnosticViewModel.searchQuery = journalSearchField.text
                            diagnosticViewModel.performSearch()
                        }
                    }
                    Text { text: "Limite:"; color: root.textColor }
                    SpinBox {
                        from: 50; to: 2000; stepSize: 50
                        value: diagnosticViewModel.logLimit
                        onValueModified: diagnosticViewModel.logLimit = value
                    }
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ScrollView {
                        anchors.fill: parent; anchors.margins: 10; clip: true
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                        ListView {
                            model: diagnosticViewModel.logModel
                            delegate: ItemDelegate {
                                width: parent.width; height: 46
                                onDoubleClicked: diagnosticViewModel.searchOnline(source + " " + message)
                                contentItem: RowLayout {
                                    spacing: 8
                                    Text {
                                        text: timestamp
                                        color: "#7f8c8d"
                                        font.pixelSize: 10
                                        font.family: "monospace"
                                        Layout.preferredWidth: 110
                                    }
                                    Text {
                                        text: priorityLabel(priority)
                                        color: priorityColor(priority)
                                        font.pixelSize: 10
                                        font.bold: true
                                        Layout.preferredWidth: 54
                                    }
                                    Text {
                                        text: source
                                        color: root.highlightColor
                                        font.pixelSize: 10
                                        elide: Text.ElideRight
                                        Layout.preferredWidth: 110
                                    }
                                    Text {
                                        text: message
                                        color: root.textColor
                                        elide: Text.ElideRight
                                        Layout.fillWidth: true
                                    }
                                }
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
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
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
                RowLayout {
                    Text { text: "Hardware"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    BusyIndicator { running: firmwareViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
                    Button {
                        text: "🔄"
                        enabled: !firmwareViewModel.isBusy
                        onClicked: { hardwareViewModel.refresh(); firmwareViewModel.refresh() }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                    ColumnLayout {
                        width: parent.width; spacing: 20

                        Text { text: "🖥️ SISTEMA (SMBIOS)"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: smbiosGrid.implicitHeight + 20
                            color: root.cardColor; radius: 8; border.color: root.borderColor
                            GridLayout {
                                id: smbiosGrid
                                anchors.fill: parent; anchors.margins: 12
                                columns: 2; columnSpacing: 12; rowSpacing: 6
                                Text { text: "Fabricante:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: hardwareViewModel.smbios.vendor || "—"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                Text { text: "Modelo:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: hardwareViewModel.smbios.model || "—"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                Text { text: "Versão:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: hardwareViewModel.smbios.version || "—"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                Text { text: "Serial:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: hardwareViewModel.smbios.serial || "—"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight; font.family: "Monospace" }
                                Text { text: "SKU:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: hardwareViewModel.smbios.sku || "—"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                Text { text: "Placa-mãe:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                Text { text: (hardwareViewModel.smbios.boardVendor || "") + " " + (hardwareViewModel.smbios.boardName || ""); color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                            }
                        }

                        Text { text: "🔌 USB"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 180
                            color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                id: usbList
                                anchors.fill: parent; anchors.margins: 10; clip: true
                                model: hardwareViewModel.usbDevices
                                ScrollBar.vertical: ScrollBar {}
                                delegate: RowLayout {
                                    width: usbList.width; height: 28; spacing: 12
                                    Text { text: (modelData.vendorId || "") + ":" + (modelData.productId || ""); color: "#7f8c8d"; font.family: "Monospace"; font.pixelSize: 11; Layout.preferredWidth: 110 }
                                    Text { text: modelData.name || "(sem nome)"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                    Text { text: "bus " + (modelData.bus || "—"); color: "#7f8c8d"; font.pixelSize: 11 }
                                }
                            }
                        }

                        Text { text: "📡 BLUETOOTH"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 140
                            color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                id: btList
                                anchors.fill: parent; anchors.margins: 10; clip: true
                                model: hardwareViewModel.bluetoothDevices
                                ScrollBar.vertical: ScrollBar {}
                                delegate: RowLayout {
                                    width: btList.width; height: 28; spacing: 12
                                    Rectangle { width: 8; height: 8; radius: 4; color: modelData.paired ? "#2ecc71" : "#95a5a6" }
                                    Text { text: modelData.name || "(sem nome)"; color: root.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
                                    Text { text: modelData.address || ""; color: "#7f8c8d"; font.family: "Monospace"; font.pixelSize: 11 }
                                }
                            }
                        }

                        Text { text: "🎮 GPU"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 160
                            color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                id: gpuList
                                anchors.fill: parent; anchors.margins: 10; clip: true
                                model: gpuViewModel.gpus
                                ScrollBar.vertical: ScrollBar {}
                                delegate: ColumnLayout {
                                    width: gpuList.width; spacing: 2
                                    RowLayout {
                                        Layout.fillWidth: true; spacing: 10
                                        Text { text: (modelData.vendor || "—") + " · " + (modelData.modelName || "—"); color: root.textColor; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
                                        Text { text: (modelData.temp > 0 ? modelData.temp.toFixed(0) + " °C" : "—"); color: "#7f8c8d"; font.family: "Monospace"; font.pixelSize: 11 }
                                    }
                                    RowLayout {
                                        Layout.fillWidth: true; spacing: 10
                                        Text { text: "Carga:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                        Text { text: modelData.load.toFixed(0) + " %"; color: root.textColor; font.pixelSize: 11; Layout.preferredWidth: 60 }
                                        Text { text: "VRAM:"; color: "#7f8c8d"; font.pixelSize: 11 }
                                        Text {
                                            text: (modelData.vramTotal > 0)
                                                ? (modelData.vramUsed.toFixed(0) + " / " + modelData.vramTotal.toFixed(0) + " MiB (" + modelData.vramPercent.toFixed(0) + " %)")
                                                : "—"
                                            color: root.textColor; font.pixelSize: 11; Layout.fillWidth: true; elide: Text.ElideRight
                                        }
                                    }
                                    Rectangle { Layout.fillWidth: true; height: 1; color: root.borderColor; opacity: 0.4 }
                                }
                            }
                        }

                        Text { text: "🔧 FIRMWARE (fwupd)"; color: root.highlightColor; font.bold: true; font.pixelSize: 12 }
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 140
                            color: root.cardColor; radius: 8; border.color: root.borderColor
                            ListView {
                                id: fwList
                                anchors.fill: parent; anchors.margins: 10; clip: true
                                model: firmwareViewModel.devices
                                ScrollBar.vertical: ScrollBar {}
                                delegate: Text {
                                    width: fwList.width; height: 28
                                    text: modelData.name; color: root.textColor
                                    verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
                                }
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
                    BusyIndicator { running: devStackViewModel.isBusy; implicitWidth: 24; implicitHeight: 24; visible: running }
                    Button { text: "🔄"; onClicked: devStackViewModel.refresh(); enabled: !devStackViewModel.isBusy }
                }
                
                ScrollView {
                    Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
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
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
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
                                text: exportViewModel.defaultExportPath
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

            // ABA 9 - Storage
            ScrollView {
                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                clip: true
                ColumnLayout {
                    width: parent.width
                    anchors.margins: 25; spacing: 15

                    RowLayout {
                        Text { text: "Storage"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                        Item { Layout.fillWidth: true }
                        Button { text: "🔄"; onClicked: storageViewModel.refreshInventory() }
                    }

                    Repeater {
                        model: storageViewModel.drives
                        delegate: Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: driveColumn.implicitHeight + 30
                            color: root.cardColor; border.color: root.borderColor; radius: 8

                            ColumnLayout {
                                id: driveColumn
                                anchors.fill: parent; anchors.margins: 15; spacing: 10

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        text: (modelData.rotational ? "💿 " : "💾 ") + modelData.name
                                        color: root.highlightColor; font.pixelSize: 16; font.bold: true
                                    }
                                    Rectangle {
                                        id: smartBadge
                                        property var smart: modelData.smart
                                        readonly property string severity: {
                                            if (!smart || !smart.available) return "unavail";
                                            if (!smart.passed || smart.wearPct >= 90 || smart.tempC >= 70) return "risk";
                                            if (smart.wearPct >= 70 || smart.tempC >= 60) return "warn";
                                            return "ok";
                                        }
                                        readonly property string label: {
                                            switch (severity) {
                                                case "ok":      return "✓ Saudável";
                                                case "warn":    return "⚠ Atenção";
                                                case "risk":    return "✗ Risco";
                                                default:        return "SMART N/A";
                                            }
                                        }
                                        readonly property color fg: {
                                            switch (severity) {
                                                case "ok":      return "#2ecc71";
                                                case "warn":    return "#f39c12";
                                                case "risk":    return root.criticalColor;
                                                default:        return root.textColor;
                                            }
                                        }
                                        implicitWidth: badgeText.implicitWidth + 14
                                        implicitHeight: badgeText.implicitHeight + 6
                                        radius: 4
                                        color: Qt.rgba(fg.r, fg.g, fg.b, 0.15)
                                        border.color: fg
                                        border.width: 1
                                        Text {
                                            id: badgeText
                                            anchors.centerIn: parent
                                            text: smartBadge.label
                                            color: smartBadge.fg
                                            font.pixelSize: 10; font.bold: true
                                        }
                                        ToolTip.visible: smartMouse.containsMouse
                                        ToolTip.text: {
                                            if (!smartBadge.smart || !smartBadge.smart.available) {
                                                return smartBadge.smart && smartBadge.smart.errorMessage
                                                    ? "SMART indisponível: " + smartBadge.smart.errorMessage
                                                    : "SMART indisponível (smartctl ausente ou sem permissão)";
                                            }
                                            return "Temp: " + smartBadge.smart.tempC + "°C · "
                                                 + "Power-On: " + smartBadge.smart.powerOnHours + "h · "
                                                 + "Desgaste: " + smartBadge.smart.wearPct + "%";
                                        }
                                        MouseArea { id: smartMouse; anchors.fill: parent; hoverEnabled: true }
                                    }
                                    Item { Layout.fillWidth: true }
                                    Text {
                                        text: root.formatBytes(modelData.sizeBytes)
                                        color: root.textColor; font.pixelSize: 13
                                    }
                                }
                                Text {
                                    visible: modelData.vendor.length > 0 || modelData.model.length > 0
                                    text: (modelData.vendor + " " + modelData.model).trim()
                                    color: root.textColor; opacity: 0.75; font.pixelSize: 11
                                }
                                Text {
                                    visible: modelData.smart && modelData.smart.available
                                    text: modelData.smart
                                        ? "🌡 " + modelData.smart.tempC + "°C  ·  ⏱ " + modelData.smart.powerOnHours + "h  ·  📉 desgaste " + modelData.smart.wearPct + "%"
                                        : ""
                                    color: root.textColor; opacity: 0.65; font.pixelSize: 10
                                }

                                Repeater {
                                    model: modelData.partitions
                                    delegate: ColumnLayout {
                                        Layout.fillWidth: true; spacing: 2

                                        RowLayout {
                                            Layout.fillWidth: true
                                            Text {
                                                text: modelData.name + (modelData.mount.length > 0 ? "  @  " + modelData.mount : "  (não montado)")
                                                color: root.textColor; font.pixelSize: 12; font.bold: true
                                            }
                                            Item { Layout.fillWidth: true }
                                            Text {
                                                text: modelData.fs.length > 0 ? modelData.fs : "—"
                                                color: root.textColor; opacity: 0.6; font.pixelSize: 11
                                            }
                                        }
                                        RowLayout {
                                            Layout.fillWidth: true
                                            ProgressBar {
                                                Layout.fillWidth: true
                                                from: 0; to: 100; value: modelData.usedPercent
                                                background: Rectangle { color: root.isDark ? "#2a2e32" : "#e0e0e0"; radius: 4; implicitHeight: 8 }
                                                contentItem: Item {
                                                    Rectangle {
                                                        width: parent.width * (modelData.usedPercent / 100.0)
                                                        height: parent.height
                                                        radius: 4
                                                        color: modelData.usedPercent >= 90 ? root.criticalColor
                                                             : modelData.usedPercent >= 70 ? "#f39c12"
                                                             : "#2ecc71"
                                                    }
                                                }
                                            }
                                            Text {
                                                text: modelData.mount.length > 0
                                                    ? root.formatBytes(modelData.usedBytes) + " / " + root.formatBytes(modelData.sizeBytes) + "  (" + modelData.usedPercent.toFixed(1) + "%)"
                                                    : root.formatBytes(modelData.sizeBytes)
                                                color: root.textColor; opacity: 0.75; font.pixelSize: 11
                                            }
                                        }
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true; Layout.topMargin: 8
                                    Button {
                                        text: storageViewModel.isBenchmarking ? "⏳ Executando…" : "⚡ Benchmark (hdparm)"
                                        enabled: !storageViewModel.isBenchmarking
                                        onClicked: benchmarkConfirmDialog.open()

                                        Dialog {
                                            id: benchmarkConfirmDialog
                                            modal: true
                                            anchors.centerIn: parent
                                            title: "Executar benchmark em /dev/" + modelData.name + "?"
                                            standardButtons: Dialog.Ok | Dialog.Cancel
                                            Text {
                                                text: "hdparm -tT roda leitura apenas, mas toca o cache do sistema.\nRequer root/pkexec para acesso ao device bruto."
                                                color: root.textColor
                                                wrapMode: Text.WordWrap
                                                width: 360
                                            }
                                            onAccepted: storageViewModel.runBenchmark(modelData.name)
                                        }
                                    }
                                    Text {
                                        visible: storageViewModel.lastBenchmark.device === modelData.name
                                              && storageViewModel.lastBenchmark.success === true
                                        text: storageViewModel.lastBenchmark.device === modelData.name
                                            ? "Seq: " + (storageViewModel.lastBenchmark.seqReadMbS || 0).toFixed(1) + " MB/s · "
                                            + "Cache: " + (storageViewModel.lastBenchmark.cachedMbS || 0).toFixed(1) + " MB/s"
                                            : ""
                                        color: root.highlightColor; font.pixelSize: 11
                                    }
                                    Text {
                                        visible: storageViewModel.lastBenchmark.device === modelData.name
                                              && storageViewModel.lastBenchmark.running !== true
                                              && storageViewModel.lastBenchmark.success === false
                                              && (storageViewModel.lastBenchmark.errorMessage || "").length > 0
                                        text: "⚠ " + (storageViewModel.lastBenchmark.errorMessage || "")
                                        color: root.criticalColor; font.pixelSize: 11; Layout.fillWidth: true; wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }
                    }

                    // Seção Scanner de Diretório (ST-05)
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: scannerColumn.implicitHeight + 30
                        color: root.cardColor; border.color: root.borderColor; radius: 8

                        ColumnLayout {
                            id: scannerColumn
                            anchors.fill: parent; anchors.margins: 15; spacing: 10

                            Text { text: "🔎 Scanner de Diretório"; color: root.highlightColor; font.pixelSize: 15; font.bold: true }
                            Text {
                                text: "Varre recursivamente uma pasta e lista os 20 subdiretórios mais pesados. Pseudo-filesystems (/proc, /sys, /run, /dev) e symlinks são ignorados."
                                color: root.textColor; opacity: 0.7; font.pixelSize: 10; wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                TextField {
                                    id: scanPathInput
                                    Layout.fillWidth: true
                                    placeholderText: "/home/" + Qt.application.arguments[0].split("/")[2]
                                    text: "/home"
                                }
                                Button {
                                    text: directoryScannerViewModel.isScanning ? "Cancelar" : "Varrer"
                                    onClicked: {
                                        if (directoryScannerViewModel.isScanning) {
                                            directoryScannerViewModel.cancel();
                                        } else if (scanPathInput.text.length > 0) {
                                            directoryScannerViewModel.scan(scanPathInput.text);
                                        }
                                    }
                                }
                            }
                            RowLayout {
                                visible: directoryScannerViewModel.isScanning
                                Layout.fillWidth: true
                                BusyIndicator { running: true; implicitWidth: 20; implicitHeight: 20 }
                                Text {
                                    text: directoryScannerViewModel.filesScanned + " arquivos · "
                                        + root.formatBytes(directoryScannerViewModel.bytesScanned)
                                    color: root.textColor; font.pixelSize: 11
                                }
                                Text {
                                    text: "▸ " + directoryScannerViewModel.currentPath
                                    color: root.textColor; opacity: 0.6; font.pixelSize: 10
                                    elide: Text.ElideLeft; Layout.fillWidth: true
                                }
                            }
                            Text {
                                visible: !directoryScannerViewModel.isScanning && directoryScannerViewModel.scannedRoot.length > 0
                                text: directoryScannerViewModel.scannedRoot + ": " + root.formatBytes(directoryScannerViewModel.totalSize)
                                color: root.textColor; font.pixelSize: 12; font.bold: true
                            }
                            Repeater {
                                model: directoryScannerViewModel.topEntries
                                delegate: RowLayout {
                                    Layout.fillWidth: true
                                    Text {
                                        text: modelData.error ? "⚠ " + modelData.name : modelData.name
                                        color: modelData.error ? root.criticalColor : root.textColor
                                        font.pixelSize: 11; font.bold: !modelData.error
                                        Layout.preferredWidth: 200; elide: Text.ElideMiddle
                                    }
                                    ProgressBar {
                                        Layout.fillWidth: true
                                        from: 0
                                        to: directoryScannerViewModel.totalSize > 0 ? directoryScannerViewModel.totalSize : 1
                                        value: modelData.sizeBytes
                                        background: Rectangle { color: root.isDark ? "#2a2e32" : "#e0e0e0"; radius: 3; implicitHeight: 6 }
                                        contentItem: Item {
                                            Rectangle {
                                                width: parent.width * (directoryScannerViewModel.totalSize > 0 ? (modelData.sizeBytes / directoryScannerViewModel.totalSize) : 0)
                                                height: parent.height
                                                radius: 3; color: root.highlightColor
                                            }
                                        }
                                    }
                                    Text {
                                        text: root.formatBytes(modelData.sizeBytes)
                                        color: root.textColor; font.pixelSize: 10
                                        Layout.preferredWidth: 90; horizontalAlignment: Text.AlignRight
                                    }
                                    Text {
                                        text: modelData.fileCount + " arq · " + modelData.dirCount + " dir"
                                        color: root.textColor; opacity: 0.6; font.pixelSize: 10
                                        Layout.preferredWidth: 120
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // ABA 10 - Installation
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
