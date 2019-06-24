import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    // c++ class
    PocDialogPulseProgressDialogs {
        id: pocDialogPulseProgressDialogs
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        Label {
            font.bold: true
            text: "Message dialog properties:"
        }
        RowLayout {
            Label {
                id: customizeTitle
                text: "Window Title"
                Layout.alignment: Qt.AlignBaseline
                font.italic: true
            }
            TextField {
                id: windowTitleField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Dialog"
                onTextChanged: pocDialogPulseProgressDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogPulseProgressDialogs.title = windowTitleField.text
            }
        }
        RowLayout {
            Label {
                id: customizeMessage
                text: "Window Message"
                Layout.alignment: Qt.AlignBaseline
                font.italic: true
            }
            TextField {
                id: windowMessageField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Message"
                onTextChanged: pocDialogPulseProgressDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogPulseProgressDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                onClicked: pocDialogPulseProgressDialogs.open()
            }
        }
    }
}
