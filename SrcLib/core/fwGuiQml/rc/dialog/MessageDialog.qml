import QtQuick 2.9
import QtQuick.Dialogs 1.2

MessageDialog {

    standardButtons: messageDialog.buttons

    onButtonClicked: {
        messageDialog.resultDialog(clickedButton)
    }
    onVisibleChanged: visible ? "" : reset()
}
