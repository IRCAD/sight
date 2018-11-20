//import related modules
import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
// Import TutoEditorQml module
import TutoEditorQml 1.0

//window containing the application
ApplicationWindow {

    //title of the application
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

    // initialize the app manager
    Component.onCompleted: {
        appManager.initialize()
        appManager.onServiceCreated(strEditor1)
        appManager.onServiceCreated(strEditor2)
    }
    onClosing: appManager.uninitialize();

    // Instantiate the AppManager
    AppManager {
        id: appManager
    }

    //a button in the middle of the content area
    SStringEditor {
        id: strEditor1

        // @disable-check M16
        onStarted: textArea1.enabled = true

        // @disable-check M16
        onEdited: textArea1.text = str
    }

    SStringEditor {
        id: strEditor2

        // @disable-check M16
        onStarted: textArea2.enabled = true

        // @disable-check M16
        onEdited: textArea2.text = str
    }

    RowLayout {
        anchors.fill: parent
        TextArea {
            id: textArea1
            textColor: "#143bc2"
            Layout.fillWidth: true
            Layout.fillHeight: true
            enabled: false

            onTextChanged: {
                if (enabled == true) {
                    strEditor1.updateString(text)
                }
            }
        }

        TextArea {
            id: textArea2
            textColor: "#902190"
            Layout.fillHeight: true
            enabled: false

            onTextChanged: {
                if (enabled == true) {
                    strEditor2.updateString(text)
                }
            }
        }

    }
}
