import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Material 2.0
import uiReconstructionQml 1.0

Item {
    id: organMaterialEditor

    signal serviceCreated(var srv)

    enabled: false

    Component.onCompleted: {
        serviceCreated(organMaterialService)
    }

    SOrganMaterialEditor {
        id: organMaterialService

        // @disable-check M16
        onStarted: {
            organMaterialEditor.enabled = true
        }
        // @disable-check M16
        onStopped: {
            organMaterialEditor.enabled = false
        }

        // @disable-check M16
        onMaterialChanged: {
            colorDialog.setColor(color)
            transparencySlider.value = opacity
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            color: "transparent"

            Button {
                id: colorButton
                anchors.fill: parent
                text: "Color"
                onClicked: {
                    colorDialog.open()
                }
                style: ButtonStyle {
                        background: Rectangle {
                            width: 10
                            height: 10
                            border.color: colorDialog.color
                            border.width: 5
                            radius: 8

                        }
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
        }

        RowLayout {

            Layout.preferredHeight: 30
            Layout.fillWidth: true

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 100
                color: "transparent"

                Text {
                    height: parent.height
                    anchors.fill: parent
                    text: "Opacity: "
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"

                Slider {
                    id: transparencySlider
                    anchors.fill: parent
                    anchors.rightMargin: 4
                    minimumValue: 0
                    maximumValue: 100
                    stepSize: 1
                    onValueChanged: {
                        organMaterialService.onOpacitySlider(value)
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 60
                color: "transparent"

                Text {
                    id: transparencyLabel
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter

                    text: transparencySlider.value  + " %"
                }
            }
        }
    }
}
