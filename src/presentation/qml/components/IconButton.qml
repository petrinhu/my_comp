import QtQuick
import QtQuick.Controls
import ".."

// Botão de ícone acessível (QA-05): o emoji não diz nada a leitor de tela nem a quem
// navega por teclado. `label` vira Accessible.name e tooltip (também no foco por Tab), e
// Enter/Return acionam o clique; o Button do Qt só responde a Espaço.
Button {
    id: iconButton
    property string label

    Accessible.name: label
    ToolTip.visible: (hovered || visualFocus) && label.length > 0
    ToolTip.text: label
    ToolTip.delay: 400

    Keys.onReturnPressed: if (enabled) iconButton.clicked()
    Keys.onEnterPressed: if (enabled) iconButton.clicked()
}
