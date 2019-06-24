import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    // c++ class
    PocDialogLoggerDialogs {
        id: pocDialogLoggerDialogs
    }

    ColumnLayout {
        id: optionsColumn
        anchors.fill: parent
        anchors.margins: 12
        spacing: 0
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
                onTextChanged: pocDialogLoggerDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogLoggerDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogLoggerDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogLoggerDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: loggerRow
            property var errorLevel : []
            property var errorMessage : []
            ButtonGroup {
                id: buttonGroup
            }
            // add one type of error in the log with its associated message when we click add
            GroupBox {
                id: groupBox
                title: "Level of error"
                Column {
                    spacing: 10
                    Layout.fillWidth: true
                    RadioButton {
                        text: "Information"
                        checked: false
                        ButtonGroup.group: buttonGroup
                    }
                    RadioButton {
                        text: "Warning"
                        checked: false
                        ButtonGroup.group: buttonGroup
                    }
                    RadioButton {
                        text: "Critical"
                        checked: false
                        ButtonGroup.group: buttonGroup
                    }
                }
            }
            TextField {
                id: windowMessageErrorField
                Layout.fillWidth: true
                text: "Custom Message"
            }
            Button {
                text: "Add"
                onClicked: {
                    if (buttonGroup.checkedButton)
                    {
                        var message = windowMessageErrorField.text;
                        loggerRow.errorMessage.push(message);
                        loggerRow.errorLevel.push(buttonGroup.checkedButton.text)
                        windowMessageErrorField.text = ""
                        buttonGroup.checkedButton = null
                        pocDialogLoggerDialogs.hasError = true
                        pocDialogLoggerDialogs.errorMessage = loggerRow.errorMessage
                        pocDialogLoggerDialogs.errorLevel = loggerRow.errorLevel
                    }
                }
            }
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                enabled: pocDialogLoggerDialogs.hasError
                onClicked: {
                    pocDialogLoggerDialogs.open();
                    loggerRow.errorLevel = [];
                    loggerRow.errorMessage = [];
                }
            }
        }
    }
}
