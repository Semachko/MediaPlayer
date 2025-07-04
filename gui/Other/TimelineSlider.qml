

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: root
    width: 200
    height: 30
    value: 0

    background: Item {
        Rectangle {
            x: leftPadding
            y: topPadding + availableHeight / 2 - height / 2
            width: availableWidth
            height: parent.height * 0.38
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
        x: leftPadding + visualPosition * (availableWidth - width) - 1
        anchors.verticalCenter: root.verticalCenter
        height: root.height * 0.9
        width: root.height * 0.5
        radius: 5
        color: "#203A47"
        border.color: "white"
        border.width: pressed ? width * 0.3 : hovered ? width * 0.28 : width * 0.25
    }
}
