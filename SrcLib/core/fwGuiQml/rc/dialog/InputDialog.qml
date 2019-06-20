import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import guiQml 1.0

Window {
    id: window
    width: 500
    height: 300
    modality: Qt.ApplicationModal
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint

    Dialog {
        objectName: "dialog"
        parent: ApplicationWindow.overlay
        modal: true
        width: 500
        height: 300
        standardButtons: Dialog.Cancel | Dialog.Ok

        ColumnLayout {
            id: column
            anchors.fill: parent
            Label {
                text: inputDialog.message
                font.bold: true
                Layout.columnSpan: 2
                Layout.fillWidth: true
                width: parent.width
                wrapMode: Text.WordWrap
            }
            TextField {
                id: answer
                Layout.fillWidth: true
                placeholderText: inputDialog.input
            }
        }

        onAccepted: {
            inputDialog.resultDialog(answer.text, true)
            window.close()
        }
        onRejected: {
            inputDialog.resultDialog(answer.text, false)
            window.close()
        }
        onVisibleChanged: visible ? "" : reset()
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
