import QtQuick 2.9
import QtQuick.Dialogs 1.2

FileDialog {
    id: fileDialog

    title: locationDialog.title
    folder: locationDialog.folder
    nameFilters: locationDialog.filter
    modality: Qt.platform.os == "osx" ? Qt.NonModal : Qt.ApplicationModal
    selectExisting: locationDialog.existing
    selectFolder: locationDialog.isFolder
    selectMultiple: locationDialog.multiple

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
