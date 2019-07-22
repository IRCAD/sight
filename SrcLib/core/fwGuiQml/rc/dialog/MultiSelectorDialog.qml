import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import guiQml 1.0

Window {
    id: window
    modality: Qt.ApplicationModal
    // flags to erase the close button
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.MSWindowsFixedSizeDialogHint

    Dialog {
        id: dialog
        objectName: "dialog"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            id: column

            // to set the checkboxs together
            GroupBox {
                id: groupBox
                title: multiSelectorDialog.message
                property var initSize: 0
                Layout.minimumWidth: 150

                Column {
                    id: columnBox
                    spacing: 10
                    Layout.fillWidth: true

                    // the Repeater will create all the checkbox using the c++ model
                    Repeater {
                        id: checkboxList

                        model: multiSelectorModel
                        CheckBox {
                            text: textOption
                            checked: check
                        }
                    }
                }
                onHeightChanged: window.height = dialog.height + height
                onWidthChanged: {
                    // to get responsive size
                    if (window.width == 0)
                    {
                        initSize = width
                    }
                    window.width = width + initSize + dialog.leftMargin + dialog.leftPadding
                }
            }
        }
        onAccepted: {
            // set an array with all checkbox status
            var listClick = [];
            var length = checkboxList.count;
            for (var i = 0; i < length; i++)
            {
                var status = checkboxList.itemAt(i).checked;
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
