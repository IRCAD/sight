import QtQuick 2.0

Item {
    width: 8
    anchors.margins: 6
    Rectangle {
        width: 1
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#22000000"
    }
    Rectangle {
        width: 1
        height: parent.height
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#33ffffff"
    }
}
