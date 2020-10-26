import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import styleQml 1.0
import Tuto02DialogQml 1.0

Item {
    clip: true

    // c++ class
    DialoggPulseProgressDialogs {
        id: Tuto02DialogQmlPulseProgressDialogs
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
                onTextChanged: Tuto02DialogQmlPulseProgressDialogs.title = windowTitleField.text
                Component.onCompleted: Tuto02DialogQmlPulseProgressDialogs.title = windowTitleField.placeholderText
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
                onTextChanged: Tuto02DialogQmlPulseProgressDialogs.message = windowMessageField.text
                Component.onCompleted: Tuto02DialogQmlPulseProgressDialogs.message = windowMessageField.placeholderText
            }
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                onClicked: Tuto02DialogQmlPulseProgressDialogs.open()
            }
        }
    }
}
