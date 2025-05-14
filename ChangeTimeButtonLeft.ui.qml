
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
    implicitWidth: 70
    implicitHeight: 70
    contentItem: Text {
        text: "5"
        color: "white"
        font.bold: true
        font.pointSize: 24
        elide: Text.ElideLeft
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        bottomPadding: 17
    }
    background: Item {
        Rectangle {
            id: background
            anchors.fill: parent
            color: pressed ? "black" : "#252525"
            opacity: hovered ? 1 : 0.5
            radius: 50
        }
        Rectangle {
            id: rect_border
            anchors.fill: parent
            color: "transparent"
            border.color: "white"
            border.width: 5
            radius: 50
        }

        Rectangle {
            id: rectangle
            x: 23
            y: 49
            width: 12
            height: 4
            color: "white"
            rotation: -20
            radius: 1
        }

        Rectangle {
            id: rectangle1
            x: 23
            y: 53
            width: 12
            height: 4
            color: "white"
            rotation: 20
            radius: 1
        }

        Rectangle {
            id: rectangle2
            x: 24
            y: 51
            width: 27
            height: 4
            color: "white"
            radius: 50
            rotation: 0
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
