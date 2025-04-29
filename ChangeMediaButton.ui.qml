import QtQuick
import QtQuick.Controls.Basic

Button {
    id: root
    width: 73
    height: 73
    text: ""
    background: Rectangle {
        id: rectangle
        radius: 10
        color: hovered ? "#06AA8F" : "#24423D"
        border.width: 7
        border.color: "white"
        Image {
            anchors.centerIn: rectangle
            anchors.horizontalCenterOffset: -5
            scale: 0.8
            source: pressed ? "qrc:/images/BlackTriangle.svg" : "qrc:/images/WhiteTriangle.svg"
        }
        Rectangle {
            anchors.centerIn: rectangle
            anchors.horizontalCenterOffset: 13
            width: 7
            height: 28
            color: pressed ? "#353535" : "white"
            radius: 9
        }
    }
}
