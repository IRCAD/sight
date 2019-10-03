import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import tuto09 1.0
import fwVTKQml 1.0
import uiImageQml 1.0
import uiMedDataQml 1.0
import uiReconstructionQml 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true

    title: qsTr("Tuto09MesherWithGenericScene 0.1")
    onClosing: appManager.uninitialize();

    Component.onCompleted: appManager.initialize()

    AppManager {
        id: appManager
        // @disable-check M16
        frameBuffer: scene3D

        // @disable-check M16
        onImageLoaded: {
            sliceEditor.visible=true
            sliceSelector.visible=true
            displayScanButton.visible = true
            saveMesh.enabled = true
            actionMesh50.enabled = true
            actionMesh80.enabled = true
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")

            Action {
                text: qsTr("&Open image")
                shortcut: "Ctrl+O"
                onTriggered: appManager.onOpenImage()
            }
            Action {
                id: saveMesh
                text: qsTr("&Save mesh")
                shortcut: "Ctrl+S"
                enabled: false
                onTriggered: appManager.onSaveMesh()
            }
            Action {
                text: qsTr("&Quit")
                shortcut: "Ctrl+Q"
                onTriggered: root.close()
            }
        }
        Menu {
            title: "Mesher"

            Action {
                id: actionMesh50
                text: qsTr("Create Mesh 50")
                enabled: false
                onTriggered: appManager.applyMesher(50)
            }
            Action {
                id: actionMesh80
                text: qsTr("Create Mesh 80")
                enabled: false
                onTriggered: appManager.applyMesher(80)
            }
        }
    }

    RowLayout {
        spacing: 0
        anchors.fill: parent

        ColumnLayout {
            spacing: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

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
                    visible: false

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
                    visible: false

                    onServiceCreated: {
                        appManager.onServiceCreated(srv)
                    }
                }

                Button {
                    id: snapButton
                    text: "Snap"
                    Layout.fillHeight: true
                    Layout.preferredWidth: 70
                    onClicked: {
                        snapFileDialog.open()
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
                Layout.fillWidth: true

                ModelList {
                    id: modelList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    onServiceCreated: {
                        // register the service in the activity manager
                        appManager.onServiceCreated(srv)
                    }
                }

                OrganMaterialEditor {
                    id: organMaterialEditor
                    Layout.preferredWidth: 400
                    Layout.preferredHeight: 120

                    onServiceCreated: {
                        // register the service in the activity manager
                        appManager.onServiceCreated(srv)
                    }
                }

                RepresentationEditor {
                    id: representationEditor
                    Layout.fillWidth: true
                    Layout.preferredHeight: 800

                    onServiceCreated: {
                        // register the service in the activity manager
                        appManager.onServiceCreated(srv)
                    }
                }
            }
        }
    }

    FileDialog {
        id: snapFileDialog
        title: "Save snapshot as"
        folder: shortcuts.home
        selectExisting: false
        nameFilters: [
            "Image file (*.jpg *.jpeg *.bmp *.png *.tiff)",
            "jpeg (*.jpg *.jpeg)",
            "bmp (*.bmp)",
            "png (*.png)",
            "tiff (*.tiff)",
            "all (*.*)"
        ]
        onAccepted: {
            // File dialog return an url like file:///myFile.png on windows and else file://myFile.png,
            // but saveToFile required a path without 'file:///'
            var urlNoProtocol = ""
            if (Qt.platform.os == "windows") {
                urlNoProtocol = (fileUrl.toString()+"").replace('file:///','')
            } else {
                urlNoProtocol = (fileUrl.toString()+"").replace('file://','')
            }

            sceneRec.grabToImage(function(result) {
                var isSaved = result.saveToFile(urlNoProtocol)
                if (!isSaved) {
                    console.error("An error occuers while saving '" + urlNoProtocol + "'.")
                }
            });
        }
    }

}

