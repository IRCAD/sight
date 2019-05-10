import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import PoCDialog 1.0

Item {
    id: root
    width: 580
    height: 400
    SystemPalette { id: palette }
    clip: true

    //! [messagedialog]
    PocDialogMessageDialogs {
        id: pocDialogMessageDialogs
    }

    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: bottomBar.top
            leftMargin: 12
        }
        ColumnLayout {
            spacing: 8
            Item { Layout.preferredHeight: 4 } // padding
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
                    onTextChanged: pocDialogMessageDialogs.title = windowTitleField.text
                }
            }
            GroupBox {
                id: groupBox
                title: "Icon"
                ExclusiveGroup {
                    id: radioBoxGroup
                    onCurrentChanged: pocDialogMessageDialogs.icon = current.icon
                }
                Column {
                    spacing: 10
                    Layout.fillWidth: true
                    RadioButton {
                        text: "None"
                        property var icon: 0
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        text: "Critical"
                        property var icon: 3
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        text: "Warning"

                        property var icon: 2
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        text: "Info"
                        property var icon: 1
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }
                    RadioButton {
                        property var icon: 4
                        text: "Question"
                        checked: false
                        exclusiveGroup: radioBoxGroup
                    }

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
                    onTextChanged: pocDialogMessageDialogs.message = windowMessageField.text
                }
            }
            Label { text: "Buttons:" }
            Flow {
                spacing: 8
                Layout.fillWidth: true
                Layout.preferredWidth: root.width - 30
                property bool updating: false
                function updateButtons(button, checked) {
                    if (updating) return
                    updating = true
                    var buttons = 0
                    for (var i = 0; i < children.length; ++i)
                        if (children[i].checked)
                            buttons |= children[i].button
                    pocDialogMessageDialogs.buttons = buttons
                    updating = false
                }

                CheckBox {
                    text: "Cancel"
                    property int button: StandardButton.Cancel
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "No"
                    property int button: StandardButton.No
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "Yes"
                    property int button: StandardButton.Yes
                    onCheckedChanged: parent.updateButtons(button, checked)
                }

                CheckBox {
                    text: "OK"
                    property int button: StandardButton.Ok
                    onCheckedChanged: parent.updateButtons(button, checked)
                }
            }
            Label {
                id: result
                text: "The result is: " + pocDialogMessageDialogs.result
            }
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
            width: parent.width
            Button {
                text: "Open"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: pocDialogMessageDialogs.open()
            }
        }
    }
}
