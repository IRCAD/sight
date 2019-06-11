import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Dialog {
    id: dialog
    width: 600
    title: progressDialog.title
    standardButtons: progressDialog.hasCallback ? Dialog.Cancel : Dialog.NoButton
    modality: Qt.NonModal
    ColumnLayout {
        anchors.fill: parent
        Text {
            id: message
            wrapMode: Text.WordWrap
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
    onRejected: progressDialog.cancelPressed()
}
