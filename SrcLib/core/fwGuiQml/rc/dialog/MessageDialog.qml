import QtQuick 2.3
import QtQuick.Dialogs 1.2

MessageDialog {
    signal resultDialog(int flag)

    title: "Overwrite?"
    text: "file.txt already exists.  Replace?"
    onButtonClicked: {
        resultDialog(clickedButton)
    }
}
