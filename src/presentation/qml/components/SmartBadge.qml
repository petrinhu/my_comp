import QtQuick
import QtQuick.Controls
import ".."

// Selo de saúde SMART de um disco (Saudável/Atenção/Risco/N/A) com tooltip de detalhes.
Rectangle {
    id: smartBadge
    // Objeto smart do drive (storageViewModel.drives[i].smart).
    property var smart
    readonly property string severity: {
        if (!smart || !smart.available) return "unavail";
        if (!smart.passed || smart.wearPct >= 90 || smart.tempC >= 70) return "risk";
        if (smart.wearPct >= 70 || smart.tempC >= 60) return "warn";
        return "ok";
    }
    readonly property string label: {
        switch (severity) {
            case "ok":      return qsTr("✓ Saudável");
            case "warn":    return qsTr("⚠ Atenção");
            case "risk":    return qsTr("✗ Risco");
            default:        return qsTr("SMART N/A");
        }
    }
    readonly property color fg: {
        switch (severity) {
            case "ok":      return Theme.successColor;
            case "warn":    return Theme.warningColor;
            case "risk":    return Theme.criticalColor;
            default:        return Theme.textColor;
        }
    }
    implicitWidth: badgeText.implicitWidth + 14
    implicitHeight: badgeText.implicitHeight + 6
    radius: 4
    color: Qt.rgba(fg.r, fg.g, fg.b, 0.15)
    border.color: fg
    border.width: 1
    Text {
        id: badgeText
        anchors.centerIn: parent
        text: smartBadge.label
        color: smartBadge.fg
        font.pixelSize: 10; font.bold: true
    }
    ToolTip.visible: smartMouse.containsMouse
    ToolTip.text: {
        if (!smartBadge.smart || !smartBadge.smart.available) {
            return smartBadge.smart && smartBadge.smart.errorMessage
                ? qsTr("SMART indisponível: %1").arg(smartBadge.smart.errorMessage)
                : qsTr("SMART indisponível (smartctl ausente ou sem permissão)");
        }
        return qsTr("Temp: %1°C · Power-On: %2h · Desgaste: %3%")
             .arg(smartBadge.smart.tempC)
             .arg(smartBadge.smart.powerOnHours)
             .arg(smartBadge.smart.wearPct);
    }
    MouseArea { id: smartMouse; anchors.fill: parent; hoverEnabled: true }
}
