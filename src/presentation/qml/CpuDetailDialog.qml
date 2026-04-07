import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: cpuDetailRoot
    title: "CPU Deep Dive"
    width: 700
    height: 600
    modal: true
    anchors.centerIn: Overlay.overlay
    
    background: Rectangle {
        color: root.isDark ? "#2a2e32" : "#ffffff"
        radius: 12
        border.color: root.borderColor
        border.width: 1
    }

    header: Rectangle {
        height: 60
        color: root.isDark ? "#1a1c1e" : "#f8f9fa"
        radius: 12
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 25
            anchors.rightMargin: 15
            
            Text { 
                text: "📊 Detalhes da CPU"
                color: root.textColor
                font.pixelSize: 18
                font.bold: true 
                Layout.alignment: Qt.AlignVCenter
            }
            
            Item { Layout.fillWidth: true }
            
            Button { 
                id: closeBtn
                text: "✖"
                flat: true
                onClicked: cpuDetailRoot.close()
                Layout.alignment: Qt.AlignVCenter
                
                contentItem: Text { 
                    text: "✖"
                    color: closeBtn.hovered ? root.criticalColor : root.textColor
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    contentItem: ColumnLayout {
        spacing: 15
        anchors.margins: 25

        // --- SEÇÃO 1: ARQUITETURA (FIXA) ---
        Text { 
            text: "ARQUITETURA E CACHE"
            color: root.highlightColor
            font.pixelSize: 12
            font.bold: true 
            leftPadding: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: root.isDark ? "#1a1c1e" : "#fdfdfd"
            radius: 8
            border.color: root.borderColor
            border.width: 1

            GridLayout {
                columns: 2
                anchors.fill: parent
                anchors.margins: 20
                rowSpacing: 12
                columnSpacing: 40
                
                LabelValue { label: "Modelo:"; value: cpuViewModel.modelName }
                LabelValue { label: "Fabricante:"; value: cpuViewModel.vendorId }
                LabelValue { label: "Threads Lógicas:"; value: cpuViewModel.logicalThreads.toString() }
                LabelValue { label: "Temperatura:"; value: cpuViewModel.temperature.toFixed(1) + "°C" }
                LabelValue { label: "L1 Cache:"; value: cpuViewModel.l1Cache }
                LabelValue { label: "L2 Cache:"; value: cpuViewModel.l2Cache }
                LabelValue { label: "L3 Cache:"; value: cpuViewModel.l3Cache }
                LabelValue { label: "Cores Físicos:"; value: cpuViewModel.physicalCores.toString() }
            }
        }

        Item { Layout.preferredHeight: 10 } // Espaçador

        // --- SEÇÃO 2: MONITORAMENTO (FIXA / CONTEÚDO ROLÁVEL) ---
        Text { 
            text: "MONITORAMENTO EM TEMPO REAL (POR CORE)"
            color: root.highlightColor
            font.pixelSize: 12
            font.bold: true 
            leftPadding: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: root.isDark ? "#1a1c1e" : "#fdfdfd"
            radius: 8
            border.color: root.borderColor
            border.width: 1
            clip: true

            ScrollView {
                id: cpuScroll
                anchors.fill: parent
                anchors.margins: 15
                clip: true
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                    active: true
                    contentItem: Rectangle {
                        implicitWidth: 6
                        radius: 3
                        color: root.isDark ? "#4d4d4d" : "#bcbebf"
                    }
                }
                
                GridLayout {
                    columns: 2
                    width: cpuScroll.availableWidth
                    rowSpacing: 18
                    columnSpacing: 35
                    
                    Repeater {
                        model: cpuViewModel.coreLoads
                        delegate: RowLayout {
                            Layout.fillWidth: true
                            spacing: 12
                            
                            Text { 
                                text: "C" + modelData.id
                                color: root.textColor
                                font.pixelSize: 11
                                font.bold: true
                                width: 25
                            }
                            
                            ColumnLayout {
                                spacing: 4
                                Layout.fillWidth: true
                                
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 8
                                    color: root.isDark ? "#3e444a" : "#e0e0e0"
                                    radius: 4
                                    
                                    Rectangle {
                                        width: parent.width * (modelData.load / 100.0)
                                        height: parent.height
                                        radius: 4
                                        color: modelData.load > 85 ? root.criticalColor : root.highlightColor
                                        Behavior on width { NumberAnimation { duration: 400 } }
                                    }
                                }
                                
                                RowLayout {
                                    Text { 
                                        text: modelData.load.toFixed(1) + "%"
                                        color: root.textColor
                                        font.pixelSize: 10
                                    }
                                    Item { Layout.fillWidth: true }
                                    Text { 
                                        text: (modelData.freq / 1000.0).toFixed(2) + " GHz"
                                        color: "#7f8c8d"
                                        font.pixelSize: 10
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    component LabelValue : RowLayout {
        property string label
        property string value
        spacing: 10
        Text { 
            text: label
            color: "#7f8c8d"
            font.pixelSize: 12
            Layout.preferredWidth: 100
        }
        Text { 
            text: value
            color: root.textColor
            font.pixelSize: 12
            font.bold: true
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
    }
}
