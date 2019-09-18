import QtQuick 2.12
import QtQuick.Controls 2.12

GroupBox {
    id: control

    background: Rectangle {
            y: control.topPadding - control.bottomPadding
            width: parent.width
            height: parent.height - control.topPadding + control.bottomPadding
            color: "transparent"
            border.color: Material.primary
            radius: 2
        }
    label: Label {
            x: control.leftPadding
            width: control.availableWidth
            text: control.title
            color: Material.primary
            elide: Text.ElideRight
        }
}
