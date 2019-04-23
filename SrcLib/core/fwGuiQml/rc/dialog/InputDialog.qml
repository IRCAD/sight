import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

Dialog {
    modality: Qt.ApplicationModal
    title: inputDialog.title
    standardButtons: StandardButton.Cancel | StandardButton.Ok

    ColumnLayout {
        id: column

        width: parent ? parent.width : 100
        Label {
            text: inputDialog.message
            Layout.columnSpan: 2
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        TextField {
            id: answer

            placeholderText: inputDialog.input
        }
    }

    onAccepted: {
        inputDialog.resultDialog(answer.text, true)
    }
    onRejected: {
        inputDialog.resultDialog(answer.text, false)
    }
    onVisibleChanged: inputDialog.visible = visible
}
