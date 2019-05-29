import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Dialog {
    id: inputDialog

    // url of each icon
    property url critical: ""
    property url hidden: loggerDialog.hidden
    property url information: ""
    property url shown: loggerDialog.shown
    property url warning: ""

    // check the text of the error level and set the according icon
    function provideIcon(levelValue)
    {
        switch (levelValue) {
        case "Critical": return critical;
        case "Warning": return warning;
        case "Information": return information;
        default : return ""
        }
    }

    modality: Qt.ApplicationModal
    standardButtons: StandardButton.Cancel | StandardButton.Ok
    width: 500

    ColumnLayout {
        anchors.fill: parent
        Layout.maximumWidth: 500
        Layout.minimumWidth: 500
        Layout.preferredWidth: 500

        Row {
            spacing: 5

            // icon of the biggest type of error from the TableView
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

            text: checkedState ? "Hide Details" : "Show Details"
            height:50
            // the style permits to not show the default checkbox indicator but instead an image if set
            style: CheckBoxStyle {
                indicator: Image {
                    id: detailsIcon

                    width: 24
                    height: 24
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    antialiasing: true
                    source: checkbox.checkedState ? shown : hidden
                    cache : true;
                    asynchronous: true;
                }
            }

            // resize the dialog each time the checkedState change
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
                // delegate permits to add an image and a text in the same Column
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
                // ToDo Edit the row height to wrap content and change the color on styleData.row%2
                delegate: Component {
                    Item {
                        Text {
                            color: styleData.textColor
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
    onVisibleChanged: visible ? "" : reset()
}
