import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Item {
    width: 580
    height: 400
    SystemPalette { id: palette }
    clip: true

    //! [filedialog]

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
                }
            }
            RowLayout {
                Text {
                    id: customizeLocation
                    text: "Window Title"
                    Layout.alignment: Qt.AlignBaseline
                }
                TextField {
                    id: windowLocationField
                    Layout.alignment: Qt.AlignBaseline
                    Layout.fillWidth: true
                }
            }
            CheckBox {
                id: fileDialogSelectFolder
                text: "Select Folder"
            }
            CheckBox {
                id: fileDialogSelectExisting
                text: "Select Existing Files"
                checked: true
            }
            CheckBox {
                id: fileDialogSelectMultiple
                text: "Select Multiple Files"
                Binding on checked { value: fileDialog.selectMultiple }
            }
            Label {
                text: "<b>current view folder:</b> "
            }
            Label {
                text: "<b>name filters:</b> {" + fileDialog.nameFilters + "}"
            }
            Label {
                text: "<b>current filter:</b>" + fileDialog.selectedNameFilter
            }
            Label {
                text: "<b>chosen files:</b> " + fileDialog.fileUrls
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
                onClicked: fileDialog.open()
            }
            Button {
                text: "Pictures"
                tooltip: "go to my Pictures directory"
                anchors.verticalCenter: parent.verticalCenter
                enabled: fileDialog.shortcuts.hasOwnProperty("pictures")
                onClicked: fileDialog.folder = fileDialog.shortcuts.pictures
            }
            Button {
                text: "Home"
                tooltip: "go to my home directory"
                anchors.verticalCenter: parent.verticalCenter
                enabled: fileDialog.shortcuts.hasOwnProperty("home")
                onClicked: fileDialog.folder = fileDialog.shortcuts.home
            }
        }
    }
}
