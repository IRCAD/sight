import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import styleQml 1.0

Window {
    id: window
    modality: Qt.NonModal
    title: progressDialog.title
    width: 600
    height: 300

    Dialog {
        // this function set the message of the download and set the value of the progress bar
        function changeValue(msg, a)
        {
            message.text = msg
            percent.text = a.toString()
            progress.value = a
        }

        id: dialog
        width: window.width
        height: window.height
        objectName: "dialog"
        standardButtons: progressDialog.hasCallback ? Dialog.Cancel : Dialog.NoButton
        modal: false

        ColumnLayout {
            anchors.fill: parent
            Label {
                id: message
                wrapMode: Text.WordWrap
                width: 550
                font.bold: true
            }
            RowLayout {
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
            }
        }

        onRejected: {
            progressDialog.cancelPressed()
            window.close()
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
    Component.onDestruction: {
        progressDialog.cancelPressed()
    }
}
