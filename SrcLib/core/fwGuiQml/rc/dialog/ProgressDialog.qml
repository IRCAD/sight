import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Dialog {
    id: dialog
    title: progressDialog.title
    modality: Qt.NonModal
    width: 440
    standardButtons: StandardButton.Cancel

    Column {
        id: column
        width: 430

        Text {
            id: progressText
            objectName: "message"
            width: column.width
            text: progressDialog.message
            wrapMode: Text.WordWrap
        }
        RowLayout {
            width: 430
            ProgressBar {
                id: progressBar
                Layout.fillWidth: true
                indeterminate: true
            }
            // text percentage at the end of the ProgressBar
            Text {
                objectName: "progressBar"
                Layout.fillWidth: true
                id: percent
                text: "100"
            }
            Text {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                text: "%"
            }
        }
    }

    onButtonClicked: progressDialog.cancelPressed()
    onVisibleChanged: percent.text = "0"
}
