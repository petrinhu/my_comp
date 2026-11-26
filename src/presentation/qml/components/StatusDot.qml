import QtQuick
import ".."

// Indicador circular ativo/inativo (serviços, bluetooth, containers).
Rectangle {
    property bool active: false
    property int size: 8
    width: size
    height: size
    radius: size / 2
    color: active ? Theme.successColor : Theme.inactiveColor
}
