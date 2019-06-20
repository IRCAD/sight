import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    PocDialogProgressDialogs {
        id: pocDialogProgressDialogs
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
                Component.onCompleted: pocDialogProgressDialogs.title = windowTitleField.text
                onTextChanged: pocDialogProgressDialogs.message = windowTitleField.text
            }
        }
        CheckBox {
            text: "Add Callback"
            onCheckedChanged: pocDialogProgressDialogs.addCallback = checked
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
                anchors.verticalCenter: parent.verticalCenter
                onClicked: pocDialogProgressDialogs.open()
            }
            Label {
                id: percent
                text: pocDialogProgressDialogs.result
            }
            Slider {
                id: windowPercentField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                value: 0.01

            }
            TextField {
                id: windowMessageProgressField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Message"
                onTextChanged: pocDialogProgressDialogs.message = windowMessageProgressField.text
                Component.onCompleted: pocDialogProgressDialogs.message = windowMessageProgressField.text
            }
            Button {
                text: "Add"
                anchors.verticalCenter: parent.verticalCenter
                enabled: pocDialogProgressDialogs.isOpen
                onClicked: pocDialogProgressDialogs.addPercent(windowPercentField.value, windowMessageProgressField.text)
            }
        }
    }
}
