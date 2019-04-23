import QtQuick 2.9
import QtQuick.Dialogs 1.2

FileDialog {
    id: fileDialog

    title: locationDialog.title
    folder: locationDialog.folder
    nameFilters: locationDialog.filter
    modality: Qt.ApplicationModal
    selectExisting: locationDialog.existing
    selectFolder: locationDialog.isFolder
    selectMultiple: locationDialog.multiple

    onAccepted: {
        locationDialog.resultDialog(fileUrls)
    }

    onRejected: {
        locationDialog.resultDialog(fileUrls)
    }
    onVisibleChanged: locationDialog.visible = visible
    onSelectedNameFilterChanged: locationDialog.filterSelected = selectedNameFilter
}
