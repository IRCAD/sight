import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Item {
    id: root
    width: 580
    height: 400
    SystemPalette { id: palette }
    clip: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        Label {
            font.bold: true
            text: "Message dialog properties:"
        }
        RowLayout {
            Text {
                id: customizeTitle
                text: "Window Title"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowTitleField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Dialog"
            }
        }
        RowLayout {
            Text {
                id: customizeMessage
                text: "Window Message"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowMessageField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Message"
            }
        }
        RowLayout {
            Text {
                id: customizeInput
                text: "Window Input Placeholder"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowInputField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Input"
            }
        }
    }
}
