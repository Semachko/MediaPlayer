import QtQuick
import QtQuick.Controls.Basic

Button {
    property string label
    property url iconSRC
    id: button
    checkable: true
    implicitWidth: 265
    implicitHeight: 50
    contentItem: Text {
        id: txt
        verticalAlignment: Text.AlignVCenter
        leftPadding: 50
        bottomPadding: 3
        text: button.label
        color: "white"
        font.pointSize: 18
    }
    background: Item{
        Rectangle {
            id: rect
            anchors.fill: parent
            radius: 5
            color: button.hovered ? "#9D9D9D" : "transparent"
            opacity: 0.5
            border.width: button.pressed ? 3 : 0
            border.color: "white"
        }
        Image {
            id: iconImg
            scale: 0.8
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            source: button.iconSRC
            fillMode: Image.PreserveAspectFit
        }
    }
    Switch {
        id: powerSwitch
        visible: button.checkable
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        scale: parent.height/height * 0.7
        checked: false
        onClicked: button.checked = powerSwitch.checked
    }
}
