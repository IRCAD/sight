import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import styleQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    // c++ class
    PocDialogInputDialogs {
        id: pocDialogInputDialogs
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 0
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
                placeholderText: "Custom Dialog"

                onTextChanged: pocDialogInputDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogInputDialogs.title = windowTitleField.placeholderText
            }
        }
        // message for the input dialog
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
                placeholderText: "Custom Message"
                onTextChanged: pocDialogInputDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogInputDialogs.message = windowMessageField.placeholderText
            }
        }
        // the placeholder for the input dialog
        RowLayout {
            Label {
                id: customizeInput
                text: "Window Input Placeholder"
                Layout.alignment: Qt.AlignBaseline
                font.italic: true
            }
            TextField {
                id: windowInputField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                placeholderText: "Custom Input"
                onTextChanged: pocDialogInputDialogs.input = windowInputField.text
                Component.onCompleted: pocDialogInputDialogs.input = windowInputField.placeholderText
            }
        }
        Label {
            text: "result input: " + pocDialogInputDialogs.result
            font.bold: true
        }

        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                onClicked: pocDialogInputDialogs.open()
            }
        }
    }
}
