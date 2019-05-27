import QtQuick 2.9
import QtQuick.Dialogs 1.2

MessageDialog {

    title: messageDialog.title
    text: messageDialog.message
    icon: messageDialog.icon
    standardButtons: messageDialog.buttons

    onButtonClicked: {
        messageDialog.resultDialog(clickedButton)
    }
    onVisibleChanged: visible ? "" : reset()
}
