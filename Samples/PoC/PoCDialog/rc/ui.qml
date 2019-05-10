import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import PoCDialog 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    modality: Qt.NonModal

    title: qsTr("PoCDialog 0.1")
    onClosing: appManager.uninitialize();

    Component.onCompleted: appManager.initialize()

    AppManager {
        id: appManager
    }
    TabView {
        anchors.fill: parent
        anchors.margins: 8
        Tab {
            id: controlPage
            title: "Input"
            InputDialogs { }
        }
        Tab {
            title: "Location"
            LocationDialogs { }
        }
        Tab {
            title: "Logger"
            LoggerDialogs { anchors.fill: parent }
        }
        Tab {
            title: "Message"
            MessageDialogs { anchors.fill:parent }
        }
        Tab {
            title: "MultiSelector"
            MultiSelectorDialogs { anchors.fill:parent }
        }
        Tab {
            title: "Progress"
            ProgressDialogs { anchors.fill:parent }
        }
        Tab {
            title: "PulseProgress"
            PulseProgressDialogs { anchors.fill:parent }
        }
        Tab {
            title: "Selector"
            SelectorDialogs { anchors.fill:parent }
        }
    }

}
