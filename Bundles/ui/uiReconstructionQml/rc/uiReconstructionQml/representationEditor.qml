import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.0
import uiReconstructionQml 1.0

Item {
    id: representationEditor
    enabled: false

    signal serviceCreated(var srv)

    Component.onCompleted: {
        serviceCreated(representationService)
    }

    SRepresentationEditor {
        id: representationService

        onStarted: {
            representationEditor.enabled = true
        }

        onStopped: {
            representationEditor.enabled = false
        }

        onMaterialChanged: {
            switch (representationMode) {
            case 1:
                surfaceButton.checked = true
                break
            case 2:
                pointButton.checked = true
                break
            case 3:
                wireframeButton.checked = true
                break;
            case 4:
                edgeButton.checked = true
                break;
            }

            switch (shadingMode) {
            case 0:
                ambientButton.checked = true
                break
            case 2:
                flatButton.checked = true
                break
            case 3:
                gouraudButton.checked = true
                break;
            case 4:
                phongButton.checked = true
                break;
            }

            switch (optionMode) {
            case 1:
                hideNormalButton.checked = true
                break
            case 2:
                pointNormalButton.checked = true
                break
            case 3:
                cellNormalButton.checked = true
                break;
            }
        }
    }

    ColumnLayout {
        anchors.right: parent.right
        anchors.fill: parent
        Layout.fillWidth: true

        GroupBox {
            Layout.fillWidth: true
            title: "Representation"
            checked: true

            ColumnLayout {
                anchors.fill: parent

                ExclusiveGroup {
                    id: representationGroup
                }
                RadioButton {
                    id: surfaceButton
                    text: "Surface"
                    Layout.fillWidth: true
                    exclusiveGroup: representationGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeRepresentation(1)
                        }
                    }
                }
                RadioButton {
                    id: pointButton
                    text: "Point"
                    Layout.fillWidth: true
                    exclusiveGroup: representationGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeRepresentation(2)
                        }
                    }
                }
                RadioButton {
                    id: wireframeButton
                    text: "Wireframe"
                    Layout.fillWidth: true
                    exclusiveGroup: representationGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeRepresentation(3)
                        }
                    }
                }

                RadioButton {
                    id: edgeButton
                    text: "Edge"
                    Layout.fillWidth: true
                    exclusiveGroup: representationGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeRepresentation(4)
                        }
                    }
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: "Shading"

            ColumnLayout {
                anchors.fill: parent

                ExclusiveGroup {
                    id: shadingGroup
                }
                RadioButton {
                    id: ambientButton
                    text: "Ambient"
                    Layout.fillWidth: true
                    exclusiveGroup: shadingGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeShading(0)
                        }
                    }
                }
                RadioButton {
                    id: flatButton
                    text: "Flat"
                    Layout.fillWidth: true
                    exclusiveGroup: shadingGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeShading(1)
                        }
                    }
                }
                RadioButton {
                    id: gouraudButton
                    text: "Gouraud"
                    Layout.fillWidth: true
                    exclusiveGroup: shadingGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeShading(2)
                        }
                    }
                }

                RadioButton {
                    id: phongButton
                    text: "Phong"
                    Layout.fillWidth: true
                    exclusiveGroup: shadingGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onChangeShading(3)
                        }
                    }
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: "Normals"

            ColumnLayout {
                anchors.fill: parent

                ExclusiveGroup {
                    id: optionGroup
                }
                RadioButton {
                    id: pointNormalButton
                    text: "Show point normals"
                    Layout.fillWidth: true
                    exclusiveGroup: optionGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onShowNormals(2)
                        }
                    }
                }
                RadioButton {
                    id: cellNormalButton
                    text: "Show cell normals"
                    Layout.fillWidth: true
                    exclusiveGroup: optionGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onShowNormals(3)
                        }
                    }
                }
                RadioButton {
                    id: hideNormalButton
                    text: "Hide normals"
                    Layout.fillWidth: true
                    exclusiveGroup: optionGroup
                    onCheckedChanged:  {
                        if (checked) {
                            representationService.onShowNormals(1)
                        }
                    }
                }
            }
        }
    }
}
