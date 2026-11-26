import QtQuick
import QtTest
import "../../src/presentation/qml/components"

// QA-05: IconButton expõe nome acessível e responde a Enter/Return, além de Espaço.
TestCase {
    id: testCase
    name: "IconButton"
    when: windowShown
    width: 200; height: 100

    Component {
        id: buttonComponent
        IconButton { text: "🔄"; label: "Atualizar" }
    }

    SignalSpy { id: clickSpy; signalName: "clicked" }

    function init() { clickSpy.clear() }

    function test_accessible_name() {
        const button = createTemporaryObject(buttonComponent, testCase)
        compare(button.Accessible.name, "Atualizar")
    }

    function test_return_and_enter_trigger_click() {
        const button = createTemporaryObject(buttonComponent, testCase)
        clickSpy.target = button
        button.forceActiveFocus()
        keyClick(Qt.Key_Return)
        keyClick(Qt.Key_Enter)
        compare(clickSpy.count, 2)
    }

    function test_disabled_ignores_return() {
        const button = createTemporaryObject(buttonComponent, testCase)
        button.enabled = false
        clickSpy.target = button
        button.forceActiveFocus()
        keyClick(Qt.Key_Return)
        compare(clickSpy.count, 0)
    }
}
