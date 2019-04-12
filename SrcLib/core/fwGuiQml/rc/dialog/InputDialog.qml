import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Dialog {
    id: inputDialog
    modality: Qt.ApplicationModal
    title: "Identification dialog"
    standardButtons: StandardButton.Cancel | StandardButton.Ok
    signal resultDialog(var answer, bool isOk)
    onAccepted: {
        resultDialog(answer.text, true)
    }
    onRejected: {
        resultDialog(answer.text, false)
    }

    ColumnLayout {
        id: column
        width: parent ? parent.width : 100
        Label {
            objectName: "message"
            text: "Enter Pin Code: "
            Layout.columnSpan: 2
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        TextField {
            id: answer
            objectName: "answer"
        }
    }
}
