import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import guiQml 1.0
import uiActivitiesQml 1.0
import ExActivitiesQml 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    modality: Qt.NonModal

    title: qsTr("ExActivitiesQml 0.1")

    ActivityLauncher {
        id: activityLauncher

        activityIdsList: ["ExImageReading", "ExImageDisplaying"]
        activityNameList: ["Read", "Display"]

    }


    // Set the global theme inside the singleton "Theme" and then set the Window theme via the Singleton
    // This singleton will permit to set the same theme in all window open for this application
    Component.onCompleted: {
        Theme.accent = Material.color(Material.LightBlue)
        Theme.theme = Material.Light
        Theme.foreground = (Material.theme == Material.Dark) ? Material.color(Material.BlueGrey, Material.Shade100) :
                                                               Material.color(Material.BlueGrey, Material.Shade900)
        Theme.background = Material.background
        Theme.primary = Material.color(Material.Teal)
        Theme.elevation = Material.elevation
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
        appManager.initialize()
    }
    onClosing: {
        activityLauncher.clear()
    }
}
