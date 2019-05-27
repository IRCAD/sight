import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    modality: Qt.ApplicationModal
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    width: column.width
    title: selectorDialog.title

    ColumnLayout {
        id: column

        Layout.fillWidth: true

        // GroupBox to set permit only one RadioButton selected
        GroupBox {
            id: groupBox
            title: selectorDialog.message
            Layout.fillWidth: true
            ExclusiveGroup { id: radioBoxGroup }

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
                        exclusiveGroup: radioBoxGroup
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
    }
    onRejected: {
        selectorDialog.resultDialog("");
    }
    onVisibleChanged: visible ? "" : reset()
}
