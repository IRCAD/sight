import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import guiQml 1.0

Item {
    id: activityView
    objectName: "guiQml_SActivityView"

    anchors.fill: parent
    ToolBar {
        id: guiQml_ToolBar
        objectName: "guiQml_ToolBar"
        anchors.left: parent.left
        anchors.right: parent.right
        RowLayout {
            ToolButton {
                id: guiQml_PreviousActivity
                objectName: "guiQml_PreviousActivity"
                text: qsTr("<")
            }
            ToolButton {
                id: guiQml_NextActivity
                objectName: "guiQml_NextActivity"
                text: qsTr(">")
            }
            ToolSeparator {}
        }
    }
    StackView {
        id: activityStackView
        objectName: "guiQml_StackView"
        anchors.top: guiQml_ToolBar.bottom
        anchors.bottom: activityView.bottom
        anchors.left: activityView.left
        anchors.right: activityView.right
    }
}
