import Qt3D.Core 2.12
import Qt3D.Render 2.14
import Qt3D.Extras 2.12
import Qt3D.Input 2.12

import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Scene3D 2.12

import fwRenderQt3D 1.0

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Quit")
                action: Action {
                    shortcut: "Ctrl+Q"
                    onTriggered: window.close()
                }
            }
        }
    }

    Scene3D {
        id: scene

        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.UserAspectRatio

        GenericScene {
            id: root

            camera.position: Qt.vector3d(0.0, -30.0, 5.0)

            Entity {
                id: cylinderEntity

                components: [cylinderMesh, cylinderMaterial]

                PhongMaterial {
                    id: cylinderMaterial
                    ambient: "crimson"
                }

                CylinderMesh {
                    id: cylinderMesh
                    length: 10.0
                    radius: 3.0
                    rings: 20
                    slices: 20
                }
            }

            Entity {
                id: planeEntity

                components: [planeMesh, planeMaterial]

                PhongMaterial {
                    id: planeMaterial
                    ambient: "blue"
                }

                PlaneMesh {
                    id: planeMesh
                    width: 10.0
                    height: 10.0
                    meshResolution: Qt.size(2, 2)
                }
            }

            renderSettings.activeFrameGraph: RenderStateSet {
                renderStates: [
                    CullFace {
                        mode: CullFace.NoCulling
                    },
                    DepthTest {
                        depthFunction: DepthTest.Less
                    }
                ]

                ForwardRenderer {
                    clearColor: "#2d2d2d"
                    camera: root.camera
                }
            }
        }
    }
}

