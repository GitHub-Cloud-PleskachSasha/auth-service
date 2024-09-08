import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import "./UserPages"


ApplicationWindow {
    id:window
    visible: true
    width: 1000
    height: 700
    title: "test"

    property int defMargin: 10

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginPage
    }

    LoginPage {
        id: loginPage
        onButtonClicked: {
            stackView.push(registerPage);
        }
    }
    RegisterPage {
        id: registerPage
        visible: false
        onButtonClicked: {
            stackView.pop(loginPage);
        }
    }
}
