import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import PoCDialog 1.0

Item {
    width: 320
    height: 360
    transformOrigin: Item.Bottom
    clip: true

    PocDialogLoggerDialogs {
        id: pocDialogLoggerDialogs
    }

    Column {
        id: optionsColumn
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
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
                onTextChanged: pocDialogLoggerDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogLoggerDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: loggerRow
            property var errorLevel : []
            property var errorMessage : []
            GroupBox {
                id: groupBox
                title: "Level of error"
                ExclusiveGroup {
                    id: radioBoxGroup
                }

                Column {
                    spacing: 10
                    Layout.fillWidth: true
                    RadioButton {
                        text: "Information"
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        text: "Warning"
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        text: "Critical"
                        checked: false
                        exclusiveGroup: radioBoxGroup
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
                    if (radioBoxGroup.current)
                    {
                        var message = windowMessageErrorField.text;
                        loggerRow.errorMessage.push(message);
                        loggerRow.errorLevel.push(radioBoxGroup.current.text)
                        windowMessageErrorField.text = ""
                        radioBoxGroup.current = null
                        pocDialogLoggerDialogs.hasError = true
                        pocDialogLoggerDialogs.errorMessage = loggerRow.errorMessage
                        pocDialogLoggerDialogs.errorLevel = loggerRow.errorLevel
                    }
                }
            }
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: buttonRow.height * 1.2
        color: Qt.darker(palette.window, 1.1)
        border.color: Qt.darker(palette.window, 1.3)
        Row {
            id: buttonRow
            spacing: 6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
            width: parent.width
            Button {
                text: "Open"
                enabled: pocDialogLoggerDialogs.hasError
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    pocDialogLoggerDialogs.open();
                    loggerRow.errorLevel = [];
                    loggerRow.errorMessage = [];
                }
            }
        }
    }
}
