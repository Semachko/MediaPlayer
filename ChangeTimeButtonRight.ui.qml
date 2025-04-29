

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Basic

Button {
    id: root
    width: 70
    height: 70
    contentItem: Text {
        text: "5"
        color: pressed ? "black" : "white"
        font.bold: true
        font.pointSize: 24
        elide: Text.ElideLeft
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        bottomPadding: 17
    }
    background: Rectangle {
        id: back
        anchors.fill: parent
        color: pressed ? "#3565A6" : hovered ? "#3565A6" : "#343F4D"
        border.width: 6
        border.color: "#FFFFFF"
        radius: 50
        Rectangle {
            id: rectangle
            x: 38
            y: 49
            width: 12
            height: 4
            color: pressed ? "black" : "white"
            rotation: 20
            radius: 1
        }

        Rectangle {
            id: rectangle1
            x: 38
            y: 53
            width: 12
            height: 4
            color: pressed ? "black" : "white"
            rotation: -20
            radius: 1
        }

        Rectangle {
            id: rectangle2
            x: 24
            y: 51
            width: 27
            height: 4
            color: pressed ? "black" : "white"
            radius: 50
            rotation: 0
        }
    }
}
