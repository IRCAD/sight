import QtQuick 2.0
import uiActivitiesQml 1.0
import QtQuick.Layouts 1.3
import guiQml 1.0

Item {
    id: activityLauncher
    anchors.fill: parent

    // clear the activities and uninitialize the AppManager
    function clear(){
        activityStackView.clear()
        appManager.uninitialize()
    }

    // ids of the activities to launch. They will by launched in the same order
    property var activityList: []

    property ToolBar toolbar: ToolBar {
        RowLayout {
            ToolButton{
                text : "<"
                onClicked: {
                    appManager.previous()
                }
            }
            ToolButton{
                text : ">"
                onClicked: {
                    appManager.next()
                }
            }
        }
    }

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
        }
    }

    StackView {
        id: activityStackView
        anchors.fill: parent
    }

    Component.onCompleted: {
        appManager.setActivities(activityList)
        appManager.initialize()
        appManager.onServiceCreated(activityView)
    }
}
