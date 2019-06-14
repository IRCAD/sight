import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3

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
        Material.theme: Material.System
        Material.accent: Material.LightBlue

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
                Text {
                    id:description

                    text: loggerDialog.message
                    // For text to wrap, a width has to be explicitly provided
                    width: 400
                    // This setting makes the text wrap at word boundaries when it goes
                    // past the width of the Text object
                    wrapMode: Text.WordWrap
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
                headerPositioning: ListView.PullBackHeader
                header: Row {
                    ItemDelegate {
                        id: indexTitle
                        text: qsTr("ItemDelegate")

                        contentItem: Text {
                            rightPadding: indexTitle.spacing
                            text: "Index"
                            font: indexTitle.font
                            color: indexTitle.enabled ? (indexTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            id: indexRect
                            implicitWidth: 90
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: indexTitle.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: indexTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary
                                anchors.bottom: indexRect.bottom
                            }
                        }
                    }
                    ItemDelegate {
                        id: levelTitle
                        text: qsTr("ItemDelegate")

                        contentItem: Text {
                            rightPadding: levelTitle.spacing
                            text: "Level"
                            font: levelTitle.font
                            color: levelTitle.enabled ? (levelTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            id: levelRect
                            implicitWidth: 130
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: levelTitle.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: levelTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary
                                anchors.bottom: levelRect.bottom
                            }
                        }
                    }
                    ItemDelegate {
                        id: messageTitle
                        text: qsTr("ItemDelegate")
                        contentItem: Text {
                            rightPadding: messageTitle.spacing
                            text: "Message"
                            font: messageTitle.font
                            color: messageTitle.enabled ? (messageTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            id: messageRect
                            implicitWidth: textDetails.width - 220
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: messageTitle.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: messageTitle.down ? Material.shade(Material.primary, Material.ShadeA700) : Material.primary
                                anchors.bottom: messageRect.bottom
                            }
                        }
                    }
                }
                delegate: Row {
                    ItemDelegate {
                        id: indexRow
                        text: qsTr("ItemDelegate")

                        contentItem: Text {
                            rightPadding: indexRow.spacing
                            text: index + 1
                            font: indexRow.font
                            color: indexRow.enabled ? (indexRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            implicitWidth: 90
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: indexRow.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: indexRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent
                                anchors.bottom: parent.bottom
                            }
                        }
                    }
                    ItemDelegate {
                        id: levelRow
                        text: qsTr("ItemDelegate")

                        contentItem: Text {
                            rightPadding: levelRow.spacing
                            text: level
                            font: levelRow.font
                            color: levelRow.enabled ? (levelRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            implicitWidth: 130
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: levelRow.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: levelRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent
                                anchors.bottom: parent.bottom
                            }
                        }
                    }
                    ItemDelegate {
                        id: messageRow
                        text: qsTr("ItemDelegate")

                        contentItem: Text {
                            rightPadding: messageRow.spacing
                            text: message
                            font: messageRow.font
                            color: messageRow.enabled ? (messageRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent) : "#bdbebf"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            implicitWidth: textDetails.width - 220
                            implicitHeight: 40
                            opacity: enabled ? 1 : 0.3
                            color: messageRow.down ? Material.color(Material.Grey, Material.Shade300) : Material.color(Material.Grey, Material.Shade200)

                            Rectangle {
                                width: parent.width
                                height: 1
                                color: messageRow.down ? Material.shade(Material.accent, Material.ShadeA700) : Material.accent
                                anchors.bottom: parent.bottom
                            }
                        }
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
    Component.onCompleted: show()
}
