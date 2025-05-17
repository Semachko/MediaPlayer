import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: root
    width: 28
    height: 200
    orientation: Qt.Vertical
    from: 2.0
    to: 0.5
    stepSize: 0.25
    value: 1.0
    snapMode: Slider.SnapAlways
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
                width: parent.width
                height: visualPosition * parent.height
                color: "#66EAB1"
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
        color: "#284331"
        border.color: "white"
        border.width: pressed ? height * 0.3 : hovered ? height * 0.25 : height * 0.2
    }
}
