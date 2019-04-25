import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Dialog {
    title: progressDialog.title
    width: 410
    standardButtons: StandardButton.Cancel
    Column {
        id: column
        width: 400
        Text {
            width: column.width
            text: progressDialog.message
            wrapMode: Text.WordWrap
        }
        RowLayout {
            width: 400
            ProgressBar {
                id: progressBar
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 100
                value: progressDialog.value
            }
            // text percentage at the end of the ProgressBar
            Text {
                id: percent
                text: (progressBar.value * 100).toFixed().toLocaleString() + "%"
            }
        }
    }
    onButtonClicked: progressDialog.cancelPressed()
    onVisibleChanged: progressDialog.visible = visible
}
