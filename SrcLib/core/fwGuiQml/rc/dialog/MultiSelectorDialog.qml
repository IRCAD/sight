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
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint

    Dialog {
        id: dialog
        objectName: "dialog"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            id: column

            Layout.fillWidth: true

            // to set the checkboxs together
            GroupBox {
                id: groupBox
                title: multiSelectorDialog.message
                Layout.fillWidth: true
                property int initSize: 0

                Column {
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
                onHeightChanged: window.height = window.height + height
                onWidthChanged: {
                    if (window.width == 0)
                    {
                        initSize = width
                    }
                    // to get responsive size
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
