import QtQuick 2.2
import QtQuick.Dialogs 1.0
    FileDialog {
        id: fileDialog
        title: "Please choose a file"

        folder: shortcuts.home

        modality: Qt.ApplicationModal

        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
        }
        onRejected: {
            console.log("Canceled")
        }
    }
