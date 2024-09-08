import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    width: 1000
    height: 700

    signal buttonClicked();

    ListModel {
        id: inputModel
        ListElement { placeholder: "Enter your email or name..."; type: "email"}
        ListElement { placeholder: "Enter your password..."; type: "password"}
    }

    Repeater {
        model: inputModel
        delegate: Rectangle {
            width: 300
            height: 50
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: (index + 4) * parent.height * 0.1
            border.color: "black"
            border.width: 2
            radius: 5

            TextField {
                id: inputTextField
                width: parent.width - 10
                height: parent.height - 10
                anchors.centerIn: parent
                placeholderText: model.placeholder
                font.pixelSize: 20
                echoMode: model.type === "password" ? TextField.Password : TextField.Normal
            }
        }
    }

    Button {
        text: "Sign in"
        width: 300
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.6
        onClicked: {
            // Implement sign in logic here
        }
    }

    Button {
        text: "Go to Register"
        width: 300
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.7
        onClicked: {
            stackView.push(Qt.resolvedUrl("RegisterPage.qml"))
        }
    }
}
