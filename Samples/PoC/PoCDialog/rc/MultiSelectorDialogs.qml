import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import PoCDialog 1.0

Item {
    id: root
    width: 580
    height: 400
    SystemPalette { id: palette }
    clip: true

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
                onTextChanged: pocDialogMultiSelectorDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogMultiSelectorDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: optionRow
            property var options: []
            property var checkedOpt: []
            Text {
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
            Button {
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
        Label {
            width: root.width - 50
            wrapMode: Text.WordWrap
            text: "<b>options selected:</b> " + pocDialogMultiSelectorDialogs.result
        }
    }
    Rectangle {
        id: bottomBar
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
            height: implicitHeight
            width: parent.width
            Button {
                text: "Open"
                enabled: pocDialogMultiSelectorDialogs.hasOption
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    pocDialogMultiSelectorDialogs.open();
                    optionRow.checkedOpt = [];
                    optionRow.options = [];
                }
            }
        }
    }
}
