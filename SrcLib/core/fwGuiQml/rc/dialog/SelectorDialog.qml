import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
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

            // GroupBox to set permit only one RadioButton selected
            GroupBox {
                id: groupBox
                title: selectorDialog.message
                property var initSize: 0
                Layout.minimumWidth: 150

                ButtonGroup {
                    id: buttonGroup
                }

                Column {
                    id: columnBox
                    spacing: 10
                    Layout.fillWidth: true

                    // Repeater to create all the needed options
                    Repeater {
                        id: checkboxList

                        model: selectorModel
                        RadioButton {
                            text: textOption
                            checked: check
                            ButtonGroup.group: buttonGroup
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
