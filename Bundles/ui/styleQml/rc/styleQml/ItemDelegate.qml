import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ItemDelegate {
    id: control

    contentItem: Label {
        rightPadding: control.spacing
        font: control.font
        text: control.text
        color: control.enabled ? ((control.font.bold || control.font.italic) ? control.Material.primary : control.Material.accent) : control.Material.hintTextColor
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
    background: Rectangle {
        implicitHeight: control.Material.delegateHeight
        opacity: 0.5
        color: control.down ? Material.color(Material.Grey, Material.Shade400) : Material.color(Material.Grey, Material.Shade200)
    }
    Rectangle {
        width: control.width
        height: 1
        color: (control.font.bold || control.font.italic) ? control.Material.primary : control.Material.accent
        anchors.bottom: control.bottom
    }
}
