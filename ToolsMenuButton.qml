import QtQuick
import QtQuick.Controls.Basic

Button {
    property string label
    property url iconSRC
    property var onClickedFunc
    id: button
    anchors.left: parent.left
    anchors.right: parent.right
    height: 50
    contentItem: Text {
        verticalAlignment: Text.AlignVCenter
        leftPadding: 50
        text: button.label
        color: "white"
        font.pointSize: 18
    }
    background: Rectangle {
        Image {
            scale: 0.8
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            source: button.iconSRC
            fillMode: Image.PreserveAspectFit
        }
        radius: 5
        color: button.hovered ? "#5C5D5D" : "transparent"
        border.width: button.pressed ? 3 : 0
        border.color: "white"
    }
    onClicked: button.onClickedFunc
}
// Button {
//                 id: filters
//                 anchors.left: parent.left
//                 anchors.right: parent.right
//                 height: 50
//                 contentItem: Text {
//                     verticalAlignment: Text.AlignVCenter
//                     leftPadding: 50
//                     text: "Filters"
//                     color: "white"
//                     font.pointSize: 18
//                 }
//                 background: Rectangle {
//                     Image {
//                         scale: 0.8
//                         anchors.verticalCenter: parent.verticalCenter
//                         anchors.left: parent.left
//                         anchors.leftMargin: 10
//                         source: "qrc:/images/filters.svg"
//                         fillMode: Image.PreserveAspectFit
//                     }
//                     radius: 5
//                     color: filters.hovered ? "#5C5D5D" : "transparent"
//                     border.width: filters.pressed ? 3 : 0
//                     border.color: "white"
//                 }
//             }
