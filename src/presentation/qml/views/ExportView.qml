import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."
import "../components"

// ABA 8 - Centro de Exportação: gera relatório JSON/Markdown/HTML.
// A escolha de pasta usa o FolderDialog global (Main.qml), que grava em exportPath.
ScrollPage {
    id: exportView
    property alias exportPath: exportPathField.text
    signal chooseFolderRequested()

    contentSpacing: 20

    PageTitle { text: qsTr("Centro de Exportação") }

    Card {
        Layout.fillWidth: true; Layout.preferredHeight: 300
        ColumnLayout {
            anchors.centerIn: parent; width: parent.width * 0.8; spacing: 15

            Text { text: qsTr("Gerar Relatório Completo do Sistema"); color: Theme.highlightColor; font.bold: true }

            RowLayout {
                spacing: 10
                TextField {
                    id: fileNameField
                    placeholderText: qsTr("Nome do arquivo (ex: relatorio_hoje)")
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
                    placeholderText: qsTr("Pasta de destino...")
                    Layout.fillWidth: true
                    readOnly: true
                    text: exportViewModel.defaultExportPath
                }
                Button {
                    text: qsTr("Escolher Pasta")
                    onClicked: exportView.chooseFolderRequested()
                }
            }

            Item { Layout.preferredHeight: 20 }

            Button {
                text: qsTr("🚀 Gerar Relatório Agora")
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
                color: Theme.successColor
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
            statusMsg.color = success ? Theme.successColor : Theme.criticalColor
        }
    }
}
