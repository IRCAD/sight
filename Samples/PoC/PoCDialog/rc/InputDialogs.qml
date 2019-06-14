import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import PoCDialog 1.0

import "style" as Sight

Item {
    clip: true

    PocDialogInputDialogs {
        id: pocDialogInputDialogs
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 0
        Sight.Label {
            font.bold: true
            text: "Message dialog properties:"
        }
        RowLayout {
            Sight.Text {
                id: customizeTitle
                text: "Window Title"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowTitleField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Dialog"
                onTextChanged: pocDialogInputDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogInputDialogs.title = windowTitleField.text
            }
        }
        RowLayout {
            Sight.Text {
                id: customizeMessage
                text: "Window Message"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowMessageField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Message"
                onTextChanged: pocDialogInputDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogInputDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            Sight.Text {
                id: customizeInput
                text: "Window Input Placeholder"
                Layout.alignment: Qt.AlignBaseline
            }
            TextField {
                id: windowInputField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Input"
                onTextChanged: pocDialogInputDialogs.input = windowInputField.text
                Component.onCompleted: pocDialogInputDialogs.input = windowInputField.text
            }
        }
        Sight.Label {
            text: "<b>result input:</b> " + pocDialogInputDialogs.result
        }

        RowLayout {
            id: buttonRow
            Sight.Button {
                text: "Open"
                onClicked: pocDialogInputDialogs.open()
            }
        }
    }
}
