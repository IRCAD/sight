import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1

Item {
    width: 320
    height: 240
    SystemPalette { id: palette }
    clip: true

    //! [colordialog]
    ColorDialog {
        id: colorDialog
        visible: colorDialogVisible.checked
        modality: colorDialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: "Choose a color"
        color: "green"
        showAlphaChannel: colorDialogAlpha.checked
        onAccepted: { console.log("Accepted: " + color) }
        onRejected: { console.log("Rejected") }
    }
    //! [colordialog]

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        Label {
            font.bold: true
            text: "Color dialog properties:"
        }
        CheckBox {
            id: colorDialogModal
            text: "Modal"
            checked: true
            Binding on checked { value: colorDialog.modality != Qt.NonModal }
        }
        CheckBox {
            id: colorDialogAlpha
            text: "Show alpha channel"
            Binding on checked { value: colorDialog.showAlphaChannel }
        }
        CheckBox {
            id: colorDialogVisible
            text: "Visible"
            Binding on checked { value: colorDialog.visible }
        }
        Row {
            id: colorRow
            spacing: parent.spacing
            height: colorLabel.implicitHeight * 2.0
            Rectangle {
                color: colorDialog.color
                height: parent.height
                width: height * 2
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onClicked: colorDialog.open()
                }
            }
            Label {
                id: colorLabel
                text: "<b>current color:</b> " + colorDialog.color
                anchors.verticalCenter: parent.verticalCenter
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
            height: implicitHeight
            width: parent.width
            Button {
                text: "Open"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: colorDialog.open()
            }
            Button {
                text: "Close"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: colorDialog.close()
            }
            Button {
                text: "set to green"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: colorDialog.color = "green"
            }
        }
    }
}
