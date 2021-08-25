import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import sight.medData 1.0
import sight.style 1.0

Item {
    id: modelList

    signal serviceCreated(var srv)

    enabled: false

    Component.onCompleted: {
        serviceCreated(modelSeriesList)
    }

    SModelSeriesList {
        id: modelSeriesList

        listModel: organListModel

        onStarted: {
            modelList.enabled = true
        }
    }

    Component {
        id: highlight
        Rectangle {
            width: listView.width; height: 40
            color: "lightsteelblue"; radius: 5
            y: listView.currentItem ? listView.currentItem.y: 0
            Behavior on y {
                SpringAnimation {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumHeight: 30
            color: Theme.background
            z: 2

            RowLayout {
                anchors.fill: parent

                CheckBox {
                    id: hideAll
                    text: "Hide all"
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    contentItem: Text {
                        text: hideAll.text
                        font: hideAll.font
                        opacity: enabled ? 1.0 : 0.3
                        color: Theme.primary
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: hideAll.indicator.width + hideAll.spacing
                    }

                    onCheckedChanged: {
                        modelSeriesList.onShowReconstructions(checked)
                        checkAll.enabled = !checked
                        uncheckAll.enabled = !checked
                    }
                }

                Button {
                    id: checkAll
                    text: "Check all"
                    Layout.fillHeight: true
                    Layout.preferredWidth: 120
                    onClicked: {
                        modelSeriesList.onCheckAllBoxes(true)
                    }
                }

                Button {
                    id: uncheckAll
                    text: "Uncheck all"
                    Layout.fillHeight: true
                    Layout.preferredWidth: 120
                    onClicked: {
                        modelSeriesList.onCheckAllBoxes(false)
                    }
                }
            }
        }

        // this list represent the log of all errors
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: OrganListModel {
                id: organListModel

                onModelReset :{
                    listView.currentIndex = -1
                    modelSeriesList.onOrganSelected(-1)
                }
            }
            snapMode: ListView.SnapToItem
            headerPositioning: ListView.PullBackHeader
            highlight: highlight

            // the header represent the title of each row
            header: Row {
                z: 2

                ItemDelegate {
                    id: visibilityTitle

                    text: qsTr("Visibility")

                    font.bold: true
                    implicitWidth: 60
                    implicitHeight: 40
                }

                ItemDelegate {
                    id: nameTitle

                    text: qsTr("Name")

                    font.bold: true
                    implicitWidth: listView.width - 180
                    implicitHeight: 40
                }

                ItemDelegate {
                    id: messageTitle

                    text: qsTr("Type")

                    font.bold: true
                    implicitWidth: 120
                    implicitHeight: 40
                }
            }

            // the delegate is the style of all item from the model
            delegate: Row {
                CheckDelegate {
                    id: organVisibilityRow
                    checked: organVisibility
                    implicitWidth: 60
                    implicitHeight: 40
                    onCheckStateChanged: modelSeriesList.onOrganVisibilityChanged(index, checked)
                }

                ItemDelegate {
                    id: organNameRow
                    text: organName
                    implicitWidth: listView.width - 180
                    implicitHeight: 40
                    onClicked: {
                        modelSeriesList.onOrganSelected(index)
                        listView.currentIndex = index
                    }
                }

                ItemDelegate {
                    id: structureTypeRow
                    text: structureType
                    implicitWidth: 120
                    implicitHeight: 40
                    onClicked: {
                        modelSeriesList.onOrganSelected(index)
                        listView.currentIndex = index
                    }
                }
            }
            onCurrentIndexChanged: modelSeriesList.onOrganSelected(currentIndex)
            onCurrentItemChanged: modelSeriesList.onOrganSelected(currentIndex)
        }
    }
}
