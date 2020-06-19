import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import styleQml 1.0
import Dialog 1.0

Window {
    id: window
    width: 500
    height: 300
    modality: Qt.ApplicationModal

    Dialog {
        id: dialog
        objectName: "dialog"
        parent: ApplicationWindow.overlay
        modal: true
        width: window.width
        height: window.height

        // this c++ object class contain an enum with all qml button and then set the standard button
        StandardButton {
            id: buttonSetting
            objectName: "standardButton"
            onButtonChanged: {
                if (button & StandardButton.Ok)
                {
                    // because setting int number from the enum doesn't work
                    // we need to recreate the group button box and change the visibility
                    okButton.visible = true;
                }
                if (button & StandardButton.Cancel)
                {
                    cancelButton.visible = true;
                }
                if (button & StandardButton.Yes)
                {
                    yesButton.visible = true;
                }
                if (button & StandardButton.No)
                {
                    noButton.visible = true;
                }
            }
        }

        RowLayout {
            id: row
            anchors.fill: parent
            Image {
                id: icon

                width: 48
                height: 48
                fillMode: Image.PreserveAspectFit
                mipmap: true
                source: messageDialog.icon
            }
            Label {
                text: messageDialog.message
                font.bold: true
                Layout.fillWidth: true
                width: parent.width
                wrapMode: Text.WordWrap
            }
        }

        // the footer contain the different button of the dialog box
        footer: DialogButtonBox {
            id: buttonBox
            // for each button we need to set the background to transparent to get similar style as other dialog
            Button {
                id: okButton

                Material.background: "transparent"
                text: qsTr("Ok")
                visible: false
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                highlighted: false
                onClicked: {
                    messageDialog.resultDialog(StandardButton.Ok)
                }
            }
            Button {
                id: cancelButton

                Material.background: "transparent"
                text: qsTr("Cancel")
                visible: false
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
                highlighted: false
                onClicked: {
                    messageDialog.resultDialog(StandardButton.Cancel)
                }
            }
            Button {
                id: yesButton

                Material.background: "transparent"
                text: qsTr("Yes")
                visible: false
                DialogButtonBox.buttonRole: DialogButtonBox.YesRole
                highlighted: false
                onClicked: {
                    messageDialog.resultDialog(StandardButton.Yes)
                }
            }
            Button {
                id: noButton

                Material.background: "transparent"
                text: qsTr("No")
                visible: false
                DialogButtonBox.buttonRole: DialogButtonBox.NoRole
                highlighted: false
                onClicked: {
                    messageDialog.resultDialog(StandardButton.No)
                }
            }
        }

        onAccepted: {
            window.close()
        }
        onRejected: {
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
