import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: root
    width: 28
    height: 200
    value: 0.5
    orientation: Qt.Vertical
    background: Item {
        Rectangle {
            x: leftPadding + (availableWidth - width) / 2
            y: topPadding
            width: parent.width * 0.35
            height: availableHeight
            radius: 9999
            color: "#E0E0E0"
            clip: true

            Rectangle {
                // x: 0
                // y: parent.height - height
                width: parent.width
                height: visualPosition * parent.height
                color: "#4B99D2"
                radius: 9999
            }
        }
    }

    handle: Rectangle {
        y: topPadding + visualPosition * (availableHeight - height)
        x: (root.width - width) / 2
        width: root.width
        height: root.width
        radius: 9999
        color: "#203A47"
        border.color: "white"
        border.width: pressed ? height * 0.3 : hovered ? height * 0.25 : height * 0.2
    }
}
