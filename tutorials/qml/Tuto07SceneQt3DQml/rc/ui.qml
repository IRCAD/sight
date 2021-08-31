import Qt3D.Core 2.12
import Qt3D.Render 2.14
import Qt3D.Extras 2.12
import Qt3D.Input 2.12

import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Scene3D 2.12

import sight.viz.qt3d 1.0 as Sight
import tutosceneqt3d 1.0

ApplicationWindow {
    id: window
    width: 1080
    height: 720
    visible: true

    onClosing: appManager.uninitialize();

    Component.onCompleted: appManager.initialize()

    // Needs an AppManager to handle services such as mesh reader.
    AppManager {
        id: appManager

        mesh: _mesh
        scene: _root
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open mesh")
                action: Action {
                    shortcut: "Ctrl+O"
                    onTriggered: appManager.onOpenModel()
                }
            }
            MenuSeparator { }
            MenuItem {
                text: qsTr("&Quit")
                action: Action {
                    shortcut: "Ctrl+Q"
                    onTriggered: window.close()
                }
            }
        }
    }

    // Needs a Scene3D node to use qt3d rendering features in an ApplicationWindow.
    Scene3D {
        id: scene

        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

        Sight.GenericScene {
            id: _root

            camera.position: Qt.vector3d(0.0, 0.0, 5.0)

            Sight.Mesh{
                id: _mesh

                material: Sight.Material {
                    id: _material
                }
            }
        }
    }
}

