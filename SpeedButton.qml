import QtQuick
import QtQuick.Controls.Basic

Button {
    id: root
    implicitWidth: 85
    implicitHeight: 85
    checkable: true
    background: Item {
        Rectangle {
            id: rectangle
            visible: checked || hovered
            anchors.fill: parent
            color: "#5C5D5D"
            radius: 11
            opacity: checked ? 0.8 : 0.2
        }
        Image {
            id: icon
            anchors.centerIn: parent
            source: "qrc:/images/Speed.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
}
