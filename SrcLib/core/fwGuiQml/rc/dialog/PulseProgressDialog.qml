import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

Dialog {
    standardButtons: StandardButton.Cancel
    width: 410
    Column {
        width: 400
        id: root
        Text {
            id: testing
            width: root.width
            text: pulseProgressDialog.message
        }
            ProgressBar {
                indeterminate: true
                id: progressBar
                width: root.width
            }
    }
    onButtonClicked: pulseProgressDialog.onCanceled()
    onVisibleChanged: visible ? "" : rejected()
}
