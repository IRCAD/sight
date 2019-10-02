import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import fwVTKQml 1.0
import uiImageQml 1.0
import uiActivitiesQml 1.0
import ExActivitiesQml 1.0
import styleQml 1.0

Activity {
    id: exImageDisplaying

    // App manager associated to this activity
    appManager: MesherManager {
        id: appManager
        frameBuffer: scene3D
    }

    ColumnLayout {

        anchors.fill: parent

        ToolBar {
            RowLayout {
                Text {
                    text: "Reduction (%): "
                    color: Theme.primary
                }

                Slider {
                    id: slider
                    from: 0
                    to: 100
                    stepSize: 1
                }

                Text {
                    text: "%1 %".arg(slider.value)
                    color: Theme.primary
                }

                ToolButton{
                    text: qsTr("Generate Mesh")
                    onClicked: appManager.applyMesher(slider.value)
                }
            }
        }

        Rectangle {
            id: sceneRec
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#006699"

            // Frame buffer to display the 3D scene
            FrameBufferItem {
                id: scene3D
                anchors.fill: parent

                onReady: appManager.createVtkScene()
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.maximumHeight: 50

            ComboBox {
                id: sliceEditor
                Layout.fillHeight: true

                model: ["One slice", "Three slice"]

                currentIndex: 1
                onActivated: {
                    appManager.onUpdateSliceMode((index == 0) ? 1 : 3)
                }
            }

            Button {
                id: displayScanButton
                checkable: true
                checked: true
                Layout.fillHeight: true

                text: "Scan"
                onCheckedChanged: {
                    sliceEditor.enabled = checked
                    appManager.onShowScan(checked)
                }
            }

            SliceSelector {
                id: sliceSelector
                Layout.fillHeight: true
                Layout.fillWidth: true

                onServiceCreated: {
                    // register the service in the activity manager
                    exImageDisplaying.registerService(srv)
                }
            }
        }
    }

    Component.onCompleted: {
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
    }
}
