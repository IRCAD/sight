import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import guiQml 1.0
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
            anchors.fill: parent

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
                    anchors.verticalCenter: parent.verticalCenter
                    Layout.fillHeight: true
                    anchors.leftMargin: 4

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
                    anchors.verticalCenter: parent.verticalCenter
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

            Layout.fillHeight: true
            Layout.minimumWidth: 300

            ColumnLayout {
                spacing: 2
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                anchors.bottomMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent

                Rectangle {
                    Layout.preferredHeight: 300
                    Layout.preferredWidth: 300
                    color: "transparent"

                    ModelList {
                        id: modelList
                        anchors.rightMargin: 5
                        anchors.leftMargin: 5
                        anchors.fill: parent

                        onServiceCreated: {
                            appManager.onServiceCreated(srv)
                        }
                    }
                }

                Rectangle {
                    Layout.preferredHeight: 60
                    Layout.preferredWidth: 300

                    color: "transparent"

                    OrganMaterialEditor {
                        id: organMaterialEditor
                        anchors.rightMargin: 5
                        anchors.leftMargin: 5
                        anchors.fill: parent

                        onServiceCreated: {
                            appManager.onServiceCreated(srv)
                        }
                    }
                }

                Rectangle {
                    Layout.preferredHeight: 400
                    Layout.preferredWidth: 300

                    color: "transparent"

                    RepresentationEditor {
                        id: representationEditor
                        anchors.rightMargin: 5
                        anchors.leftMargin: 5
                        anchors.fill: parent

                        onServiceCreated: {
                            appManager.onServiceCreated(srv)
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        appManager.replaceInputs(replaceMap)
        appManager.initialize()
    }
}
