import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Item {
    id: root
    width: 580
    height: 400
    SystemPalette { id: palette }
    clip: true

    //! [dialog]
    Dialog {
        id: helloDialog
        modality: dialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: customizeTitle.checked ? windowTitleField.text : "Hello"
        onButtonClicked: console.log("clicked button " + clickedButton)
        onAccepted: lastChosen.text = "Accepted " +
            (clickedButton == StandardButton.Ok ? "(OK)" : (clickedButton == StandardButton.Retry ? "(Retry)" : "(Ignore)"))
        onRejected: lastChosen.text = "Rejected " +
            (clickedButton == StandardButton.Close ? "(Close)" : (clickedButton == StandardButton.Abort ? "(Abort)" : "(Cancel)"))
        onHelp: lastChosen.text = "Yelped for help!"
        onYes: lastChosen.text = (clickedButton == StandardButton.Yes ? "Yeessss!!" : "Yes, now and always")
        onNo: lastChosen.text = (clickedButton == StandardButton.No ? "Oh No." : "No, no, a thousand times no!")
        onApply: lastChosen.text = "Apply"
        onReset: lastChosen.text = "Reset"

        Label {
            text: "Hello world!"
        }
    }
    //! [dialog]

    Dialog {
        id: spinboxDialog
        modality: dialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: customizeTitle.checked ? windowTitleField.text : "Spinbox"
        onHelp: {
            lastChosen.text = "No help available, sorry.  Please answer the question."
            visible = true
        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Ok && answer.value == 11.0)
                lastChosen.text = "You are correct!"
            else
                lastChosen.text = "Having failed to give the correct answer, you shall not pass!"
        }

        ColumnLayout {
            id: column
            width: parent ? parent.width : 100
            Label {
                text: "<b>What</b> is the average airspeed velocity of an unladen European swallow?"
                Layout.columnSpan: 2
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                TextField {
                    id: answer
                }
                Label {
                    text: "m/s"
                    Layout.alignment: Qt.AlignBaseline | Qt.AlignLeft
                }
            }
        }
    }

    Dialog {
        id: dateDialog
        modality: dialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: customizeTitle.checked ? windowTitleField.text : "Choose a date"
        onButtonClicked: console.log("clicked button " + clickedButton)
        onAccepted: {
            if (clickedButton == StandardButton.Ok)
                lastChosen.text = "Accepted " + calendar.selectedDate.toLocaleDateString()
            else
                lastChosen.text = (clickedButton == StandardButton.Retry ? "(Retry)" : "(Ignore)")
        }
        onRejected: lastChosen.text = "Rejected"

        Calendar {
            id: calendar
            width: parent ? parent.width : implicitWidth
            onDoubleClicked: dateDialog.click(StandardButton.Ok)
        }
    }

    Dialog {
        id: filledDialog
        modality: dialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: customizeTitle.checked ? windowTitleField.text : "Customized content"
        onRejected: lastChosen.text = "Rejected"
        onAccepted: lastChosen.text = "Accepted " +
                    (clickedButton === StandardButton.Retry ? "(Retry)" : "(OK)")
        onButtonClicked: if (clickedButton === StandardButton.Retry) lastChosen.text = "Retry"
        contentItem: Rectangle {
            color: "lightskyblue"
            implicitWidth: 400
            implicitHeight: 100
            Label {
                text: "Hello blue sky!"
                color: "navy"
                anchors.centerIn: parent
            }
            Keys.onPressed: if (event.key === Qt.Key_R && (event.modifiers & Qt.ControlModifier)) filledDialog.click(StandardButton.Retry)
            Keys.onEnterPressed: filledDialog.accept()
            Keys.onEscapePressed: filledDialog.reject()
            Keys.onBackPressed: filledDialog.reject() // especially necessary on Android
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        Label {
            font.bold: true
            text: "Message dialog properties:"
        }
        CheckBox {
            id: dialogModal
            text: "Modal"
            Binding on checked { value: helloDialog.modality != Qt.NonModal }
        }
        RowLayout {
            CheckBox {
                id: customizeTitle
                text: "Window Title"
                Layout.alignment: Qt.AlignBaseline
                checked: true
            }
            TextField {
                id: windowTitleField
                Layout.alignment: Qt.AlignBaseline
                Layout.fillWidth: true
                text: "Custom Dialog"
            }
        }
        Label {
            text: "Buttons:"
        }
        Flow {
            spacing: 8
            Layout.fillWidth: true
            property bool updating: false
            function updateButtons(button, checked) {
                if (updating) return
                updating = true
                var buttons = 0
                for (var i = 0; i < children.length; ++i)
                    if (children[i].checked)
                        buttons |= children[i].button
                if (!buttons)
                    buttons = StandardButton.Ok
                helloDialog.standardButtons = buttons
                spinboxDialog.standardButtons = buttons
                dateDialog.standardButtons = buttons
                updating = false
            }

            CheckBox {
                text: "Help"
                property int button: StandardButton.Help
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Abort"
                property int button: StandardButton.Abort
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Close"
                property int button: StandardButton.Close
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Cancel"
                property int button: StandardButton.Cancel
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "NoToAll"
                property int button: StandardButton.NoToAll
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "No"
                property int button: StandardButton.No
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "YesToAll"
                property int button: StandardButton.YesToAll
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Yes"
                property int button: StandardButton.Yes
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Ignore"
                property int button: StandardButton.Ignore
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Retry"
                property int button: StandardButton.Retry
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Apply"
                property int button: StandardButton.Apply
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Reset"
                property int button: StandardButton.Reset
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "Restore Defaults"
                property int button: StandardButton.RestoreDefaults
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            CheckBox {
                text: "OK"
                checked: true
                property int button: StandardButton.Ok
                onCheckedChanged: parent.updateButtons(button, checked)
            }

            Component.onCompleted: updateButtons()
        }
        Label {
            id: lastChosen
        }
        Item { Layout.fillHeight: true }
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
                text: "Hello World"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: helloDialog.open()
            }
            Button {
                text: "Input"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: spinboxDialog.open()
            }
            Button {
                text: "Date"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: dateDialog.open()
            }
            Button {
                text: "No buttons or margins"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: filledDialog.open()
            }
        }
    }
}
