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
            visible: hovered
            anchors.fill: parent
            color: "#5C5D5D"
            radius: 11
            opacity: 0.2
        }
        Image {
            id: muted
            visible: checked
            anchors.centerIn: parent
            source: "qrc:/images/MutedWhite.svg"
            fillMode: Image.PreserveAspectFit
        }
        Image {
            id: notmuted
            visible: !checked
            anchors.centerIn: parent
            source: "qrc:/images/NotMutedWhite.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
