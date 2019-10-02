import QtQuick 2.12
import QtQuick.Controls 2.12

TextField {
    selectByMouse: true

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.IBeamCursor
        acceptedButtons: Qt.NoButton
    }
}
