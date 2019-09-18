import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import styleQml 1.0

Window{
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
        standardButtons: Dialog.Cancel | Dialog.Ok

        GroupBox {
            id: groupBoxOption
            anchors.fill: parent
            height: window.height
            width: window.width
            title: multiSelectorDialog.message

            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: columnRepeater.implicitHeight
                contentWidth: columnRepeater.implicitWidth
                clip: true

                Column {
                    id: columnRepeater
                    anchors.fill: parent

                    Repeater {
                        id: checkBoxList

                        model: multiSelectorModel
                        delegate: CheckBox {
                            text: textOption
                            checked: check
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
            // set an array with all checkbox status
            var listClick = [];
            var length = checkBoxList.count
            for (var i = 0; i < length; i++)
            {
                var status = checkBoxList.itemAt(i).checked
                listClick.push(status);
            }
            multiSelectorDialog.resultDialog(listClick, true);
            window.close()
        }
        onRejected: {
            multiSelectorDialog.resultDialog([], false);
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
