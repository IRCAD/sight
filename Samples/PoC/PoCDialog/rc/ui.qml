import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import PoCDialog 1.0
import "style"

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    modality: Qt.NonModal
    Material.accent: Material.LightBlue
    Material.theme: Material.Light

    title: qsTr("PoCDialog 0.1")

    header:         TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Input")
        }
        TabButton {
            text: qsTr("Location")
        }
        TabButton {
            text: qsTr("Logger")
        }
        TabButton {
            text: qsTr("Message")
        }
        TabButton {
            text: qsTr("MultiSelector")
        }
        TabButton {
            text: qsTr("Progress")
        }
        TabButton {
            text: qsTr("PulseProgress")
        }
        TabButton {
            text: qsTr("Selector")
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: bar.currentIndex
        InputDialogs { }
        LocationDialogs { }
        LoggerDialogs { }
        MessageDialogs { }
        MultiSelectorDialogs { }
        ProgressDialogs { }
        PulseProgressDialogs { }
        SelectorDialogs { }
    }
    footer: ToolBar {
        background:  Rectangle {
            implicitHeight: 40
            color: Material.background

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color: "transparent"
                border.color: Material.accent
            }
        }
        objectName: "toolBar"
    }
}
