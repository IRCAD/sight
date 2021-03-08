import QtQuick 2.12
import QtQuick.Controls 2.12

Label {
    id: control

    color: (font.bold || font.italic) ? control.Material.primary : control.Material.foreground
}
