import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import PoCDialog 1.0
import "style" as Sight

Item {
    clip: true

    PocDialogMultiSelectorDialogs {
        id: pocDialogMultiSelectorDialogs
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
                onTextChanged: pocDialogMultiSelectorDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogMultiSelectorDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogMultiSelectorDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogMultiSelectorDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: optionRow
            property var options: []
            property var checkedOpt: []
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
            CheckBox {
                id: windowCheckField
                text: "Is it checked ?"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignBaseline
            }
            Sight.Button {
                text: "Add"
                onClicked: {
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
        Sight.Label {
            text: "<b>options selected:</b> " + pocDialogMultiSelectorDialogs.result
        }
        RowLayout {
            id: buttonRow
            Sight.Button {
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
