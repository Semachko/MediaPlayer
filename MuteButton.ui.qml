

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
