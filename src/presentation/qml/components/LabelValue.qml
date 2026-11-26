import QtQuick
import QtQuick.Layouts
import ".."

// Par rótulo/valor usado na grade de arquitetura do CpuDetailDialog.
RowLayout {
    id: labelValue
    property string label
    property string value
    spacing: 10
    Text { 
        textFormat: Text.PlainText
        text: labelValue.label
        color: Theme.mutedTextColor
        font.pixelSize: 12
        Layout.preferredWidth: 100
    }
    Text { 
        textFormat: Text.PlainText
        text: labelValue.value
        color: Theme.textColor
        font.pixelSize: 12
        font.bold: true
        Layout.fillWidth: true
        elide: Text.ElideRight
    }
}
