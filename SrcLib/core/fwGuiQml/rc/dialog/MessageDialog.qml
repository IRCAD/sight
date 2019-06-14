import QtQuick 2.9
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3

MessageDialog {

    standardButtons: messageDialog.buttons
    Material.theme: Material.System
    Material.accent: Material.LightBlue

    onButtonClicked: {
        messageDialog.resultDialog(clickedButton)
    }
    onVisibleChanged: visible ? "" : reset()
}
