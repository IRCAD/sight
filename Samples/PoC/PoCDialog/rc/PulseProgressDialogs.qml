import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import PoCDialog 1.0
import "style" as Sight

Item {
    clip: true

    PocDialogPulseProgressDialogs {
        id: pocDialogPulseProgressDialogs
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
                onTextChanged: pocDialogPulseProgressDialogs.title = windowTitleField.text
                Component.onCompleted: pocDialogPulseProgressDialogs.title = windowTitleField.text
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
                onTextChanged: pocDialogPulseProgressDialogs.message = windowMessageField.text
                Component.onCompleted: pocDialogPulseProgressDialogs.message = windowMessageField.text
            }
        }
        RowLayout {
            id: buttonRow
            Sight.Button {
                text: "Open"
                onClicked: pocDialogPulseProgressDialogs.open()
            }
        }
    }
}
