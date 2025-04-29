import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Shapes

Button {
    id: root
    text: ""
    width: 86
    height: 86
    checkable: true
    background: Rectangle {
        id: rectangle
        width: 86
        height: 86
        color: hovered ? "#66EAB1" : "#ffffff"
        radius: 9999
        border.width: 15
        border.color: "#283743"
        Image {
            id: resume
            visible: !checked
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 4
            source: pressed ? "qrc:/images/WhiteTriangle.svg" : "qrc:/images/BlackTriangle.svg"
        }
        Item {
            id: pause
            visible: checked
            anchors.fill: rectangle
            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: -8
                width: 9
                height: 28
                color: pressed ? "white" : "#353535"
                radius: 4
                border.width: 0
            }
            Rectangle {
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 8
                width: 9
                height: 28
                color: pressed ? "white" : "#353535"
                radius: 4
                border.width: 0
            }
        }
    }
}
