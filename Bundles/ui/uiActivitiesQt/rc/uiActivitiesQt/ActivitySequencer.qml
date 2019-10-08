import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12

Item{

    id: activitySequencer
    objectName: "sequencer"
    height: 50

    property int currentSelection: 0

    signal activitySelected(int index)

    function enableActivity(index){
        repeater.itemAt(index).enabled = true
    }

    function setCurrentActivity(index){
        currentSelection = index
    }

    Component.onCompleted: {
        Material.theme = (theme === "light") ? Material.Light : Material.Dark
        Material.accent = (accent != "") ? accent : Material.color(Material.Teal, Material.Shade900)
        Material.foreground =  (foreground != "") ? foreground : Material.color(Material.BlueGrey, Material.Shade900)
        Material.background = (background != "") ? background : Material.background
        Material.primary = (primary != "") ? primary : Material.color(Material.Teal)
        Material.elevation = (elevation != "") ? elevation : Material.elevation
    }

    RowLayout {
        id: sequencerLayout
        anchors.margins: 0
        spacing: 0
        anchors.fill: parent

        Repeater {
            id: repeater
            model: activityNameList

            RowLayout{
                width: (widgetWidth / activityNameList.length)
                enabled: false

                Rectangle{
                    Layout.fillWidth: true
                    color: Material.primary
                    height: 3
                    visible: index > 0
                }
                Button{
                    id: toolButton
                    text : modelData

                    onClicked: {
                        activitySelected(index)
                        activitySequencer.currentSelection = index
                    }
                    background: Rectangle {
                        color: {
                            var color = Material.background
                            if (activitySequencer.currentSelection === index)
                            {
                                color = Material.accent
                            }
                            toolButton.hovered? Qt.lighter(color) : color
                        }
                        radius: 10
                    }

                    contentItem: RowLayout {
                        id: buttonLayout

                        Rectangle {
                            id: activityIndex
                            color: Material.primary
                            radius: 15
                            width: 30
                            height: 30

                            Text {
                                anchors.fill: parent
                                text: index + 1
                                font: toolButton.font
                                opacity: enabled ? 1.0 : 0.3
                                color: Material.background
                                elide: Text.ElideRight
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        Text {
                            Layout.fillWidth: true
                            text: toolButton.text
                            font: Qt.font({pointSize: 12})
                            opacity: enabled ? 1.0 : 0.3
                            color: Material.primary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    color: Material.primary
                    height: 3
                    visible: index < activityNameList.length -1
                }
            }
        }
    }
}
