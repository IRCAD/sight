import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12
import uiReconstructionQml 1.0
import styleQml 1.0

Item {
    id: organMaterialEditor

    enabled: false

    signal serviceCreated(var srv)

    Component.onCompleted: {
        serviceCreated(organMaterialService)
    }

    SOrganMaterialEditor {
        id: organMaterialService

        // @disable-check M16
        onStarted: {
            organMaterialEditor.enabled = true
        }
        onStopped: {
            organMaterialEditor.enabled = false
        }

        onMaterialChanged: {
            colorDialog.setColor(color)
            transparencySlider.value = opacity
        }
    }

    ColumnLayout {
        anchors.right: parent.right
        anchors.fill: parent
        Layout.fillWidth: true
        spacing: 2

        Button {
            id: colorButton
            Layout.preferredHeight: 60
            Layout.fillWidth: true
            text: "Color"
            onClicked: {
                colorDialog.open()
            }
            contentItem: Text {
                text: colorButton.text
                font: colorButton.font
                opacity: enabled ? 1.0 : 0.3
                color: Theme.foreground
                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: Qt.AlignHCenter
            }

            background: Rectangle {
                border.color: colorDialog.color
                border.width: 5
                radius: 8
                color: "transparent"
            }
        }

        ColorDialog {
            id: colorDialog
            title: "Please choose a color"
            color: "#ffffff"
            onAccepted: {
                organMaterialService.onColor(color)
            }
        }

        RowLayout {
            Layout.preferredHeight: 30

            Text {
                Layout.preferredHeight: 30
                text: "Opacity: "
                verticalAlignment: Text.AlignVCenter
            }

            Slider {
                id: transparencySlider
                Layout.fillHeight: true
                Layout.fillWidth: true
                anchors.rightMargin: 4
                from: 0
                to: 100
                stepSize: 1
                onValueChanged: {
                    organMaterialService.onOpacitySlider(value)
                }
            }

            Text {
                id: transparencyLabel
                Layout.fillHeight: true
                Layout.preferredWidth: 60
                verticalAlignment: Text.AlignVCenter

                text: transparencySlider.value  + " %"
            }
        }
    }
}
