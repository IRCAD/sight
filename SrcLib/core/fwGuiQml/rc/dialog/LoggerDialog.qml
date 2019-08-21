import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12

import guiQml 1.0

Window{
    id: window
    width: 600
    height: 400
    modality: Qt.ApplicationModal

    Dialog {
        objectName: "dialog"
        id: dialog
        width: parent.width
        height: parent.height
        modal: true
        standardButtons: Dialog.Cancel | Dialog.Ok

        // column use only to resize everything without moving checkbox and rowLayout
        ColumnLayout {
            id: columnReposition
            width: window.width
            height: 50
        }

        ColumnLayout {
            id: column
            anchors.fill: checkbox.checkState ? parent : columnReposition

            Row {
                id: rowLayout
                Layout.alignment: Qt.AlignTop
                width: window.width

                // icon of the biggest type of error from the TableView
                Image {
                    id: icon

                    width: 48
                    height: 48
                    x: description.leftPadding
                    y: description.height / 2 - icon.height / 2 + 15
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    source: loggerDialog.icon
                }
                // this label is the summary of the number of errors
                Label {
                    id:description

                    text: loggerDialog.message
                    // For text to wrap, a width has to be explicitly provided
                    width: window.width
                    // This setting makes the text wrap at word boundaries when it goes
                    // past the width of the Text object
                    wrapMode: Text.WordWrap
                    font.bold: true
                }
            }

            // this checkbox show and unshow
            CheckBox {
                id: checkbox
                Layout.alignment: Qt.AlignTop

                text: checkState ? "Hide Details" : "Show Details"
                // the style permits to not show the default checkbox indicator but instead an image if set
                indicator: Image {
                    id: detailsIcon

                    width:  28
                    height: 28
                    x: checkbox.leftPadding
                    y: parent.height / 2 - height / 2
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    antialiasing: true
                    source: checkbox.checkState ? loggerDialog.shown : loggerDialog.hidden
                    cache : true;
                    asynchronous: true;
                }

                onCheckedChanged: {
                    textDetails.visible = checkState;
                }
            }

            // this list represent the log of all errors
            ListView {
                id: textDetails
                Layout.alignment: Qt.AlignBottom

                visible: false
                width: window.width
                Layout.topMargin: 40
                Layout.rightMargin: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: loggerModel
                snapMode: ListView.SnapToItem
                headerPositioning: ListView.PullBackHeader
                // the header represent the title of each row
                header: Row {
                    z: 2

                    ItemDelegate {
                        id: indexTitle

                        text: qsTr("Index")

                        font.bold: true
                        implicitWidth: 90
                        implicitHeight: 40
                    }

                    ItemDelegate {
                        id: levelTitle

                        text: qsTr("Level")

                        font.bold: true
                        implicitWidth: 130
                        implicitHeight: 40
                    }

                    ItemDelegate {
                        id: messageTitle

                        text: qsTr("Message")

                        font.bold: true
                        implicitWidth: textDetails.width - 310
                        implicitHeight: 40
                    }
                }

                // the delegate is the style of all item from the model
                delegate: Row {
                    ItemDelegate {
                        id: indexRow

                        text: index + 1

                        implicitWidth: 90
                        implicitHeight: 40
                    }

                    ItemDelegate {
                        id: levelRow
                        text: level

                        implicitWidth: 130
                        implicitHeight: 40
                    }

                    ItemDelegate {
                        id: messageRow
                        text: message

                        implicitWidth: textDetails.width - 310
                        implicitHeight: 40
                    }
                }
            }
        }

        onAccepted: {
            loggerDialog.resultDialog(true)
            window.close()
        }
        onRejected: {
            loggerDialog.resultDialog(false)
            window.close()
        }
        onVisibleChanged: visible ? "" : reset()
    }

    //Set Material Theme from Singleton
    Component.onCompleted: {
        Material.accent = Theme.accent
        Material.theme = Theme.theme
        Material.foreground = Theme.foreground
        Material.background = Theme.background
        Material.primary = Theme.primary
        Material.elevation = Theme.elevation
        window.show()
    }
}
