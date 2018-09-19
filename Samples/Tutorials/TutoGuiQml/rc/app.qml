import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0

ApplicationWindow {
    id: mainWindow
    title: qsTr("TutoGUI")
    visible: true

    minimumWidth: 800
    minimumHeight: 600

    Item {
        anchors.fill: parent
        focus: true

        Keys.onEscapePressed: {
            Qt.quit();
        }
    }

    menuBar: MenuBar {
        Menu {
            title: "My Menu"
            MenuItem {
                checkable: myItem1.checkable
                checked: myItem1.checked
                text: "My Item 1"
                shortcut: "1"
                onCheckedChanged: {
                    myItem1.checked = checked
                }
            }
            MenuSeparator {}
            MenuItem {
                checkable: myItem2.checkable
                checked: myItem2.checked
                enabled: myItem2.enabled
                text: "My Item 2"
                shortcut: "2"
                onCheckedChanged: {
                    myItem2.checked = checked
                }
            }
            MenuItem {
                checkable: myItem3.checkable
                checked: myItem3.checked
                enabled: myItem3.enabled
                text: "My Item 3"
                shortcut: "3"
                onCheckedChanged: {
                    myItem3.checked = checked
                }
            }
            MenuSeparator {}
            MenuItem {
                checkable: myMonkey1.checkable
                checked: myMonkey1.checked
                enabled: myMonkey1.enabled
                text: "My Item A"
                shortcut: "A"
                onCheckedChanged: {
                    myMonkey1.checked = checked
                }
            }
            MenuItem {
                checkable: myMonkey2.checkable
                checked: myMonkey2.checked
                enabled: myMonkey2.enabled
                text: "My Item B"
                shortcut: "B"
                onCheckedChanged: {
                    myMonkey2.checked = checked
                }
            }
        }
        Menu {
            title: "My Menu 2"
            MenuItem {
                checkable: myItem1.checkable
                checked: myItem1.checked
                text: "My Item 1"
                onCheckedChanged: {
                    myItem1.checked = checked
                }
            }
            MenuItem {
                checkable: true
                checked: myWarning1.checked
                text: "Start 2-3 / Stop A-B"
                onCheckedChanged: {
                    myWarning1.checked = checked
                }
            }
            MenuItem {
                checkable: true
                checked: myWarning2.checked
                text: "Start A-B / Stop 2-3"
                onCheckedChanged: {
                    myWarning2.checked = checked
                    if (checked && myMonkey1.checked == false && myMonkey2.checked == false) {
                        myMonkey2.checked = checked
                    }
                }
            }
        }
        Menu {
            title: "My Menu 3"
            enabled: true
        }
    }

    toolBar: ToolBar {
        width: parent.width
        height: 35
        RowLayout {
            anchors.fill: parent
            ToolButton {
                id: playButton
                width: parent.height
                iconSource: "icons/start.svg"
                onClicked: {
                    togglePlayPause(true, false)
                }
            }
            ToolButton {
                id: pauseButton
                iconSource: "icons/pause.svg"
                visible: false
                onClicked: {
                    togglePlayPause(false, true)
                }
            }
            ToolButton {
                id: stopButton
                iconSource: "icons/stop.svg"
                enabled: false
                onClicked: {
                    togglePlayPause(false, false);
                }
            }
            ToolBarSeparator {}
            ToolButton {
                id: myItem1
                checkable: true
                checked: true
                iconSource: "icons/important.png"
            }
            ToolBarSeparator {}
            ToolButton {
                property bool force: false

                id: myItem2
                checked: true
                checkable: true
                enabled: false
                iconSource: "icons/system.png"

                onCheckedChanged: {
                    if (!force) {
                        myItem3.force = true
                        myItem3.checked = !checked
                        myItem3.force = false
                    }
                    myItem2Fork.checked = checked
                    force = false
                }
            }
            ToolButton {
                property bool force: false
                id: myItem3
                checkable: true
                enabled: false
                iconSource: "icons/system.png"
                onCheckedChanged: {
                    if (!force) {
                        myItem2.force = true
                        myItem2.checked = !checked
                        myItem2.force = false
                    }
                    myItem3Fork.checked = checked
                    force = false
                }
            }
            ToolBarSeparator {}
            ToolButton {
                property bool force: false

                id: myMonkey1
                enabled: false
                checkable: true
                iconSource: "icons/monkey.png"
                onCheckedChanged: {
                    if (!force) {
                        myMonkey2.force = true
                        myMonkey2.checked = !checked
                        myMonkey2.force = false
                    }
                    myMonkey1Fork.checked = checked
                    force = false
                }
                PropertyAnimation {
                    id: animation
                    target: myMonkey1
                    property: "rotation"
                    from: 0;to: 360;duration: 5000
                    loops: Animation.Infinite
                }
            }
            ToolButton {
                property bool force: false

                id: myMonkey2
                enabled: false
                checkable: true
                iconSource: "icons/monkey.png"
                onCheckedChanged: {
                    if (!force) {
                        myMonkey1.force = true
                        myMonkey1.checked = !checked
                        myMonkey1.force = false
                    }
                    myMonkey2Fork.checked = checked
                    force = false
                }
            }
            ToolBarSeparator {}
            ToolButton {
                property bool force: false

                id: myWarning1
                checkable: true
                checked: true
                iconSource: "icons/important.png"
                onCheckedChanged: {
                    if (!force) {
                        myWarning2.force = true
                        myWarning2.checked = !checked
                    }
                    myItem2.enabled = checked
                    myItem3.enabled = checked
                    force = false
                }
            }
            ToolButton {
                property bool force: false

                id: myWarning2
                checkable: true
                iconSource: "icons/important.png"
                onCheckedChanged: {
                    if (!force) {
                        myWarning1.force = true
                        myWarning1.checked = !checked
                    }
                    myMonkey1.enabled = checked
                    myMonkey2.enabled = checked
                    force = false
                }
            }
            Item { Layout.fillWidth: true }
            Button {
                id: myDropDown
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                onClicked: menu.popup()
                text: "hide/show"
                menu: Menu {
                    id: menu
                    MenuItem {
                        id: playButtonMenu
                        iconSource: "icons/start.svg"
                        text: "Play"
                        onTriggered: {
                            togglePlayPause(true, false)
                        }
                    }
                    MenuItem {
                        id: pauseButtonMenu
                        iconSource: "icons/pause.svg"
                        text: "Pause"
                        visible: false
                        onTriggered: {
                            togglePlayPause(false, true)
                        }
                    }
                    MenuItem {
                        id: stopButtonMenu
                        iconSource: "icons/stop.svg"
                        enabled: false
                        text: "Stop"
                        onTriggered: {
                            togglePlayPause(false, false)
                        }
                    }
                }
            }
            ToolButton {
                iconSource: "icons/unreadable.png"
                onClicked: {
                    Qt.quit()
                }
            }
        }
    }
    ColumnLayout {
        spacing: 8
        anchors.fill: parent

        View {
            name: "View 1"
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height / 2
            content: [
               RowLayout {
                   anchors.fill: parent

                   View {
                    name: "View 3"
                    Layout.preferredHeight: parent.height
                    Layout.fillWidth: true
                    implicitWidth: parent.width * 0.3
                    Layout.fillHeight: true
                    content: [
                        TabView {
                            height: parent.height - 20
                            width: parent.width - 20
                            x: 10
                            y: 10

                            Tab {
                                title: "Tab1"
                            }
                            Tab {
                                title: "Tab2"
                            }
                            Tab {
                                title: "Tab3"
                            }
                        }

                    ]
                   }
                   View {
                       name: "View 4"
                       Layout.preferredHeight: parent.height
                       Layout.fillWidth: true
                       Layout.fillHeight: true
                       implicitWidth: parent.width * 0.3
                   }
                   View {
                       name: "View 5"
                       Layout.preferredHeight: parent.height
                       Layout.fillWidth: true
                       Layout.fillHeight: true
                       implicitWidth: parent.width * 0.3
                   }
               }
           ]
        }
        View {
            name: "View 2"
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height / 2

            content: [
                SplitView {
                    anchors.fill: parent
                    orientation: Qt.Horizontal

                    View {
                        name: "CardinalView1"
                        width: 200
                        Layout.minimumWidth: 100

                        content: [
                            ToolBar {
                                id: forkedToolBar

                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                height: 35
                                style: ToolBarStyle {
                                    background: Rectangle {
                                       color: "transparent"
                                    }
                                }
                                RowLayout {
                                    ToolButton {
                                        id: myItem2Fork
                                        checked: myItem2.checked
                                        checkable: myItem2.checkable
                                        enabled: myItem2.enabled
                                        iconSource: "icons/system.png"
                                        onCheckedChanged: {
                                            myItem2.checked = checked
                                        }
                                    }
                                    ToolButton {
                                        id: myItem3Fork
                                        checkable: myItem3.checkable
                                        checked: myItem3.checked
                                        enabled: myItem3.enabled
                                        iconSource: "icons/system.png"
                                        onCheckedChanged: {
                                            myItem3.checked = checked
                                        }
                                    }
                                    ToolBarSeparator {}
                                    ToolButton {
                                        id: myMonkey1Fork
                                        enabled: myMonkey1.enabled
                                        checkable: myMonkey1.checkable
                                        checked: myMonkey1.checked
                                        iconSource: "icons/monkey.png"
                                        onCheckedChanged: {
                                            myMonkey1.checked = checked
                                        }
                                    }
                                    ToolButton {
                                        id: myMonkey2Fork
                                        enabled: myMonkey1.enabled
                                        checkable: myMonkey1.checkable
                                        checked: myMonkey2.checked
                                        iconSource: "icons/monkey.png"
                                        onCheckedChanged: {
                                            myMonkey2.checked = checked
                                        }
                                    }
                                }
                            }

                        ]

                    }
                    SplitView {
                        Layout.minimumWidth: 50
                        Layout.fillWidth: true
                        orientation: Qt.Vertical

                        View {
                            name: "CardinalView2"
                            height: 100
                        }
                        View {
                            name: "CardinalView3"
                            height: 100
                        }
                    }
                }
            ]
        }
        Component.onCompleted: animation.start()
    }

    function    togglePlayPause(state, pause) {
        pauseButton.visible = state
        pauseButtonMenu.visible = state
        playButton.visible = !state
        playButtonMenu.visible = !state
        if (pause === false) {
            stopButton.enabled = state
            stopButtonMenu.enabled = state
        }
    }
}
