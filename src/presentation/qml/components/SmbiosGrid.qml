import QtQuick
import QtQuick.Layouts
import ".."

// Grade rótulo/valor com os dados SMBIOS (hardwareViewModel.smbios).
GridLayout {
    columns: 2; columnSpacing: 12; rowSpacing: 6
    Text { text: qsTr("Fabricante:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: hardwareViewModel.smbios.vendor || "—"; color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
    Text { text: qsTr("Modelo:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: hardwareViewModel.smbios.model || "—"; color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
    Text { text: qsTr("Versão:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: hardwareViewModel.smbios.version || "—"; color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
    Text { text: qsTr("Serial:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: hardwareViewModel.smbios.serial || ((hardwareViewModel.smbios.restricted || []).indexOf("serial") >= 0 ? qsTr("🔒 requer root") : "—"); color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight; font.family: "Monospace" }
    Text { text: qsTr("SKU:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: hardwareViewModel.smbios.sku || "—"; color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
    Text { text: qsTr("Placa-mãe:"); color: Theme.mutedTextColor; font.pixelSize: 11 }
    Text { textFormat: Text.PlainText; text: (hardwareViewModel.smbios.boardVendor || "") + " " + (hardwareViewModel.smbios.boardName || ""); color: Theme.textColor; Layout.fillWidth: true; elide: Text.ElideRight }
}
