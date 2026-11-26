import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Diálogo "CPU Deep Dive": arquitetura/cache e carga por core em tempo real.
Dialog {
    id: cpuDetailRoot
    title: qsTr("CPU Deep Dive")
    width: 700
    height: 600
    modal: true
    anchors.centerIn: Overlay.overlay
    
    background: Rectangle {
        color: Theme.dialogBg
        radius: 12
        border.color: Theme.borderColor
        border.width: 1
    }

    header: Rectangle {
        height: 60
        color: Theme.dialogHeaderBg
        radius: 12
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 25
            anchors.rightMargin: 15
            
            Text { 
                text: qsTr("📊 Detalhes da CPU")
                color: Theme.textColor
                font.pixelSize: 18
                font.bold: true 
                Layout.alignment: Qt.AlignVCenter
            }
            
            Item { Layout.fillWidth: true }
            
            IconButton {
                id: closeBtn
                text: "✖"
                label: qsTr("Fechar detalhes da CPU")
                flat: true
                onClicked: cpuDetailRoot.close()
                Layout.alignment: Qt.AlignVCenter
                
                contentItem: Text { 
                    text: "✖"
                    color: closeBtn.hovered ? Theme.criticalColor : Theme.textColor
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
            text: qsTr("ARQUITETURA E CACHE")
            color: Theme.highlightColor
            font.pixelSize: 12
            font.bold: true 
            leftPadding: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: Theme.dialogSectionBg
            radius: 8
            border.color: Theme.borderColor
            border.width: 1

            GridLayout {
                columns: 2
                anchors.fill: parent
                anchors.margins: 20
                rowSpacing: 12
                columnSpacing: 40
                
                LabelValue { label: qsTr("Modelo:"); value: cpuViewModel.modelName }
                LabelValue { label: qsTr("Fabricante:"); value: cpuViewModel.vendorId }
                LabelValue { label: qsTr("Threads Lógicas:"); value: cpuViewModel.logicalThreads.toString() }
                LabelValue { label: qsTr("Temperatura:"); value: cpuViewModel.temperature.toFixed(1) + "°C" }
                LabelValue { label: qsTr("L1 Cache:"); value: cpuViewModel.l1Cache }
                LabelValue { label: qsTr("L2 Cache:"); value: cpuViewModel.l2Cache }
                LabelValue { label: qsTr("L3 Cache:"); value: cpuViewModel.l3Cache }
                LabelValue { label: qsTr("Cores Físicos:"); value: cpuViewModel.physicalCores.toString() }
            }
        }

        Item { Layout.preferredHeight: 10 } // Espaçador

        // --- SEÇÃO 2: MONITORAMENTO (FIXA / CONTEÚDO ROLÁVEL) ---
        Text { 
            text: qsTr("MONITORAMENTO EM TEMPO REAL (POR CORE)")
            color: Theme.highlightColor
            font.pixelSize: 12
            font.bold: true 
            leftPadding: 5
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.dialogSectionBg
            radius: 8
            border.color: Theme.borderColor
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
                        color: Theme.borderColor
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
                                text: qsTr("C%1").arg(modelData.id)
                                color: Theme.textColor
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
                                    color: Theme.coreLoadTrackColor
                                    radius: 4
                                    
                                    Rectangle {
                                        width: parent.width * (modelData.load / 100.0)
                                        height: parent.height
                                        radius: 4
                                        color: modelData.load > 85 ? Theme.criticalColor : Theme.highlightColor
                                        Behavior on width { NumberAnimation { duration: 400 } }
                                    }
                                }
                                
                                RowLayout {
                                    Text { 
                                        text: modelData.load.toFixed(1) + "%"
                                        color: Theme.textColor
                                        font.pixelSize: 10
                                    }
                                    Item { Layout.fillWidth: true }
                                    Text { 
                                        text: (modelData.freq / 1000.0).toFixed(2) + " GHz"
                                        color: Theme.mutedTextColor
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
}
