import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import styleQml 1.0
import fwVTKQml 1.0
import uiImageQml 1.0
import uiActivitiesQml 1.0
import uiMedDataQml 1.0
import uiReconstructionQml 1.0
import ExActivitiesQml 1.0


Activity {
    id: exImageDisplaying
    appManager: ImageDisplayingManager {
        id: appManager
        frameBuffer: scene3D
    }

    RowLayout {
        spacing: 0
        anchors.fill: parent

        ColumnLayout {

            Rectangle {
                id: sceneRec
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#006699"

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
                        appManager.onServiceCreated(srv)
                    }
                }
            }
        }
        ScrollView {
            spacing: 5
            enabled: true
            transformOrigin: Item.Center
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true

                ModelList {
                    id: modelList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    onServiceCreated: {
                        appManager.onServiceCreated(srv)
                    }
                }

                OrganMaterialEditor {
                    id: organMaterialEditor
                    Layout.preferredWidth: 400
                    Layout.preferredHeight: 120

                    onServiceCreated: {
                        appManager.onServiceCreated(srv)
                    }
                }

                RepresentationEditor {
                    id: representationEditor
                    Layout.fillWidth: true
                    Layout.preferredHeight: 800

                    onServiceCreated: {
                        appManager.onServiceCreated(srv)
                    }
                }
            }
        }
    }
}
