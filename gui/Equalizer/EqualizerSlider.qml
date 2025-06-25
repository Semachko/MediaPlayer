import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: root
    implicitWidth: 28
    implicitHeight: 200
    snapMode: Slider.SnapAlways
    orientation: Qt.Vertical

    background: Item {
        Rectangle {
            x: leftPadding + (availableWidth - width) / 2
            y: topPadding
            width: parent.width * 0.3
            height: availableHeight
            radius: 9999
            color: "#E0E0E0"
            clip: true

            Rectangle {
                x: 0
                y: visualPosition * parent.height
                width: parent.width
                height: (1 - visualPosition) * parent.height
                color: "#3F38FE"
                radius: 9999
            }
        }
    }

    handle: Rectangle {
        y: topPadding + visualPosition * (availableHeight - height)
        x: (root.width - width) / 2
        width: root.width
        height: root.width*0.5
        radius: 9999
        color: "#140F97"
        border.color: "white"
        border.width: pressed ? height * 0.3 : hovered ? height * 0.25 : height * 0.2
    }
}
