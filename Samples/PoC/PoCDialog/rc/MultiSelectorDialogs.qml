import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    // c++ class
    PocDialogMultiSelectorDialogs {
        id: pocDialogMultiSelectorDialogs
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
                placeholderText: "Custom Dialog"
                onTextChanged: pocDialogMultiSelectorDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogMultiSelectorDialogs.title = windowTitleField.placeholderText
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
                placeholderText: "Custom Message"
                onTextChanged: pocDialogMultiSelectorDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogMultiSelectorDialogs.message = windowMessageField.placeholderText
            }
        }
        // row to add an option of the list of the multi selector dialog
        RowLayout {
            id: optionRow
            property var options: []
            property var checkedOpt: []
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
            CheckBox {
                id: windowCheckField
                text: "Is it checked ?"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
            }
            Button {
                text: "Add"
                onClicked: {
                    var text = windowOptionField.text ? windowOptionField.text : windowOptionField.placeholderText
                    optionRow.options.push(windowOptionField.text);
                    optionRow.checkedOpt.push(windowCheckField.checked);
                    windowOptionField.text = "";
                    windowCheckField.checked = false;
                    pocDialogMultiSelectorDialogs.hasOption = true;
                    pocDialogMultiSelectorDialogs.options = optionRow.options;
                    pocDialogMultiSelectorDialogs.checked = optionRow.checkedOpt;
                }
            }
        }
        Label {
            text: "options selected: " + pocDialogMultiSelectorDialogs.result
            font.bold: true
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                enabled: pocDialogMultiSelectorDialogs.hasOption
                onClicked: {
                    pocDialogMultiSelectorDialogs.open();
                    pocDialogMultiSelectorDialogs.hasOption = false;
                    optionRow.checkedOpt = [];
                    optionRow.options = [];
                }
            }
        }
    }
}
