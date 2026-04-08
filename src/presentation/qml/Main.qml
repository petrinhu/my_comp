import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 1024
    height: 768
    visible: true
    title: qsTr("my_comp v2.0 Alpha")
    
    // --- Sistema de Temas ---
    property int themeMode: 2 // Iniciar em Auto Mode
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
    readonly property color highlightColor: "#3daee9"
    readonly property color criticalColor: "#ed1515"

    property int currentView: 0

    CpuDetailDialog { id: cpuDetailDialog }

    RowLayout {
        anchors.fill: parent
        spacing: 25

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
                    
                    ToolTip.visible: hovered
                    ToolTip.text: "Alternar entre Light, Dark e Auto (Sistema)"
                    ToolTip.delay: 500

                    background: Rectangle {
                        color: themeToggle.pressed ? (root.isDark ? "#454a4f" : "#d0d0d0") : 
                               themeToggle.hovered ? (root.isDark ? "#3e444a" : "#e8e8e8") : 
                               (root.isDark ? "#2a2e32" : "#f5f5f5")
                        radius: 6
                        border.color: themeToggle.hovered || themeToggle.pressed ? root.highlightColor : (root.isDark ? "#4d4d4d" : "#dbdbdb")
                        border.width: 1
                        
                        Behavior on color { ColorAnimation { duration: 100 } }
                        
                        scale: themeToggle.pressed ? 0.98 : 1.0
                        Behavior on scale { NumberAnimation { duration: 50 } }
                    }

                    contentItem: RowLayout {
                        spacing: 10
                        Text { 
                            text: root.themeMode === 0 ? "☀️" : root.themeMode === 1 ? "🌙" : "🌓"
                            font.pixelSize: 16
                            color: root.textColor
                        }
                        Text { 
                            text: root.themeMode === 0 ? "Light Mode" : root.themeMode === 1 ? "Dark Mode" : "Auto Mode"
                            color: root.textColor
                            font.bold: true 
                        }
                    }
                }

                Rectangle { 
                    Layout.fillWidth: true
                    height: 1
                    color: root.borderColor
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10 
                }

                Repeater {
                    model: [
                        {name: "Dashboard", icon: "📊", tip: "Visão geral do sistema em tempo real"},
                        {name: "Updates", icon: "📦", tip: "Gerenciar pacotes DNF, Flatpak e NPM"},
                        {name: "Journal", icon: "📜", tip: "Analisar logs e erros críticos do sistema"},
                        {name: "Network", icon: "🌐", tip: "Monitoramento detalhado de conexões e rotas"},
                        {name: "Hardware", icon: "💻", tip: "Topologia completa de dispositivos e portas"},
                        {name: "Security", icon: "🛡️", tip: "Auditoria de Firewall, SELinux e Logins"},
                        {name: "Export", icon: "📤", tip: "Gerar relatórios em MD, JSON ou HTML"}
                    ]
                    delegate: ItemDelegate {
                        id: navDelegate
                        Layout.fillWidth: true
                        highlighted: root.currentView === index
                        onClicked: { 
                            root.currentView = index
                            if (index === 1 && updateViewModel.totalUpdates === 0) updateViewModel.refresh() 
                        }
                        
                        ToolTip.visible: hovered
                        ToolTip.text: modelData.tip
                        ToolTip.delay: 500

                        background: Rectangle {
                            color: navDelegate.pressed ? (root.isDark ? "#454a4f" : "#d0d0d0") :
                                   navDelegate.highlighted ? root.highlightColor : 
                                   navDelegate.hovered ? (root.isDark ? "#3e444a" : "#e8e8e8") : 
                                   (root.isDark ? "#2a2e32" : "#f5f5f5")
                            radius: 6
                            opacity: navDelegate.highlighted ? 0.2 : 1.0
                            border.color: navDelegate.highlighted || navDelegate.hovered ? root.highlightColor : (root.isDark ? "#4d4d4d" : "#dbdbdb")
                            border.width: 1
                            
                            Behavior on color { ColorAnimation { duration: 100 } }

                            scale: navDelegate.pressed ? 0.98 : 1.0
                            Behavior on scale { NumberAnimation { duration: 50 } }
                        }

                        contentItem: RowLayout { 
                            spacing: 12
                            Text { 
                                text: modelData.icon
                                font.pixelSize: 18
                                color: navDelegate.highlighted ? root.highlightColor : root.textColor
                            }
                            Text { 
                                text: modelData.name
                                color: navDelegate.highlighted ? root.highlightColor : root.textColor
                                font.bold: navDelegate.highlighted
                            } 
                        }
                    }
                }
                Item { Layout.fillHeight: true }
                ColumnLayout { 
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 2
                    Text { text: "Agnóstico a Distro"; color: "#7f8c8d"; font.pixelSize: 10 }
                    Text { text: "v2.0 Alpha"; color: root.highlightColor; font.pixelSize: 10; font.bold: true } 
                }
            }
        }

        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.currentView

            // ABA 0
            ScrollView {
                ColumnLayout {
                    width: parent.width
                    anchors.margins: 25
                    spacing: 25
                    Text { text: "Mission Control"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        rowSpacing: 20
                        columnSpacing: 20
                        DashboardCard { 
                            id: cpuCard
                            title: "CPU LOAD"
                            value: cpuViewModel.totalLoad.toFixed(1) + "%"
                            subValue: (cpuViewModel.avgFrequency / 1000.0).toFixed(2) + " GHz"
                            valueColor: root.highlightColor
                            onClicked: cpuDetailDialog.open()
                        }
                        DashboardCard { 
                            id: memCard
                            title: "MEMORY USAGE"
                            value: memoryViewModel.ramUsagePercent.toFixed(1) + "%"
                            subValue: memoryViewModel.ramUsed + " / " + memoryViewModel.ramTotal
                            valueColor: memoryViewModel.ramUsagePercent > 90 ? root.criticalColor : root.highlightColor
                        }
                        DashboardCard { 
                            id: ioCard
                            title: "I/O THROUGHPUT"
                            value: storageViewModel.totalThroughput
                            subValue: "Read/Write Active"
                            valueColor: root.highlightColor
                        }
                        DashboardCard { 
                            id: netCard
                            title: "NETWORK QUALITY"
                            value: networkViewModel.latency > 0 ? networkViewModel.latency.toFixed(0) + " ms" : "---"
                            subValue: networkViewModel.status + (networkViewModel.activeInterface ? " (" + networkViewModel.activeInterface + ")" : "")
                            valueColor: networkViewModel.latency > 0 && networkViewModel.latency < 100 ? "#2ecc71" : 
                                        networkViewModel.latency >= 100 && networkViewModel.latency < 300 ? "#f1c40f" : root.criticalColor
                        }
                        DashboardCard { 
                            id: tempCard
                            title: "THERMAL HEATMAP"
                            value: cpuViewModel.temperature.toFixed(0) + "°C"
                            subValue: "CPU Package"
                            valueColor: cpuViewModel.temperature > 80 ? root.criticalColor : root.highlightColor
                        }
                        DashboardCard { 
                            id: swapCard
                            title: "SWAP / ZRAM"
                            value: memoryViewModel.swapUsagePercent.toFixed(1) + "%"
                            subValue: memoryViewModel.hasZram ? "ZRAM Active (x" + memoryViewModel.zramCompressionRatio.toFixed(1) + ")" : "Standard Swap"
                            valueColor: "#9b59b6"
                        }
                    }
                }
            }

            // ABA 1
            ColumnLayout {
                id: updatesLayout
                anchors.margins: 25
                Layout.topMargin: 45
                spacing: 15
                RowLayout { 
                    Text { text: "Central de Atualizações"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { 
                        text: "Atualizar Tudo"
                        highlighted: true
                        onClicked: {
                            updateViewModel.upgradeAll()
                            root.currentView = 7 // Muda para a aba de terminal
                        }
                    } 
                    Button { text: "Verificar Agora"; highlighted: true; onClicked: updateViewModel.refresh() } 
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ListView {
                        id: updateList
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: updateViewModel.updateModel
                        ScrollBar.vertical: ScrollBar { 
                            policy: ScrollBar.AlwaysOn
                            contentItem: Rectangle { implicitWidth: 6; radius: 3; color: root.isDark ? "#4d4d4d" : "#bcbebf" }
                        }
                        delegate: ItemDelegate {
                            width: updateList.width; height: 65
                            onDoubleClicked: {
                                let providerInt = 0
                                if (source === "flatpak") providerInt = 1
                                else if (source === "npm") providerInt = 2
                                else if (source === "snap") providerInt = 3
                                
                                installationViewModel.startInstallation(name, providerInt)
                                root.currentView = 7 // Muda para a aba de terminal dedicada
                            }
                            contentItem: RowLayout {
                                spacing: 15
                                Rectangle { 
                                    width: 40; height: 40; radius: 20
                                    color: isCritical ? root.criticalColor : root.highlightColor
                                    Text { anchors.centerIn: parent; text: source.charAt(0).toUpperCase(); color: "white"; font.bold: true } 
                                }
                                ColumnLayout { 
                                    spacing: 2
                                    Text { text: name; color: root.textColor; font.bold: true }
                                    Text { text: source + " • " + description; color: "#95a5a6"; font.pixelSize: 12 } 
                                }
                                Item { Layout.fillWidth: true }
                                Text { text: newVersion; color: root.highlightColor; font.bold: true }
                            }
                        }
                        Text { anchors.centerIn: parent; visible: updateList.count === 0 && !updateViewModel.isBusy; text: "Sistema íntegro e atualizado."; color: "#95a5a6" }
                        BusyIndicator { anchors.centerIn: parent; running: updateViewModel.isBusy }
                    }
                }
            }

            // ABA 2
            ColumnLayout {
                id: journalLayout
                anchors.margins: 25
                spacing: 15
                RowLayout { 
                    Text { text: "Analisador de Journal"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    TextField {
                        id: searchField
                        placeholderText: "Buscar no Journal..."
                        Layout.preferredWidth: 300
                        onTextChanged: diagnosticViewModel.searchQuery = text
                        Keys.onReturnPressed: diagnosticViewModel.performSearch()
                    }
                    Button { text: "Buscar"; highlighted: true; onClicked: diagnosticViewModel.performSearch() }
                    Button { text: "Reset"; onClicked: diagnosticViewModel.refresh() } 
                }
                ProgressBar {
                    Layout.fillWidth: true
                    value: diagnosticViewModel.searchProgress
                    visible: diagnosticViewModel.isBusy
                }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ListView {
                        id: journalList
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: diagnosticViewModel.logModel
                        delegate: ItemDelegate {
                            width: journalList.width; height: 50
                            contentItem: RowLayout {
                                spacing: 15
                                Rectangle { width: 8; Layout.fillHeight: true; color: root.criticalColor; radius: 4 }
                                ColumnLayout { 
                                    spacing: 2
                                    Text { text: message; color: root.textColor; font.pixelSize: 13; elide: Text.ElideRight; Layout.fillWidth: true }
                                    Text { text: timestamp + " • " + source; color: "#7f8c8d"; font.pixelSize: 11 } 
                                }
                            }
                        }
                        Text { anchors.centerIn: parent; visible: journalList.count === 0 && !diagnosticViewModel.isBusy; text: "Nenhum erro crítico detectado."; color: "#95a5a6" }
                        BusyIndicator { anchors.centerIn: parent; running: diagnosticViewModel.isBusy }
                    }
                }
            }

            // ABA 2 (Journal) continua aqui...
            // ... (vou manter o código do Journal intacto)

            // ABA 3 - Network (Movida para antes do Hardware)
            PlaceholderView { title: "Dashboard de Rede"; icon: "🌐"; desc: "Monitoramento em tempo real de interfaces, rotas e tráfego." }

            // ABA 4 - Hardware
            ColumnLayout {
                id: hardwareLayout
                anchors.margins: 25
                spacing: 15
                RowLayout { 
                    Text { text: "Topologia de Hardware"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                    Item { Layout.fillWidth: true }
                    Button { text: "Atualizar Hardware"; highlighted: true; onClicked: firmwareViewModel.refresh() } 
                }
                Text { text: "Dispositivos de Firmware (LVFS/fwupd)"; color: root.highlightColor; font.bold: true; Layout.topMargin: 10 }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ListView {
                        id: firmwareList
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: firmwareViewModel.devices
                        ScrollBar.vertical: ScrollBar { 
                            policy: ScrollBar.AsNeeded
                            contentItem: Rectangle { implicitWidth: 6; radius: 3; color: root.isDark ? "#4d4d4d" : "#bcbebf" }
                        }
                        delegate: ItemDelegate {
                            width: firmwareList.width; height: 60
                            background: Rectangle { color: "transparent" }
                            contentItem: RowLayout {
                                spacing: 15
                                ColumnLayout { 
                                    spacing: 2
                                    Text { text: modelData.name; color: root.textColor; font.bold: true; elide: Text.ElideRight; Layout.fillWidth: true }
                                    Text { text: (modelData.vendor ? modelData.vendor + " • " : "") + "ID: " + modelData.id; color: "#7f8c8d"; font.pixelSize: 11; elide: Text.ElideRight; Layout.fillWidth: true } 
                                }
                                Item { Layout.fillWidth: true }
                                Rectangle {
                                    color: root.isDark ? "#2a2e32" : "#f5f5f5"
                                    radius: 4
                                    width: 100; height: 25
                                    Text { 
                                        anchors.centerIn: parent
                                        text: modelData.version
                                        color: root.highlightColor
                                        font.family: "Monospace"
                                        font.pixelSize: 11
                                    }
                                }
                            }
                        }
                        Text { anchors.centerIn: parent; visible: firmwareList.count === 0 && !firmwareViewModel.isBusy; text: "Nenhum dispositivo de firmware detectado."; color: "#95a5a6" }
                        BusyIndicator { anchors.centerIn: parent; running: firmwareViewModel.isBusy }
                    }
                }
                Item { Layout.fillHeight: true }
            }

            // ABA 5 e 6 - Placeholders restantes
            PlaceholderView { title: "Auditoria de Segurança"; icon: "🛡️"; desc: "SELinux, Firewall, Portas Abertas e Logins." }
            PlaceholderView { title: "Exportar Relatórios"; icon: "📤"; desc: "Paridade total com legado (MD, HTML, JSON)." }

            // ABA 7 - Terminal de Instalação (Acessada via Double Click)
            InstallationView {
                id: installView
                anchors.margins: 25
            }
        }
    }

    component DashboardCard : Rectangle {
        id: cardRoot
        property string title
        property string value
        property string subValue
        property color valueColor: root.highlightColor
        signal clicked

        Layout.fillWidth: true
        Layout.minimumWidth: 150
        Layout.preferredHeight: 140
        radius: 12
        color: root.isDark ? "#1a1c1e" : "#ffffff"
        border.color: cardMouseArea.containsPress ? root.highlightColor : (root.isDark ? "#4d4d4d" : root.borderColor)
        border.width: 2

        scale: cardMouseArea.containsPress ? 0.98 : 1.0
        Behavior on scale { NumberAnimation { duration: 50 } }

        MouseArea {
            id: cardMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: cardRoot.clicked()
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 5
            
            Text { 
                text: title
                color: root.textColor
                font.pixelSize: 13
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }
            
            Text { 
                color: valueColor
                text: cardRoot.value
                font.pixelSize: 34
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
                fontSizeMode: Text.Fit
                minimumPixelSize: 18
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Text { 
                text: subValue
                color: root.isDark ? "#aab2b7" : "#7f8c8d"
                font.pixelSize: 11
                Layout.alignment: Qt.AlignHCenter
                elide: Text.ElideRight
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    component PlaceholderView : ColumnLayout {
        property string title; property string icon; property string desc
        anchors.margins: 50; spacing: 20
        Text { text: icon; font.pixelSize: 80; Layout.alignment: Qt.AlignHCenter }
        Text { text: title; color: root.textColor; font.pixelSize: 28; font.bold: true; Layout.alignment: Qt.AlignHCenter }
        Text { 
            text: desc
            color: root.isDark ? "#bdc3c7" : "#7f8c8d"
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
        }
        Item { Layout.fillHeight: true }
        Button { text: "Funcionalidade em Desenvolvimento"; enabled: false; Layout.alignment: Qt.AlignHCenter }
    }
}
