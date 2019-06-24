import QtQuick 2.12
import QtQuick.Dialogs 1.3

FileDialog {
    id: fileDialog

    // check if we are on macOs because there is a bug on mac that not permit
    // modal window for FileDialog
    modality: Qt.platform.os == "osx" ? Qt.NonModal : Qt.ApplicationModal

    onAccepted: {
        locationDialog.resultDialog(fileUrls)
    }

    onRejected: {
        locationDialog.resultDialog(fileUrls)
        console.log()
    }
    onVisibleChanged: visible ? "" : rejected()
    // specify the selected extension to c++
    onSelectedNameFilterChanged: locationDialog.filterSelected = selectedNameFilter
}
