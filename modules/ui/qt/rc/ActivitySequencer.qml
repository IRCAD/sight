import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12

Item{

    id: activitySequencer
    objectName: "Sequencer"
    height: 50

    property int currentSelection: 0

    signal activitySelected(int index)

    function enableActivity(index){
        repeater.itemAt(index).enabled = true
    }

    function disableActivity(index){
        repeater.itemAt(index).enabled = false
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
                    color: Material.primary
                    height: 3
                    visible: index > 0
                    width: 30
                }
                Button{
                    id: toolButton
                    text : modelData

                    onClicked: {
                        if (activitySequencer.currentSelection !== index)
                        {
                            activitySelected(index)
                        }
                    }
                    background: Rectangle {
                        implicitWidth: buttonWidth
                        implicitHeight: 50
                        color: {
                            var color = Material.background
                            border.width = 0
                            if(activitySequencer.currentSelection === index)
                            {
                                color = Material.accent
                                border.color = "white"
                                border.width = 2
                            }
                            if(toolButton.hovered)
                            {
                                color = elevation
                            }
                            toolButton.enabled? color : Material.foreground

                        }
                        radius: 10
                    }

                    contentItem: RowLayout {
                        id: buttonLayout

                        Rectangle {
                            id: activityIndex
                            opacity: enabled ? 1.0 : 0.3
                            color: Material.primary
                            radius: 15
                            width: 30
                            height: 30

                            Text {
                                anchors.fill: parent
                                text: index + 1
                                font: toolButton.font
                                color: enabled? Material.background : Material.foreground
                                elide: Text.ElideRight
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        Text {
                            Layout.fillWidth: true
                            text: toolButton.text
                            font: Qt.font({pointSize: fontSize})
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
