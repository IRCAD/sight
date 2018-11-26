import QtQuick 2.0
import QtQuick.Layouts 1.0

Rectangle {
    property alias content: childrenContainer.children
    property string name: ""

    Text {
        id: caption
        text: parent.name
        font.pointSize: 8
    }
    Rectangle {
        id: childrenContainer

        color: '#DDD3D3D3'
        y: caption.height
        x: 5
        width: parent.width - 10
        height: parent.height - caption.height
        border.width: 1
        border.color: "#C2C2C2"
    }
}
