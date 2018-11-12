import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Material 2.0
import uiMedDataQml 1.0

Item {
    id: modelList

    signal serviceCreated(var srv)

    enabled: false

    Component.onCompleted: {
        serviceCreated(modelSeriesList)
    }

    SModelSeriesList {
        id: modelSeriesList

        // @disable-check M16
        listModel: organListModel

        // @disable-check M16
        onStarted: {
            modelList.enabled = true
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {

            Layout.fillWidth: true
            Layout.maximumHeight: 30

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"

                CheckBox {
                    id: hideAll
                    text: "Hide all"
                    anchors.fill: parent
                    anchors.rightMargin: 4
                    anchors.left: parent.left
                    anchors.centerIn: parent
                    onCheckedChanged: {
                        modelSeriesList.onShowReconstructions(checked)
                        checkAll.enabled = !checked
                        uncheckAll.enabled = !checked
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 100
                color: "transparent"

                Button {
                    id: checkAll
                    text: "Check all"
                    anchors.fill: parent
                    anchors.rightMargin: 4
                    anchors.left: parent.left
                    onClicked: {
                        modelSeriesList.onCheckAllBoxes(true)
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 100
                color: "transparent"

                Button {
                    id: uncheckAll
                    text: "Uncheck all"
                    anchors.fill: parent
                    anchors.rightMargin: 4
                    anchors.left: parent.left
                    onClicked: {
                        modelSeriesList.onCheckAllBoxes(false)
                    }
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "transparent"
            TableView {
                id: listView
                anchors.fill: parent
                focus: true


                TableViewColumn {
                    role: "organVisibility"
                    title: ""
                    width: 20
                    delegate: Item{
                        width: listView.width
                        height: 30
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        CheckBox {
                            checked: styleData.value
                            width: 15
                            onCheckedChanged: {
                                modelSeriesList.onOrganVisibilityChanged(styleData.row, checked)
                            }
                        }
                    }
                }

                TableViewColumn {
                    role: "organName"
                    title: "Organ name"
                    width: 120
                    delegate: Item {
                        width: listView.width
                        height: 30
                        Text { text: styleData.value }
                    }
                }
                TableViewColumn {
                    role: "structureType"
                    title: "Structure type"
                    width: 100
                    delegate: Item{
                        width: listView.width
                        height: 30
                        Text { text: styleData.value }
                    }
                }

                model: OrganListModel {
                    id: organListModel

                    onModelReset :{
                        listView.currentRow = -1
                        listView.selection.clear()
                        modelSeriesList.onOrganSelected(-1)
                    }
                }

                onCurrentRowChanged: modelSeriesList.onOrganSelected(listView.currentRow)
            }
        }
    }
}
