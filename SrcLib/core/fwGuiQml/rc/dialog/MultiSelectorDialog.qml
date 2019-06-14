import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
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
    Component.onCompleted: show()
}
