import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import styleQml 1.0
import Tuto02DialogQml 1.0

Item {
    clip: true

    // c++ class
    DialoggSelectorDialogs {
        id: Tuto02DialogQmlSelectorDialogs
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
                onTextChanged: Tuto02DialogQmlSelectorDialogs.title = windowTitleField.text
                Component.onCompleted: Tuto02DialogQmlSelectorDialogs.title = windowTitleField.placeholderText
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
                onTextChanged: Tuto02DialogQmlSelectorDialogs.message = windowMessageField.text
                Component.onCompleted: Tuto02DialogQmlSelectorDialogs.message = windowMessageField.placeholderText
            }
        }
        // row to add an option of the list of the multi selector dialog
        RowLayout {
            id: optionRow
            property var options: []
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
            Button {
                text: "Add"
                onClicked: {
                    var option = windowOptionField.text ? windowOptionField.text : windowOptionField.placeholderText
                    optionRow.options.push(option)
                    windowOptionField.text = ""
                    Tuto02DialogQmlSelectorDialogs.hasOption = true;
                    Tuto02DialogQmlSelectorDialogs.options = optionRow.options
                }
            }
        }
        Label {
            text: "option selected: " + Tuto02DialogQmlSelectorDialogs.result
            font.bold: true
        }
        RowLayout {
            id: buttonRow
            Button {
                text: "Open"
                enabled: Tuto02DialogQmlSelectorDialogs.hasOption
                onClicked: {
                    Tuto02DialogQmlSelectorDialogs.open()
                    optionRow.options = []
                }
            }
        }
    }
}
