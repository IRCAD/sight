import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import PoCDialog 1.0
import "style" as Sight

Item {
    clip: true

    PocDialogSelectorDialogs {
        id: pocDialogSelectorDialogs
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
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
                onTextChanged: pocDialogSelectorDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogSelectorDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogSelectorDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogSelectorDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: optionRow
            property var options: []
            Sight.Text {
                id: customizeFilter
                text: "Option"
                Layout.alignment: Qt.AlignBaseline
            }

            TextField {
                id: windowOptionField
                placeholderText: "name of the option"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
            }
            Sight.Button {
                text: "Add"
                onClicked: {
                    optionRow.options.push(windowOptionField.text)
                    windowOptionField.text = ""
                    pocDialogSelectorDialogs.hasOption = true;
                    pocDialogSelectorDialogs.options = optionRow.options
                }
            }
        }
        Sight.Label {
            text: "<b>option selected:</b> " + pocDialogSelectorDialogs.result
        }
        RowLayout {
            id: buttonRow
            Sight.Button {
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
