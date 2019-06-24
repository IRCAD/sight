import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    // c++ class
    PocDialogSelectorDialogs {
        id: pocDialogSelectorDialogs
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
                onTextChanged: pocDialogSelectorDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogSelectorDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogSelectorDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogSelectorDialogs.message = windowMessageField.text
            }
        }
        // row to add an option of the list of the multi selector dialog
        RowLayout {
            id: optionRow
            property var options: []
            Label {
                id: customizeFilter
                text: "Option"
                Layout.alignment: Qt.AlignBaseline
                font.italic: true
            }

            TextField {
                id: windowOptionField
                placeholderText: "name of the option"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
            }
            Button {
                text: "Add"
                onClicked: {
                    optionRow.options.push(windowOptionField.text)
                    windowOptionField.text = ""
                    pocDialogSelectorDialogs.hasOption = true;
                    pocDialogSelectorDialogs.options = optionRow.options
                }
            }
        }
        Label {
            text: "option selected: " + pocDialogSelectorDialogs.result
            font.bold: true
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                enabled: pocDialogSelectorDialogs.hasOption
                onClicked: {
                    pocDialogSelectorDialogs.open()
                    optionRow.options = []
                }
            }
        }
    }
}
