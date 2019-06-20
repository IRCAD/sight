import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import guiQml 1.0

RowLayout {
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
    Label {
        Layout.fillWidth: true
        id: percent
    }
    Label {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignRight
        text: "%"
    }
    Button {
        text: "Cancel"
        enabled: progressDialog.hasCallback
        onClicked: progressDialog.cancelPressed()
    }
}
