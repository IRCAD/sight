import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import styleQml 1.0
import Tuto02DialogQml 1.0

Item {
    clip: true

    // c++ class
    DialoggInputDialogs {
        id: Tuto02DialogQmlInputDialogs
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 0
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

                onTextChanged: Tuto02DialogQmlInputDialogs.title = windowTitleField.text
                Component.onCompleted: Tuto02DialogQmlInputDialogs.title = windowTitleField.placeholderText
            }
        }
        // message for the input dialog
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
                onTextChanged: Tuto02DialogQmlInputDialogs.message = windowMessageField.text
                Component.onCompleted: Tuto02DialogQmlInputDialogs.message = windowMessageField.placeholderText
            }
        }
        // the placeholder for the input dialog
        RowLayout {
            Label {
                id: customizeInput
                text: "Window Input Placeholder"
                Layout.alignment: Qt.AlignBaseline
                font.italic: true
            }
            TextField {
                id: windowInputField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                placeholderText: "Custom Input"
                onTextChanged: Tuto02DialogQmlInputDialogs.input = windowInputField.text
                Component.onCompleted: Tuto02DialogQmlInputDialogs.input = windowInputField.placeholderText
            }
        }
        Label {
            text: "result input: " + Tuto02DialogQmlInputDialogs.result
            font.bold: true
        }

        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                onClicked: Tuto02DialogQmlInputDialogs.open()
            }
        }
    }
}
