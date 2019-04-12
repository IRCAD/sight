import QtQuick 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

Dialog {
    signal resultDialog(var checked)
    modality: Qt.ApplicationModal
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    width: column.width
    title: "Overwrite?"
    onButtonClicked: {
        var listClick = []
        for (var i = 0; i < checkboxList.count; i++)
        {
            var status = checkboxList.itemAt(i).checked;
            listClick.push(status);
        }
        resultDialog(listClick);
    }
    ColumnLayout {
        id: column
        Layout.fillWidth: true
        GroupBox {
            id: groupBox
            objectName: "groupBox"
            Layout.fillWidth: true
            Column {
                spacing: 10
                Layout.fillWidth: true
                Repeater {
                    id: checkboxList
                    model: ListModel{
                        id: options
                        objectName: "options"
                        function addOption (opt, checked) {
                            options.append({textOption: opt, check: checked})
                        }
                    }

                    CheckBox {
                        text: textOption
                        checked: check
                    }
                }
            }
        }
    }
}
