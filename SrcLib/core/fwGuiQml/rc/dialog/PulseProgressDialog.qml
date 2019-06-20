import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12

import guiQml 1.0

Window {
    id: window
    width: 600
    height: 200
    modality: Qt.ApplicationModal
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint

    Dialog {
        objectName: "dialog"
        standardButtons: Dialog.Cancel
        modal: true
        width: window.width
        height: window.height

        Column {
            width: 550
            height: 150
            id: root
            Label {
                id: testing
                width: root.width
                wrapMode: Text.WordWrap
                text: pulseProgressDialog.message
                font.bold: true
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
    Component.onCompleted: {
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
        window.show()
    }
}
