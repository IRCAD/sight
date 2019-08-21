import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12

import guiQml 1.0

Window {
    id: window

    height: 400
    minimumWidth: 600
    modality: Qt.ApplicationModal
    // flags to erase the close button
    flags: Qt.MSWindowsFixedSizeDialogHint

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

            property var initSize: 0
            property var checkLength: 0

            ButtonGroup {
                id: buttonGroup
            }

            Flickable {
                anchors.fill: parent
                contentHeight: columnRepeater.implicitHeight
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
                                if (width > groupBox.checkLength && width > window.width && width > groupBox.width)
                                {
                                    groupBox.checkLength = width
                                    if (window.width == 0)
                                    {
                                        groupBox.initSize = width
                                        groupBox.checkLength = width / 2
                                    }
                                    window.width = groupBox.checkLength + groupBox.initSize + dialog.leftMargin + dialog.leftPadding
                                }
                            }
                        }
                    }
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
