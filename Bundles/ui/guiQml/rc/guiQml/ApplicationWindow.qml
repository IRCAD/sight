import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.3

ApplicationWindow {
    id: window

    color: Material.backgroundColor

    overlay.modal: Rectangle {
        color: window.Material.backgroundDimColor
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    overlay.modeless: Rectangle {
        color: window.Material.backgroundDimColor
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }
}
