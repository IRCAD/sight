import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
    RowLayout {
        anchors.fill: parent
        Text {
            id: message
            wrapMode: Text.WordWrap
        }
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
        Button {
            text: "Cancel"
            enabled: progressDialog.hasCallback
            onClicked: progressDialog.cancelPressed()
        }
    function changeValue(msg, a)
    {
        message.text = msg
        percent.text = a.toString()
        progress.value = a
    }
    }
