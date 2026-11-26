import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Página rolável padrão das abas: ScrollView com um ColumnLayout na largura disponível.
// Os filhos declarados numa ScrollPage vão para o ColumnLayout interno.
ScrollView {
    id: page
    default property alias content: column.data
    // Espaçamento entre os itens da página ("spacing" é FINAL em Control).
    property alias contentSpacing: column.spacing

    contentWidth: availableWidth
    clip: true
    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

    // Atribuição explícita: o default property deste arquivo é o alias acima.
    contentData: [
        ColumnLayout {
            id: column
            width: parent.width
            anchors.margins: 25
            spacing: 15
        }
    ]
}
