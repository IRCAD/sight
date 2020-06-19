import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12

import styleQml 1.0

Window {
    id: window

    height: 400
    minimumWidth: 600
    modality: Qt.ApplicationModal

    Dialog {
        objectName: "dialog"
        id: dialog
        height: parent.height
        width: parent.width
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        // GroupBox to set permit only one RadioButton selected
        GroupBox {
            id: groupBox
            anchors.fill: parent
            title: selectorDialog.message
            height: window.height
            width: window.width

            property var initSize: 0
            property var checkLength: 0

            ButtonGroup {
                id: buttonGroup
            }

            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: columnRepeater.implicitHeight
                contentWidth: columnRepeater.implicitWidth
                clip: true

                Column {
                    id: columnRepeater
                    anchors.fill: parent

                    // Repeater to create all the needed options
                    Repeater {
                        id: checkboxList

                        model: selectorModel
                        delegate: RadioButton {
                            text: textOption
                            checked: check
                            ButtonGroup.group: buttonGroup

                            Component.onCompleted: {
                                if (width > flickable.contentWidth)
                                    flickable.contentWidth = width
                            }
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar {
                    parent: flickable.parent
                    anchors.top: flickable.top
                    anchors.right: flickable.right
                    anchors.bottom: flickable.bottom
                    active: true
                }
                ScrollBar.horizontal: ScrollBar {
                    id: horizontalScroll
                    parent: flickable.parent
                    anchors.left: flickable.left
                    anchors.right: flickable.right
                    anchors.bottom: flickable.bottom
                    active: true
                }
            }
        }

        onAccepted: {
            var selected = "";
            var length = checkboxList.count;
            for (var i = 0; i < length; i++)
            {
                if (checkboxList.itemAt(i).checked)
                {
                    selected = checkboxList.itemAt(i).text;
                    break;
                }
            }
            // send only the selected RadioButton option
            selectorDialog.resultDialog(selected);
            window.close()
        }
        onRejected: {
            selectorDialog.resultDialog("");
            window.close()
        }
        onVisibleChanged: visible ? "" : reset()
    }

    //Set Material Theme from Singleton
    Component.onCompleted: {
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
        window.show()
    }
}
