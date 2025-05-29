import QtQuick
import QtQuick.Controls.Basic

Button {
    property string label
    property url iconSRC
    id: button
    implicitWidth: 200
    height: 50
    checkable: true
    contentItem: Text {
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
            color: button.hovered ? "#9D9D9D" : checked ? "#4A4A4A" : "transparent"
            opacity: 0.5
            border.width: button.pressed ? 3 : 0
            border.color: "white"
        }
        Image {
            scale: 0.8
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            source: button.iconSRC
            fillMode: Image.PreserveAspectFit
        }
    }
}
