import QtQuick 2.0
import QtQuick.Controls 2.12
import uiActivitiesQml 1.0
import QtQuick.Layouts 1.3

import styleQml 1.0

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

        RowLayout {
            ActivitySequencer {
                Layout.fillWidth: true
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

        StackView {
            id: activityStackView
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Component.onCompleted: {
        appManager.initialize()
        appManager.onServiceCreated(activityView)
    }
}
