import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import ExActivitiesQml 1.0

import guiQml 1.0

Item {
    id: exImageReading

    Rectangle {
        color: "blue"
        anchors.fill: parent
    }

    ImageReadingManager {
        id: appManager
    }

    Component.onCompleted: {
        appManager.initialize()
    }

    Component.onDestruction: {
        appManager.uninitialize()
    }
}
