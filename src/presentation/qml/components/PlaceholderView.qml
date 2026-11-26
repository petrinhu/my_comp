import QtQuick
import QtQuick.Layouts
import ".."

// Placeholder para abas ainda não implementadas (ícone + título + descrição).
ColumnLayout {
    id: placeholder
    property string title; property string icon; property string desc
    Layout.fillWidth: true; Layout.fillHeight: true; spacing: 10
    Text { textFormat: Text.PlainText; text: placeholder.icon; font.pixelSize: 48; Layout.alignment: Qt.AlignHCenter }
    Text { textFormat: Text.PlainText; text: placeholder.title; color: Theme.textColor; font.pixelSize: 20; font.bold: true; Layout.alignment: Qt.AlignHCenter }
    Text { textFormat: Text.PlainText; text: placeholder.desc; color: Theme.mutedTextColor; Layout.alignment: Qt.AlignHCenter }
}
