import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Cartão de um disco na aba Storage: cabeçalho + SMART, partições e benchmark (hdparm).
Rectangle {
    id: driveCard
    // Entrada de storageViewModel.drives.
    property var drive: ({})

    Layout.fillWidth: true
    Layout.preferredHeight: driveColumn.implicitHeight + 30
    color: Theme.cardColor; border.color: Theme.borderColor; radius: 8

    ColumnLayout {
        id: driveColumn
        anchors.fill: parent; anchors.margins: 15; spacing: 10

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: (driveCard.drive.rotational ? "💿 " : "💾 ") + driveCard.drive.name
                color: Theme.highlightColor; font.pixelSize: 16; font.bold: true
            }
            SmartBadge { smart: driveCard.drive.smart }
            Item { Layout.fillWidth: true }
            Text {
                text: Format.formatBytes(driveCard.drive.sizeBytes)
                color: Theme.textColor; font.pixelSize: 13
            }
        }
        Text {
            visible: driveCard.drive.vendor.length > 0 || driveCard.drive.model.length > 0
            text: (driveCard.drive.vendor + " " + driveCard.drive.model).trim()
            color: Theme.mutedTextColor; font.pixelSize: 11
        }
        Text {
            visible: driveCard.drive.smart && driveCard.drive.smart.available
            text: driveCard.drive.smart
                ? qsTr("🌡 %1°C  ·  ⏱ %2h  ·  📉 desgaste %3%").arg(driveCard.drive.smart.tempC).arg(driveCard.drive.smart.powerOnHours).arg(driveCard.drive.smart.wearPct)
                : ""
            color: Theme.mutedTextColor; font.pixelSize: 10
        }

        Repeater {
            model: driveCard.drive.partitions
            delegate: PartitionUsageRow { partition: modelData }
        }

        RowLayout {
            Layout.fillWidth: true; Layout.topMargin: 8
            Button {
                text: storageViewModel.isBenchmarking ? qsTr("⏳ Executando…") : qsTr("⚡ Benchmark (hdparm)")
                enabled: !storageViewModel.isBenchmarking
                onClicked: benchmarkConfirmDialog.open()

                Dialog {
                    id: benchmarkConfirmDialog
                    modal: true
                    anchors.centerIn: parent
                    title: qsTr("Executar benchmark em /dev/%1?").arg(driveCard.drive.name)
                    standardButtons: Dialog.Ok | Dialog.Cancel
                    Text {
                        text: qsTr("hdparm -tT roda leitura apenas, mas toca o cache do sistema.\nRequer root/pkexec para acesso ao device bruto.")
                        color: Theme.textColor
                        wrapMode: Text.WordWrap
                        width: 360
                    }
                    onAccepted: storageViewModel.runBenchmark(driveCard.drive.name)
                }
            }
            Text {
                visible: storageViewModel.lastBenchmark.device === driveCard.drive.name
                      && storageViewModel.lastBenchmark.success === true
                text: storageViewModel.lastBenchmark.device === driveCard.drive.name
                    ? qsTr("Seq: %1 MB/s · Cache: %2 MB/s")
                        .arg((storageViewModel.lastBenchmark.seqReadMbS || 0).toFixed(1))
                        .arg((storageViewModel.lastBenchmark.cachedMbS || 0).toFixed(1))
                    : ""
                color: Theme.highlightColor; font.pixelSize: 11
            }
            Text {
                visible: storageViewModel.lastBenchmark.device === driveCard.drive.name
                      && storageViewModel.lastBenchmark.running !== true
                      && storageViewModel.lastBenchmark.success === false
                      && (storageViewModel.lastBenchmark.errorMessage || "").length > 0
                text: "⚠ " + (storageViewModel.lastBenchmark.errorMessage || "")
                color: Theme.criticalColor; font.pixelSize: 11; Layout.fillWidth: true; wrapMode: Text.WordWrap
            }
        }
    }
}
