import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import sight.style 1.0
import sight.InputDialog 1.0

Window {
    id: window
    width: 500
    height: 400
    modality: Qt.ApplicationModal

    Dialog {
        objectName: "dialog"
        parent: ApplicationWindow.overlay
        modal: true
        width: window.width
        height: window.height
        standardButtons: Dialog.Cancel | Dialog.Ok
        title: inputDialog.title

        ColumnLayout {
            id: column
            anchors.fill: parent

            //Label to show programm message
            Label {
                text: inputDialog.message
                font.bold: true
                Layout.columnSpan: 2
                Layout.fillWidth: true
                width: parent.width
                wrapMode: Text.WordWrap
            }
            //TextField for the response of the user
            TextField {
                id: answer
                Layout.fillWidth: true
                placeholderText: inputDialog.input
                echoMode: inputDialog.echoMode == EchoMode.NOECHO ? TextInput.NoEcho : inputDialog.echoMode == EchoMode.PASSWORD ? TextInput.Password : inputDialog.echoMode == EchoMode.ECHO_ON_EDIT ? TextInput.PasswordEchoOnEdit : TextInput.Normal
            }
        }

        onAccepted: {
            inputDialog.resultDialog(answer.text, true)
            window.close()
        }
        onRejected: {
            inputDialog.resultDialog(answer.text, false)
            window.close()
        }
        onVisibleChanged: visible ? "" : reset()
    }

    //Set Material Theme from Singleton
    Component.onCompleted: {
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
        window.show()
    }
}
