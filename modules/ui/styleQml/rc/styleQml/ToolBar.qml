import QtQuick 2.12
import QtQuick.Controls 2.12

ToolBar {

    background: Rectangle {
        implicitHeight: 40
        color: Theme.background
    }
    Rectangle {
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: "transparent"
        border.color: Theme.primary
    }
}
