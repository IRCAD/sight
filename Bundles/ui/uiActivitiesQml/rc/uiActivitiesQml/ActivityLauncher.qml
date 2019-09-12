import QtQuick 2.0
import QtQuick.Controls 2.12
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

        RowLayout {
            id: stepperLayout
            Layout.fillWidth: true
            height: 60
            anchors.margins: 0
            spacing: 0
            Repeater {
                model: activityNameList.length == 0 ? activityIdsList : activityNameList
                anchors.margins: 0
                RowLayout{
                    implicitWidth: (stepperLayout.width / activityIdsList.length)
                    width: (stepperLayout.width / activityIdsList.length)
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
                            appManager.goTo(index)
                        }
                        background: Rectangle {
                            color: toolButton.hovered? Qt.lighter(Theme.background) : Theme.background
                        }

                        contentItem: RowLayout {
                            id: buttonLayout
                            anchors.margins: 0
                            Rectangle {
                                id: activityIndex
                                color: Theme.primary
                                radius: 15
                                width: 30
                                height: 30

                                Text {
                                    anchors.fill: parent
                                    text: index
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

        StackView {
            id: activityStackView
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Component.onCompleted: {
        appManager.setActivities(activityIdsList)
        appManager.initialize()
        appManager.onServiceCreated(activityView)
    }
}
