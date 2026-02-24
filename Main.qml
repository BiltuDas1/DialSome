import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import DialSome


ApplicationWindow {
    id: mainWindow
    visible: true
    width: 360
    height: 640
    title: "DialSome"
    color: "#000000"

    leftPadding: 15
    rightPadding: 15

    Backend {
        id: myBackend

        Component.onCompleted: {
            myBackend.fetchStartupData();
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: titleSection
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            color: "transparent"
            border.width: 1

            RowLayout {
                anchors.fill: parent

                Text {
                    text: "DialSome"
                    color: "#5B89F7"
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    font.pixelSize: 18
                }

                Rectangle {
                    width: 10
                    height: 10
                    color: myBackend.serverConnected ? "green" : "red"
                    radius: width / 2

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 2
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Rectangle {
            id: optionsSection
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            color: "#121212"
            Layout.topMargin: 10
            radius: 8

            // 0 = Favourites, 1 = Recents, 2 = Contacts
            property int selectedIndex: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 3

                Rectangle {
                    id: optionsSectionFavourites
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: optionsSection.selectedIndex === 0 ? mainWindow.color : "transparent"
                    radius: 5

                    Text {
                        text: "Favorites"
                        color: optionsSection.selectedIndex === 0 ? "#5B89F7" : "white"
                        anchors.centerIn: parent

                        font.pixelSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: optionsSection.selectedIndex = 0
                    }

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }

                Rectangle {
                    id: optionsSectionRecents
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: optionsSection.selectedIndex === 1 ? mainWindow.color : "transparent"
                    radius: 5

                    Text {
                        text: "Recents"
                        color: optionsSection.selectedIndex === 1 ? "#5B89F7" : "white"
                        anchors.centerIn: parent

                        font.pixelSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: optionsSection.selectedIndex = 1
                    }

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }

                Rectangle {
                    id: optionsSectionContacts
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: optionsSection.selectedIndex === 2 ? mainWindow.color : "transparent"
                    radius: 5

                    Text {
                        text: "Contacts"
                        color: optionsSection.selectedIndex === 2 ? "#5B89F7" : "white"
                        anchors.centerIn: parent

                        font.pixelSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: optionsSection.selectedIndex = 2
                    }

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                }
            }
        }

        Rectangle {
            id: mainSection
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            Layout.topMargin: 10

            ColumnLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 20

                TextField {
                    id: roomInput
                    placeholderText: "Enter Room ID (e.g. 123)"
                    color: "white"
                    background: Rectangle { color: "#333"; radius: 5 }
                    Layout.preferredWidth: 250
                }

                Button {
                    text: "Start / Join Call"
                    onClicked: myBackend.startCall(roomInput.text)
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: myBackend.message
                    color: "#5B89F7"
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // Push everything to the top
        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Login with Google"
            onClicked: myBackend.loginWithGoogle("87640868239-dje4suitg3fi100c8hirlunckcji4g40.apps.googleusercontent.com")
        }

        Connections {
            target: myBackend
            function onLoginFinished(email, name, token) {
                console.log("QML Success: " + name + " (" + email + ")");
                myBackend.setMessage("Logged in as: " + name);
            }

            function onLoginError(error) {
                console.log("QML Error: " + error);
                myBackend.setMessage("Login Error: " + error);
            }
        }
    }
}
