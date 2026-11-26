import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ".."

// Overlay de notificações (toasts) empilhado no canto superior direito.
// Uso: ToastOverlay { id: toasts } + toasts.notify("msg", "success" | "error" | "info").
// Erros persistem até fechamento manual; success/info auto-dismiss em 4s.
Item {
    id: toastRoot
    anchors.fill: parent
    z: 1000

    // Cores da paleta central (Theme); podem ser sobrescritas por quem instancia.
    property color successColor: Theme.toastSuccessBg
    property color errorColor: Theme.toastErrorBg
    property color infoColor: Theme.toastInfoBg
    property color contrastTextColor: Theme.contrastTextColor
    readonly property int autoDismissMs: 4000

    ListModel { id: toastModel }

    function notify(message, kind) {
        var k = kind || "info"
        toastModel.append({ "message": message, "kind": k, "id": Date.now() + Math.random() })
    }

    function dismiss(index) {
        if (index >= 0 && index < toastModel.count) toastModel.remove(index)
    }

    ColumnLayout {
        id: stack
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.rightMargin: 16
        spacing: 8
        width: 380

        Repeater {
            model: toastModel
            delegate: Rectangle {
                id: toast
                Layout.fillWidth: true
                implicitHeight: row.implicitHeight + 20
                radius: 6
                color: {
                    if (model.kind === "success") return toastRoot.successColor
                    if (model.kind === "error") return toastRoot.errorColor
                    return toastRoot.infoColor
                }
                border.color: Qt.darker(color, 1.2)
                opacity: 0

                Component.onCompleted: opacity = 1

                Behavior on opacity { NumberAnimation { duration: 220 } }

                Timer {
                    id: autoClose
                    interval: toastRoot.autoDismissMs
                    running: model.kind !== "error"
                    repeat: false
                    onTriggered: toastRoot.dismiss(index)
                }

                RowLayout {
                    id: row
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Text {
                        text: {
                            if (model.kind === "success") return "✓"
                            if (model.kind === "error") return "✕"
                            return "ℹ"
                        }
                        color: toastRoot.contrastTextColor
                        font.pixelSize: 18
                        font.bold: true
                        Layout.preferredWidth: 20
                    }

                    Text {
                        text: model.message
                        color: toastRoot.contrastTextColor
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    IconButton {
                        text: "✕"
                        label: qsTr("Fechar aviso")
                        flat: true
                        implicitWidth: 28
                        implicitHeight: 28
                        onClicked: toastRoot.dismiss(index)
                        contentItem: Text {
                            text: parent.text
                            color: toastRoot.contrastTextColor
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle { color: "transparent" }
                    }
                }
            }
        }
    }
}
