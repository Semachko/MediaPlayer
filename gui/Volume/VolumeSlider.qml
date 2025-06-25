import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: root
    width: 200
    height: 28
    value: 0.2
    stepSize: 0.01
    snapMode: Slider.SnapAlways
    background: Item {
        Rectangle {
            x: leftPadding
            y: topPadding + availableHeight / 2 - height / 2
            width: availableWidth
            height: parent.height * 0.35
            radius: 9999
            color: "#E0E0E0"
            clip: true
            Rectangle {
                width: visualPosition * parent.width
                height: parent.height
                color: "#4B99D2"
                radius: 9999
            }
        }
    }

    handle: Rectangle {
        x: leftPadding + visualPosition * (availableWidth - width)
        height: root.height
        width: root.height
        radius: 9999
        color: "#203A47"
        border.color: "white"
        border.width: pressed ? width * 0.3 : hovered ? width * 0.25 : width * 0.2
    }
}
