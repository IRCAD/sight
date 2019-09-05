import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import guiQml 1.0

// this RowLayout need to go inside the footer of the ApplicationWindow
RowLayout {
    // this function set the message of the download and set the value of the progress bar
    function changeValue(msg, a)
    {
        message.text = msg
        percent.text = a.toString()
        progress.value = a
    }

    anchors.fill: parent

    Label {
        id: message
        wrapMode: Text.WordWrap
        font.bold: true
    }
    ProgressBar {
        id: progress
        Layout.fillWidth: true
        to: 100
    }
    // the number of the progress value
    Label {
        Layout.fillWidth: true
        id: percent
    }
    Label {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignRight
        text: "%"
    }
    // the button to cancel the operation if callback exist
    Button {
        text: "Cancel"
        enabled: progressDialog.hasCallback
        onClicked: progressDialog.cancelPressed()
    }
}
