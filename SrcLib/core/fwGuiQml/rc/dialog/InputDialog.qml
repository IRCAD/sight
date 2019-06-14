import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

Window {
    id: window
    width: 500
    height: 300
    Material.theme: Material.Light
    Material.accent: Material.LightBlue

    Dialog {
        objectName: "dialog"
        modal: true
        width: window.width
        height: window.height
        standardButtons: Dialog.Cancel | Dialog.Ok

        ColumnLayout {
            id: column
            anchors.fill: parent
            Label {
                text: inputDialog.message
                color: Material.accent
                Layout.columnSpan: 2
                Layout.fillWidth: true
                width: parent.width
                wrapMode: Text.WordWrap
            }
            TextField {
                id: answer
                Layout.fillWidth: true
                placeholderText: inputDialog.input
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
    Component.onCompleted: window.show()
}
