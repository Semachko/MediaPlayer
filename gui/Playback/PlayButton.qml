import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Shapes

Button {
    id: root
    text: ""
    implicitHeight: 86
    implicitWidth: 86
    checkable: true
    background: Item {
        Rectangle {
            id: background
            anchors.fill: parent
            color: pressed ? "black" : "#252525"
            opacity: hovered ? 1 : 0.5
            radius: 9999
        }
        Rectangle {
            id: rect_border
            anchors.fill: parent
            color: "transparent"
            border.color: "white"
            border.width: 7
            radius: 9999
        }

        Image {
            id: resume
            visible: !checked
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 4
            source: "qrc:/resources/icons/WhiteTriangle.svg"
        }
        Item {
            id: pause
            visible: checked
            anchors.fill: parent
            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -8
                width: 9
                height: 28
                color: "white"
                radius: 4
                border.width: 0
            }
            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 8
                width: 9
                height: 28
                color: "white"
                radius: 4
                border.width: 0
            }
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
