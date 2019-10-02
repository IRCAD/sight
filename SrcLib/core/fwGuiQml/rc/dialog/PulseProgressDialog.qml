import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12

import styleQml 1.0

Window {
    id: window
    width: 600
    height: 200
    modality: Qt.ApplicationModal
    // the number of the progress value
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint

    Dialog {
        objectName: "dialog"
        standardButtons: Dialog.NoButton
        modal: true
        width: window.width
        height: window.height

        Column {
            anchors.fill: parent
            spacing: 15
            Label {
                id: testing
                width: window.width - 50
                wrapMode: Text.WordWrap
                text: pulseProgressDialog.message
                font.bold: true
            }
            // the inderteminate state is to setup a progress bar
            // when we don't know how much time it will take
            ProgressBar {
                indeterminate: true
                id: progressBar
                width: window.width - 50
            }
        }
        onVisibleChanged: visible ? "" : rejected()
        Component.onCompleted: {
            window.width = width
            window.height = height
        }
    }

    //Set Material Theme from Singleton
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
