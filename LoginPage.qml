import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: loginPage
    anchors.fill: parent
    color: "#000000"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Welcome to DialSome"
            color: "#5B89F7"
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            text: "Login with Google"
            Layout.preferredWidth: 200
            Layout.alignment: Qt.AlignHCenter
            // You can still access the 'myBackend' id if it's defined in Main.qml
            onClicked: myBackend.loginWithGoogle("87640868239-dje4suitg3fi100c8hirlunckcji4g40.apps.googleusercontent.com")
        }
    }
}
