import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import styleQml 1.0
import uiActivitiesQml 1.0
import ExActivitiesQml 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    modality: Qt.NonModal

    Material.theme: Material.Light

    title: qsTr("ExActivitiesQml 0.1")

    onClosing: {
        activityLauncher.clear()
    }

    // Set the global theme inside the singleton "Theme" and then set the Window theme via the Singleton
    // This singleton will permit to set the same theme in all window open for this application
    Component.onCompleted: {
        Theme.theme = Material.theme
        Theme.accent = Material.color(Material.Teal, Material.Shade900)
        Theme.foreground =  Material.foreground
        Theme.background = Material.background
        Theme.primary = Material.color(Material.Teal)
        Theme.elevation = Material.elevation

        Material.accent = Theme.accent
        Material.primary = Theme.primary
    }

    // Display the activities: display a stepper and the selected activity in the main container
    ActivityLauncher {
        id: activityLauncher
        anchors.fill: parent
        // Activity ids
        activityIdsList: ["ExImageReadingActivity", "ExMesherActivity", "ExImageDisplayingActivity"]
        // Name of the activities displayed in the stepper
        activityNameList: ["Read", "Mesher", "Display"]
    }
}
