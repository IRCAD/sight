import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import PoCDialog 1.0

Item {
    width: 580
    height: 400
    SystemPalette { id: palette }
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
            bottom: bottomBar.top
            leftMargin: 12
        }
        ColumnLayout {
            spacing: 8
            Item { Layout.preferredHeight: 4 } // padding
            Label {
                font.bold: true
                text: "File dialog properties:"
            }
            RowLayout {
                Text {
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
                }
            }
            RowLayout {
                Text {
                    id: customizeLocation
                    text: "Location"
                    Layout.alignment: Qt.AlignBaseline
                }
                TextField {
                    id: windowLocationField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                    onTextChanged: pocDialogLocationDialogs.folder = windowLocationField.text
                }
            }
            RowLayout {
                id: filterRow
                property var nameFilters: []
                property var nameOfFilters: []
                Text {
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
                onCheckedStateChanged: pocDialogLocationDialogs.isFolder = fileDialogSelectFolder.checked
            }
            CheckBox {
                id: fileDialogSelectExisting
                text: "Select Existing Files"
                checked: true
                onCheckedStateChanged: pocDialogLocationDialogs.existing = fileDialogSelectExisting.checked
            }
            CheckBox {
                id: fileDialogSelectMultiple
                text: "Select Multiple Files"
                onCheckedStateChanged: pocDialogLocationDialogs.multiple = fileDialogSelectMultiple.checked
            }
            Label {
                id: labelFilter
                text: "<b>current filter:</b>" + fileDialog.nameFilters
            }
            Label {
                text: "<b>chosen files:</b> " + pocDialogLocationDialogs.result
            }
        }
    }

    Rectangle {
        id: bottomBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: buttonRow.height * 1.2
        color: Qt.darker(palette.window, 1.1)
        border.color: Qt.darker(palette.window, 1.3)
        Row {
            id: buttonRow
            spacing: 6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
            height: implicitHeight
            width: parent.width
            Button {
                text: "Open"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    pocDialogLocationDialogs.open();
                    filterRow.nameFilters = [];
                    filterRow.nameOfFilters = [];
                }
            }
            Button {
                text: "Pictures"
                tooltip: "go to my Pictures directory"
                anchors.verticalCenter: parent.verticalCenter
                enabled: fileDialog.shortcuts.hasOwnProperty("pictures")
                onClicked:
                {
                    var pictures = fileDialog.shortcuts.pictures;
                    windowLocationField.text = pictures.replace("file://", "");
                }
            }
            Button {
                text: "Home"
                tooltip: "go to my home directory"
                anchors.verticalCenter: parent.verticalCenter
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
