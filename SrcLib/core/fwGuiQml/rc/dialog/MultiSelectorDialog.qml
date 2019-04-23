import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    modality: Qt.ApplicationModal
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    width: column.width
    title: multiSelectorDialog.title

    ColumnLayout {
        id: column

        Layout.fillWidth: true

        GroupBox {
            id: groupBox
            title: multiSelectorDialog.message
            Layout.fillWidth: true

            Column {
                spacing: 10
                Layout.fillWidth: true

                Repeater {
                    id: checkboxList

                    model: multiSelectorModel
                    CheckBox {
                        text: textOption
                        checked: check
                    }
                }
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
    }
    onRejected: {
        multiSelectorDialog.resultDialog([], false);
    }
    onVisibleChanged: multiSelectorDialog.visible = visible
}
