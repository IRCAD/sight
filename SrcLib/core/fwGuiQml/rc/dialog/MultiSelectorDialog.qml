import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import guiQml 1.0

Window{
    id: window

    height: 400
    modality: Qt.ApplicationModal
    // flags to erase the close button
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.MSWindowsFixedSizeDialogHint

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
            title: multiSelectorDialog.message
            Layout.minimumWidth: 150

            property var initSize: 0
            property var checkLength: 0

            Flickable {
                anchors.fill: parent
                contentHeight: columnRepeater.implicitHeight
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
                                if (width > groupBoxOption.checkLength)
                                {
                                    groupBoxOption.checkLength = width
                                    if (window.width == 0)
                                    {
                                        groupBoxOption.initSize = width
                                        groupBoxOption.checkLength = 0
                                    }
                                    window.width = groupBoxOption.checkLength + groupBoxOption.initSize + dialog.leftMargin + dialog.leftPadding
                                }
                            }
                        }
                    }
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
