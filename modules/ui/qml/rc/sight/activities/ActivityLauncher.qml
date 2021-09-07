import QtQuick 2.0
import QtQuick.Controls 2.12
import sight.activities 1.0
import QtQuick.Layouts 1.3

import sight.style 1.0

Item {
    id: activityLauncher

    // clear the activities and uninitialize the AppManager
    function clear(){
        activityStackView.clear()
        appManager.uninitialize()
    }

    // ids of the activities to launch. They will by launched in the same order
    property var activityIdsList: []
    // name of the activities to launch. Displayed in the sequencer
    property var activityNameList: []

    // C++ service manager
    ActivityLauncherManager {
        id: appManager
    }

    // launch activities in the stack view
    SActivityView {
        id: activityView

        onLaunchRequested: {
            activityStackView.clear(StackView.Immediate)
            activityStackView.push(Qt.createComponent(path), {"replaceMap": replace}, StackView.Immediate)
            notifyActivityCreation()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle{
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: Theme.background
            z: 100 // To be drawn on top of the other copmponent

            RowLayout {
                anchors.fill: parent
                // Sequencer Object that displays the activity stepper
                ActivitySequencer {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    activityIdsList: activityLauncher.activityIdsList
                    activityNameList: activityLauncher.activityNameList

                    onServiceCreated: {
                        appManager.onServiceCreated(srv)
                    }
                }
                Button{
                    id: menuButton
                    text: ""
                    icon.source: "icons/menu.svg"
                    Layout.fillHeight: true

                    onClicked: {
                        if (menu.visible){
                            menu.close()
                        } else {
                            menu.open()
                        }
                    }
                }

                Menu{
                    id: menu
                    x: menuButton.x
                    y: menuButton.y + menuButton.height
                    width: menuButton.width
                    Action {
                        text: ""
                        icon.source: "icons/open.svg"
                        onTriggered: appManager.open()
                    }
                    Action {
                        text: ""
                        icon.source: "icons/save.svg"
                        onTriggered: appManager.save()
                    }
                }
            }
        }

        // the activities component will be displayed in this view
        StackView {
            id: activityStackView
            Layout.fillHeight: true
            Layout.fillWidth: true
            z:0
        }
    }

    Component.onCompleted: {
        appManager.initialize()
        appManager.onServiceCreated(activityView)
    }
}
