import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.0
import QtQuick.Window 2.3

Window {
    id: window
    modality: Qt.NonModal
    title: progressDialog.title
    width: 600
    height: 300
    Dialog {
        id: dialog
        width: window.width
        height: window.height
        objectName: "dialog"
        standardButtons: progressDialog.hasCallback ? Dialog.Cancel : Dialog.NoButton
        modal: false
        Material.theme: Material.System
        Material.accent: Material.LightBlue
        ColumnLayout {
            anchors.fill: parent
            Text {
                id: message
                wrapMode: Text.WordWrap
                width: 550
            }
            RowLayout {
                ProgressBar {
                    id: progress
                    Layout.fillWidth: true
                    to: 100
                }
                Text {
                    Layout.fillWidth: true
                    id: percent
                }
                Text {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight
                    text: "%"
                }
            }
        }
        function changeValue(msg, a)
        {
            message.text = msg
            percent.text = a.toString()
            progress.value = a
        }
        onRejected: {
            progressDialog.cancelPressed()
            window.close()
        }
    }
    Component.onCompleted: show()
}
