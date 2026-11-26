import QtQuick
import QtTest
import "../../src/presentation/qml/components"

// MC-10: o nome do processo (cmdline, controlado por qualquer programa local) aparece
// como texto puro no diálogo de confirmação; marcação não pode mudar o que se confirma.
TestCase {
    id: testCase
    name: "KillProcessDialog"
    when: windowShown
    width: 640; height: 480

    Component {
        id: dialogComponent
        KillProcessDialog {}
    }

    function findByObjectName(item, nome) {
        if (item.objectName === nome)
            return item
        const filhos = item.children || []
        for (let i = 0; i < filhos.length; ++i) {
            const achado = findByObjectName(filhos[i], nome)
            if (achado)
                return achado
        }
        if (item.contentItem && item.contentItem !== item) {
            const achado = findByObjectName(item.contentItem, nome)
            if (achado)
                return achado
        }
        return null
    }

    function test_marcacao_no_nome_fica_literal() {
        const dialog = createTemporaryObject(dialogComponent, testCase)
        verify(dialog)
        dialog.openWith(4242, "<b>firefox</b><img src='file:///etc/hostname'>")
        const alvo = findByObjectName(dialog.contentItem, "killTargetName")
        verify(alvo, "Text do alvo não encontrado")
        compare(alvo.textFormat, Text.PlainText)
        verify(alvo.text.indexOf("<b>firefox</b>") !== -1, "a marcação deveria aparecer como texto")
        verify(alvo.text.indexOf("4242") !== -1)
        dialog.close()
    }
}
