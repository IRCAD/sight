import QtQuick 2.0
import QtQuick.Controls 2.12
import sight.activities 1.0
import QtQuick.Layouts 1.3

import sight.style 1.0

Item {
    height: 60
    anchors.margins: 0

    // ids of the activities to launch. They will by launched in the same order
    property var activityIdsList: []
    // name of the activities to launch. Displayed in the sequencer
    property var activityNameList: []

    // Signal emitted when the service is created, it must be listen in the upper Qml object
    signal serviceCreated(var srv)

    Component.onCompleted: {
        serviceCreated(activitySequencer)
    }

    // C++ service: allow to select and enable the activities
    SActivitySequencer {
        id: activitySequencer
        activityIds: activityIdsList

        property int currentSelection

        onEnable: {
            repeater.itemAt(index).enabled = true
        }
        onSelect: {
            currentSelection = index
        }
    }
    RowLayout {
        id: stepperLayout
        height: 60
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // display the stepper buttons
        Repeater {
            id: repeater
            model: activityNameList.length == 0 ? activityIdsList : activityNameList

            RowLayout{
                width: (stepperLayout.width / activityIdsList.length)
                enabled: false

                Rectangle{
                    Layout.fillWidth: true
                    color: Theme.primary
                    height: 3
                    visible: index > 0
                }
                Button{
                    id: toolButton
                    text : modelData

                    onClicked: {
                        activitySequencer.goTo(index)
                        activitySequencer.currentSelection = index
                    }
                    background: Rectangle {
                        color: {
                            var color = Theme.background
                            if (activitySequencer.currentSelection == index)
                            {
                                color = Theme.accent
                            }
                            toolButton.hovered? Qt.lighter(color) : color
                        }
                        radius: 10
                    }

                    contentItem: RowLayout {
                        id: buttonLayout

                        Rectangle {
                            id: activityIndex
                            color: Theme.primary
                            radius: 15
                            width: 30
                            height: 30

                            Text {
                                anchors.fill: parent
                                text: index + 1
                                font: toolButton.font
                                opacity: enabled ? 1.0 : 0.3
                                color: Theme.background
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
                            color: Theme.primary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true
                    color: Theme.primary
                    height: 3
                    visible: index < activityIdsList.length -1
                }
            }
        }
    }
}
