import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1

Item {
    width: 320
    height: 360
    SystemPalette { id: palette }
    clip: true

    FontDialog {
        id: fontDialog
        visible: fontDialogVisible.checked
        modality: fontDialogModal.checked ? Qt.WindowModal : Qt.NonModal
        scalableFonts: fontDialogScalableFonts.checked
        nonScalableFonts: fontDialogNonScalableFonts.checked
        monospacedFonts: fontDialogMonospacedFonts.checked
        proportionalFonts: fontDialogProportionalFonts.checked
        title: "Choose a font"
        font: Qt.font({ family: "Arial", pointSize: 24, weight: Font.Normal })
        currentFont: Qt.font({ family: "Arial", pointSize: 24, weight: Font.Normal })
        onCurrentFontChanged: { console.log("CurrentFontChanged: " + currentFont) }
        onAccepted: { console.log("Accepted: " + font) }
        onRejected: { console.log("Rejected") }
    }

    Column {
        id: optionsColumn
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8
        Label {
            font.bold: true
            text: "Font dialog properties:"
        }
        CheckBox {
            id: fontDialogModal
            text: "Modal"
            checked: true
            Binding on checked { value: fontDialog.modality != Qt.NonModal }
        }
        CheckBox {
            id: fontDialogScalableFonts
            text: "Scalable fonts"
            Binding on checked { value: fontDialog.scalableFonts }
        }
        CheckBox {
            id: fontDialogNonScalableFonts
            text: "Non scalable fonts"
            Binding on checked { value: fontDialog.nonScalableFonts }
        }
        CheckBox {
            id: fontDialogMonospacedFonts
            text: "Monospaced fonts"
            Binding on checked { value: fontDialog.monospacedFonts }
        }
        CheckBox {
            id: fontDialogProportionalFonts
            text: "Proportional fonts"
            Binding on checked { value: fontDialog.proportionalFonts }
        }
        CheckBox {
            id: fontDialogVisible
            text: "Visible"
            Binding on checked { value: fontDialog.visible }
        }
        Label {
            text: "Current font:"
        }
        Label {
            id: fontLabel
            text: "<b>" + fontDialog.font.family + " - " + fontDialog.font.pointSize + "</b>"
            MouseArea {
                anchors.fill: parent
                onClicked: fontDialog.open()
            }
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: buttonRow.height * 1.2
        color: Qt.darker(palette.window, 1.1)
        border.color: Qt.darker(palette.window, 1.3)
        Row {
            id: buttonRow
            spacing: 6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
            width: parent.width
            Button {
                text: "Open"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: fontDialog.open()
            }
            Button {
                text: "Close"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: fontDialog.close()
            }
            Button {
                text: "set to default"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: fontDialog.font = Qt.font({ family: "Arial", pointSize: 24, weight: Font.Normal })
            }
        }
    }
}
