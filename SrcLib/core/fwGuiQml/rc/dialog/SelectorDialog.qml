import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Window 2.3

Window {
    id: window
    modality: Qt.ApplicationModal
    Dialog {
        id: dialog
        objectName: "dialog"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        Material.theme: Material.System
        Material.accent: Material.LightBlue

        ColumnLayout {
            id: column

            Layout.fillWidth: true

            // GroupBox to set permit only one RadioButton selected
            GroupBox {
                id: groupBox
                title: selectorDialog.message
                Layout.fillWidth: true
                property int initSize: 0
                ButtonGroup {
                    id: buttonGroup
                }
                background: Rectangle {
                    y: groupBox.topPadding - groupBox.bottomPadding
                    width: parent.width
                    height: parent.height - groupBox.topPadding + groupBox.bottomPadding
                    color: "transparent"
                    border.color: Material.accent
                    radius: 2
                }
                label: Label {
                    text: groupBox.title
                    color: Material.primary
                }

                Column {
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
                onHeightChanged: window.height = window.height + height
                onWidthChanged: {
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
    Component.onCompleted: show()
}
