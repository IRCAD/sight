 pragma Singleton
import QtQuick 2.9
import QtQuick.Controls.Material 2.3

QtObject {
    id: objectTheme
    objectName: "objectTheme"
    property var accent: Material.accent
    property var theme: Material.theme
    property var foreground: Material.foreground
    property var background: Material.background
    property var primary: Material.primary
    property var elevation: Material.elevation
}
