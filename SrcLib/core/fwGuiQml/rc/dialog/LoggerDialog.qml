import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Dialog {
    id: inputDialog

    property url critical: loggerDialog.critical
    property url hidden: loggerDialog.hidden
    property url information: loggerDialog.information
    property url shown: loggerDialog.shown
    property url warning: loggerDialog.warning

    function provideIcon(levelValue)
    {
        switch (levelValue) {
        case "Critical": return critical;
        case "Warning": return warning;
        case "Information": return information;
        }
    }

    modality: Qt.ApplicationModal
    title: loggerDialog.title
    standardButtons: StandardButton.Cancel | StandardButton.Ok
    width: 500

    ColumnLayout {
        anchors.fill: parent
        Layout.maximumWidth: 500
        Layout.minimumWidth: 500
        Layout.preferredWidth: 500

        Row {
            spacing: 5

            Image {
                id: icon

                width: 48
                height: 48
                mipmap: true
                source: loggerDialog.icon
                fillMode: Image.PreserveAspectFit
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

            text: "Check Box"
            height:50
            style: CheckBoxStyle {
                indicator: Image {
                    id: detailsIcon

                    width: 16
                    height: 16
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    antialiasing: true
                    source: checkbox.checkedState ? shown : hidden
                }
            }

            onCheckedChanged: {
                textDetails.visible = checkedState;
                if (!checkedState) {
                    inputDialog.width = 500;
                    inputDialog.height = 50;
                }
            }
        }
        TableView {
            id: textDetails

            visible: false
            Layout.minimumWidth: 400
            Layout.fillWidth: true
            Layout.fillHeight: true
            sortIndicatorVisible: true
            sortIndicatorColumn: 0
            model: loggerModel
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy: Qt.ScrollBarAsNeeded

            TableViewColumn {
                width: 50
                role: "index"
                delegate: Component {
                    Item {
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            text:  styleData.row + 1
                            color: styleData.textColor
                            elide: styleData.elideMode
                        }
                    }
                }
            }
            TableViewColumn {
                id:level

                role: "level"
                title: "Level"
                width: 120
                delegate: Component {
                    id: imageDelegate

                    Item {
                        Image {
                            fillMode: Image.PreserveAspectFit
                            height:20
                            width: 20
                            cache : true;
                            asynchronous: true;
                            mipmap: true
                            antialiasing: true
                            source: inputDialog.provideIcon(styleData.value)
                        }
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            text:  styleData.value
                            color: styleData.textColor
                            elide: styleData.elideMode
                        }
                    }
                }
            }
            TableViewColumn {
                id:message

                role: "message"
                title: "Message"
                width: textDetails.width - 180
                delegate: Component {
                    Item {
                        Text {
                            color: styleData.textColor
                            //color: styleData.row % 2 ? "red" : "black"
                            elide: styleData.elideMode
                            text: styleData.value
                            width: message.width
                        }
                    }
                }
            }
        }
    }

    onAccepted: {
        loggerDialog.resultDialog(true)
    }
    onRejected: {
        loggerDialog.resultDialog(false)
    }
    onVisibleChanged: loggerDialog.visible = visible
}
