import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import guiQml 1.0
import PoCDialog 1.0

Item {
    clip: true

    PocDialogLocationDialogs{
        id: pocDialogLocationDialogs
    }

    //! [filedialog]
    FileDialog {
        id: fileDialog
        visible:  false
    }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 0
            Label {
                font.bold: true
                text: "File dialog properties:"
            }
            RowLayout {
                Label {
                    id: customizeTitle
                    text: "Window Title"
                    Layout.alignment: Qt.AlignBaseline
                    font.italic: true
                }
                TextField {
                    id: windowTitleField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                    text: "Custom Dialog"
                    onTextChanged: pocDialogLocationDialogs.title = windowTitleField.text
                    Component.onCompleted: pocDialogLocationDialogs.title = windowTitleField.text
                }
            }
            RowLayout {
                Label {
                    id: customizeLocation
                    text: "Location"
                    font.italic: true
                    Layout.alignment: Qt.AlignBaseline
                }
                TextField {
                    id: windowLocationField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                    onTextChanged: pocDialogLocationDialogs.folder = windowLocationField.text
                    Component.onCompleted: pocDialogLocationDialogs.folder = windowLocationField.text
                }
            }
            RowLayout {
                id: filterRow
                property var nameFilters: []
                property var nameOfFilters: []
                // filters extension to see only some type of file
                Label {
                    id: customizeFilter
                    text: "Filters"
                    font.italic: true
                    Layout.alignment: Qt.AlignBaseline
                }

                TextField {
                    id: windowFilterField
                    placeholderText: "name of the filter"
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignBaseline
                }
                TextField {
                    id: windowFilterExtensionField
                    Layout.fillWidth: true
                    placeholderText: "*.vtk"
                    Layout.alignment: Qt.AlignBaseline
                }
                Button {
                    text: "Add"
                    onClicked: {
                        filterRow.nameFilters.push(windowFilterExtensionField.text)
                        filterRow.nameOfFilters.push(windowFilterField.text)
                        windowFilterField.text = ""
                        windowFilterExtensionField.text = ""
                        fileDialog.nameFilters = filterRow.nameFilters
                        pocDialogLocationDialogs.filter = filterRow.nameOfFilters
                        pocDialogLocationDialogs.filterExt = filterRow.nameFilters
                    }
                }
            }
            CheckBox {
                id: fileDialogSelectFolder
                text: "Select Folder"
                onCheckStateChanged: pocDialogLocationDialogs.isFolder = fileDialogSelectFolder.checked
            }
            CheckBox {
                id: fileDialogSelectExisting
                text: "Select Existing Files"
                checked: true
                onCheckStateChanged: pocDialogLocationDialogs.existing = fileDialogSelectExisting.checked
            }
            CheckBox {
                id: fileDialogSelectMultiple
                text: "Select Multiple Files"
                onCheckStateChanged: {
                    if (fileDialogSelectMultiple.checked)
                    {
                        fileDialogSelectExisting.checked = true
                    }
                    pocDialogLocationDialogs.multiple = fileDialogSelectMultiple.checked
                }
            }
            Label {
                id: labelFilter
                text: "current filter: " + fileDialog.nameFilters
                font.bold: true
            }
            Label {
                text: "chosen files: " + pocDialogLocationDialogs.result
                font.bold: true
            }
            RowLayout {
                id: buttonRow
                Button {
                    text: "Open"
                    onClicked: {
                        pocDialogLocationDialogs.open();
                        filterRow.nameFilters = [];
                        filterRow.nameOfFilters = [];
                    }
                }
                // to set the location to picture location folder
                Button {
                    text: "Pictures"
                    enabled: fileDialog.shortcuts.hasOwnProperty("pictures")
                    onClicked:
                    {
                        var pictures = fileDialog.shortcuts.pictures;
                        windowLocationField.text = pictures.replace("file://", "");
                    }
                }
                // to set the location to home location folder
                Button {
                    text: "Home"
                    enabled: fileDialog.shortcuts.hasOwnProperty("home")
                    onClicked:
                    {
                        var home = fileDialog.shortcuts.home;
                        windowLocationField.text = home.replace("file://", "");
                    }
                }
            }
        }

}
