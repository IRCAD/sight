import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Window 2.3

Window {
    id: window
    width: 600
    height: 200
    modality: Qt.ApplicationModal

    Dialog {
        objectName: "dialog"
        standardButtons: Dialog.Cancel
        modal: true
        width: window.width
        height: window.height
        Material.theme: Material.System
        Material.accent: Material.LightBlue
        Column {
            width: 550
            height: 150
            id: root
            Text {
                id: testing
                width: root.width
                wrapMode: Text.WordWrap
                text: pulseProgressDialog.message
            }
            ProgressBar {
                indeterminate: true
                id: progressBar
                width: root.width
            }
        }
        onRejected: {
            pulseProgressDialog.onCanceled()
            window.close()
        }
        onVisibleChanged: visible ? "" : rejected()
        Component.onCompleted: {
            window.width = width
            window.height = height
        }
    }
    Component.onCompleted: show()
}
