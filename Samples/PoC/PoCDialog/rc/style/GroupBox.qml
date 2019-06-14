import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3

GroupBox {
    id: control
    title: qsTr("GroupBox")

    background: Rectangle {
        y: control.topPadding - control.bottomPadding
        width: parent.width
        height: parent.height - control.topPadding + control.bottomPadding
        color: "transparent"
        border.color: Material.accent
        radius: 2
    }
    label: Label {
        x: control.leftPadding
        width: control.availableWidth
        text: groupBox.title
        color: Material.primary
    }
}
