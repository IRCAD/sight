import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import tuto08 1.0
import fwVTKQml 1.0
import uiImageQml 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true

    onClosing: appManager.uninitialize();

    Component.onCompleted: appManager.initialize()

    AppManager {
        id: appManager
        // @disable-check M16
        frameBuffer: scene3D

        // @disable-check M16
        onImageLoaded: {
            sliceEditor.enabled=true
            sliceSelector.enabled=true
            displayScanButton.enabled = true
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open image...")
                shortcut: "Ctrl+I"
                onTriggered: appManager.onOpenImage()
            }
            Action {
                text: qsTr("&Open meshes...")
                shortcut: "Ctrl+M"
                onTriggered: appManager.onOpenModel()
            }
            Action {
                text: qsTr("&Open texture...")
                shortcut: "Ctrl+T"
                onTriggered: appManager.onOpenTexture()
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Quit")
                shortcut: "Ctrl+Q"
                onTriggered: root.close()
            }
        }
    }

    ColumnLayout {
        spacing: 2
        anchors.fill: parent
        Rectangle {
            id: sceneRec
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#006699"

            FrameBufferItem {
                id: scene3D
                anchors.fill: parent

                // @disable-check M16
                onReady: appManager.createVtkScene()
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.maximumHeight: 50
            spacing: 4

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 150

                color: "transparent"
                ComboBox {
                    id: sliceEditor
                    anchors.fill: parent
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 4
                    visible: true
                    enabled: false

                    model: [qsTr("One slice"), qsTr("Three slice")]

                    currentIndex: 1
                    onActivated: {
                        appManager.onUpdateSliceMode((index == 0) ? 1 : 3)
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 75

                color: "transparent"
                Button {
                    id: displayScanButton
                    visible: true
                    enabled: false
                    checkable: true
                    checked: true
                    anchors.fill: parent
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Scan")
                    onCheckedChanged: {
                        sliceEditor.enabled = checked
                        appManager.onShowScan(checked)
                    }
                }
            }

            SliceSelector {
                id: sliceSelector
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: true
                enabled: false
                sliceOrientation: 2 // axial

                onServiceCreated: {
                    appManager.onServiceCreated(srv)
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 70
                color: "transparent"

                Button {
                    id: snapButton
                    text: qsTr("Snap")
                    anchors.fill: parent
                    anchors.rightMargin: 4
                    anchors.left: parent.left
                    onClicked: {
                        snapFileDialog.open()
                    }
                }
            }
        }
    }

    FileDialog {
        id: snapFileDialog
        title: qsTr("Save snapshot as")
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
            // File dialog return an url like file://myFile.png, but saveToFile required a path without 'file://'
            var urlNoProtocol = (fileUrl+"").replace('file://','');
            sceneRec.grabToImage(function(result) {
                var isSaved = result.saveToFile(urlNoProtocol)
                if (!isSaved) {
                    console.error("An error occuers while saving '" + urlNoProtocol + "'.")
                }
            });
        }
    }
}

