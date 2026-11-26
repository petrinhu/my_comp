import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ".."

// Barra lateral: logo, alternância de tema e itens de navegação.
// A ordem dos itens corresponde aos índices do StackLayout de Main.qml.
Rectangle {
    id: sidebar
    // Índice da aba ativa (vem de Main.qml).
    property int currentIndex: 0
    // Emitido ao clicar num item; Main.qml decide a troca de aba.
    signal navigateRequested(int index)

    color: Theme.sidebarColor
    border.color: Theme.borderColor
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 8

        Text {
            text: qsTr("my_comp")
            color: Theme.highlightColor
            font.pixelSize: 26
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 20
        }

        ThemeToggle {}

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.borderColor; Layout.margins: 5 }

        Repeater {
            model: [
                {name: qsTr("Dashboard"), icon: "📊"},
                {name: qsTr("Updates"), icon: "📦"},
                {name: qsTr("Journal"), icon: "📜"},
                {name: qsTr("Services"), icon: "⚙️"},
                {name: qsTr("Network"), icon: "🌐"},
                {name: qsTr("Hardware"), icon: "💻"},
                {name: qsTr("Dev Tools"), icon: "🛠️"},
                {name: qsTr("Security"), icon: "🛡️"},
                {name: qsTr("Export"), icon: "📤"},
                {name: qsTr("Storage"), icon: "💾"},
                {name: qsTr("VRAM"), icon: "🎮"}
            ]
            delegate: ItemDelegate {
                id: navDelegate
                required property int index
                required property var modelData
                Layout.fillWidth: true
                highlighted: sidebar.currentIndex === index
                onClicked: sidebar.navigateRequested(index)
                ToolTip.visible: hovered
                ToolTip.text: modelData.name
                background: Rectangle {
                    color: navDelegate.highlighted ? Theme.highlightColor : (navDelegate.hovered ? Theme.hoverColor : "transparent")
                    opacity: navDelegate.highlighted ? Theme.navActiveOpacity : 1.0
                    radius: 6
                }
                contentItem: RowLayout {
                    spacing: 12
                    Text { text: navDelegate.modelData.icon; font.pixelSize: 18; color: navDelegate.highlighted ? Theme.highlightColor : Theme.textColor }
                    Text { text: navDelegate.modelData.name; color: navDelegate.highlighted ? Theme.highlightColor : Theme.textColor; font.bold: navDelegate.highlighted }
                }
            }
        }
        Item { Layout.fillHeight: true }
        Text { text: qsTr("v2.0 Alpha"); color: Theme.highlightColor; font.pixelSize: 10; font.bold: true; Layout.alignment: Qt.AlignHCenter }
    }
}
