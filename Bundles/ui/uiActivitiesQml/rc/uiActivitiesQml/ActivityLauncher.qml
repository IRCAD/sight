import QtQuick 2.0
import QtQuick.Controls 2.12
import uiActivitiesQml 1.0
import QtQuick.Layouts 1.3

import guiQml 1.0

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

        ActivitySequencer {
            Layout.fillWidth: true
            activityIdsList: activityLauncher.activityIdsList
            activityNameList: activityLauncher.activityNameList

            onServiceCreated: {
                appManager.onServiceCreated(srv)
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
