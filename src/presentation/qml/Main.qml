import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

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
    readonly property color highlightColor: "#3daee9"
    readonly property color criticalColor: "#ed1515"

    property int currentView: 0

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
        currentFolder: "file:///usr/bin"
        onAccepted: {
            // Converter URL para caminho local
            let path = selectedFile.toString()
            if (path.startsWith("file://")) path = path.substring(7)
            exePathField.text = path
        }
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
                        {name: "Security", icon: "🛡️"},
                        {name: "Export", icon: "📤"}
                    ]
                    delegate: ItemDelegate {
                        id: navDelegate
                        Layout.fillWidth: true
                        highlighted: root.currentView === index
                        onClicked: root.currentView = index
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
                        DashboardCard { title: "MEM"; value: memoryViewModel.ramUsagePercent.toFixed(1) + "%" }
                        DashboardCard { title: "DISK"; value: storageViewModel.totalThroughput }
                        DashboardCard { title: "NET"; value: networkViewModel.latency.toFixed(0) + "ms" }
                        DashboardCard { title: "POWER"; value: powerViewModel.batteryPercent.toFixed(0) + "%" }
                        DashboardCard { title: "CORE"; value: systemInfoViewModel.kernel.split("-")[0] }
                    }
                }
            }

            // ABA 1 - Updates
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                Text { text: "Central de Atualizações"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ListView {
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: updateViewModel.updateModel
                        delegate: ItemDelegate {
                            width: parent.width; height: 50
                            contentItem: RowLayout {
                                Text { text: name; color: root.textColor; font.bold: true; Layout.fillWidth: true }
                                Text { text: newVersion; color: root.highlightColor }
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
                    ListView {
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: diagnosticViewModel.logModel
                        delegate: ItemDelegate {
                            width: parent.width; height: 40
                            contentItem: Text { text: message; color: root.textColor; elide: Text.ElideRight }
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
                    ListView {
                        id: serviceList
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: serviceViewModel.services
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
                    ListView {
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: firmwareViewModel.devices
                        delegate: ItemDelegate {
                            width: parent.width; height: 40
                            contentItem: Text { text: modelData.name; color: root.textColor }
                        }
                    }
                }
            }

            // ABA 6 - Security
            ColumnLayout {
                anchors.margins: 25; spacing: 15
                Text { text: "Segurança"; color: root.textColor; font.pixelSize: 24; font.bold: true }
                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true; color: root.cardColor; radius: 8; border.color: root.borderColor
                    ListView {
                        anchors.fill: parent; anchors.margins: 10; clip: true; model: userMetadataViewModel.loginHistory
                        delegate: Text { text: modelData; color: root.textColor; font.family: "Monospace"; font.pixelSize: 10 }
                    }
                }
            }

            // ABA 7 - Export
            PlaceholderView { title: "Exportar"; icon: "📤"; desc: "Gerar relatórios." }

            // ABA 8 - Installation
            InstallationView { id: installView; anchors.margins: 25 }
        }
    }

    component DashboardCard : Rectangle {
        property string title; property string value; signal clicked
        Layout.fillWidth: true; Layout.preferredHeight: 100; radius: 10
        color: root.cardColor; border.color: root.borderColor
        MouseArea { anchors.fill: parent; onClicked: parent.clicked() }
        ColumnLayout {
            anchors.centerIn: parent
            Text { text: title; color: root.textColor; font.pixelSize: 12; Layout.alignment: Qt.AlignHCenter }
            Text { text: value; color: root.highlightColor; font.pixelSize: 24; font.bold: true; Layout.alignment: Qt.AlignHCenter }
        }
    }

    component PlaceholderView : ColumnLayout {
        property string title; property string icon; property string desc
        anchors.centerIn: parent; spacing: 10
        Text { text: icon; font.pixelSize: 48; Layout.alignment: Qt.AlignHCenter }
        Text { text: title; color: root.textColor; font.pixelSize: 20; font.bold: true; Layout.alignment: Qt.AlignHCenter }
        Text { text: desc; color: "#7f8c8d"; Layout.alignment: Qt.AlignHCenter }
    }
}
