import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Controls.Material 2.3

RowLayout {
    anchors.fill: parent
    Text {
        id: message
        wrapMode: Text.WordWrap
        color: Material.primary
    }
    ProgressBar {
        id: progress
        Layout.fillWidth: true
        to: 100
    }
    Text {
        Layout.fillWidth: true
        id: percent
        color: Material.primary
    }
    Text {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignRight
        text: "%"
        color: Material.primary
    }
    Button {
        text: "Cancel"
        highlighted: true
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
