import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import DialSome
import "ui"


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

    StackView {
        id: mainStack
        anchors.fill: parent
        initialItem: myBackend.isLoggedIn() ? mainDashboardComponent : loginPageComponent
    }

    Component {
        id: loginPageComponent
        LoginPage {}
    }

    Component {
        id: mainDashboardComponent
        MainPage {}
    }

    Connections {
        target: myBackend
        function onLoginFinished(email, name, userid, refresh_token) {
            myBackend.showToast("Welcome " + name)
            mainStack.replace(mainDashboardComponent)
        }
        function onLoginError(error) {
            myBackend.showToast(error)
        }
    }
}
