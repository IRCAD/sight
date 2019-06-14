import QtQuick 2.2
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import PoCDialog 1.0
import "style" as Sight

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

    ScrollView {
        id: scrollView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            leftMargin: 12
        }
        ColumnLayout {
            spacing: 8
            Item { Layout.preferredHeight: 4 } // padding
            Sight.Label {
                font.bold: true
                text: "File dialog properties:"
            }
            RowLayout {
                Sight.Text {
                    id: customizeTitle
                    text: "Window Title"
                    Layout.alignment: Qt.AlignBaseline
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
                Sight.Text {
                    id: customizeLocation
                    text: "Location"
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
                Sight.Text {
                    id: customizeFilter
                    text: "Filters"
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
                Sight.Button {
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
            Sight.Label {
                id: labelFilter
                text: "<b>current filter:</b>" + fileDialog.nameFilters
            }
            Sight.Label {
                text: "<b>chosen files:</b> " + pocDialogLocationDialogs.result
            }
            RowLayout {
                id: buttonRow
                Sight.Button {
                    text: "Open"
                    onClicked: {
                        pocDialogLocationDialogs.open();
                        filterRow.nameFilters = [];
                        filterRow.nameOfFilters = [];
                    }
                }
                Sight.Button {
                    text: "Pictures"
                    enabled: fileDialog.shortcuts.hasOwnProperty("pictures")
                    onClicked:
                    {
                        var pictures = fileDialog.shortcuts.pictures;
                        windowLocationField.text = pictures.replace("file://", "");
                    }
                }
                Sight.Button {
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

}
