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
    flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint

    Dialog {
        objectName: "dialog"
        id: dialog
        width: parent.width
        height: parent.height
        modal: true
        standardButtons: Dialog.Cancel | Dialog.Ok

        ColumnLayout {
            anchors.fill: parent

            Row {
                spacing: 5

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
                Label {
                    id:description

                    text: loggerDialog.message
                    // For text to wrap, a width has to be explicitly provided
                    width: 400
                    // This setting makes the text wrap at word boundaries when it goes
                    // past the width of the Text object
                    wrapMode: Text.WordWrap
                    font.bold: true
                }
            }

            CheckBox {
                id: checkbox

                text: checkState ? "Hide Details" : "Show Details"
                // the style permits to not show the default checkbox indicator but instead an image if set
                indicator: Image {
                    id: detailsIcon

                    width:  24
                    height: 24
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

            ListView {
                id: textDetails
                visible: false
                Layout.minimumWidth: 400
                Layout.topMargin: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: loggerModel
                snapMode: ListView.SnapToItem
                headerPositioning: ListView.OverlayHeader
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
                        implicitWidth: textDetails.width - 220
                        implicitHeight: 40
                    }
                }

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

                        implicitWidth: textDetails.width - 220
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
