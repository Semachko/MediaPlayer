import QtQuick
import QtQuick.Controls.Basic

Button {
    id: root
    implicitHeight: 73
    implicitWidth: 73
    // width: 73
    // height: 73
    text: ""
    background: Item {
        Rectangle {
            id: background
            anchors.fill: parent
            color: pressed ? "black" : "#252525"
            opacity: hovered ? 1 : 0.5
            radius: 10
        }
        Rectangle {
            id: rect_border
            anchors.fill: parent
            color: "transparent"
            border.color: "white"
            border.width: 6
            radius: 10
        }

        Image {
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: -5
            scale: 0.8
            source: "qrc:/resources/icons/WhiteTriangle.svg"
        }
        Rectangle {
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 13
            width: 7
            height: 28
            color: "white"
            radius: 9
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
