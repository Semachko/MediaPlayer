

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
    implicitWidth: 65
    implicitHeight: 65
    checkable: true
    background: Item {
        Image {
            visible: !root.checked
            anchors.centerIn: parent
            source: pressed ? "qrc:/images/MediaSizeButtonPressed.svg" : hovered ? "qrc:/images/MediaSizeButtonHovered.svg" : "qrc:/images/MediaSizeButton.svg"
            fillMode: Image.PreserveAspectFit
        }
        Image {
            visible: root.checked
            anchors.centerIn: parent
            source: pressed ? "qrc:/images/MediaSizeButtonCheckedPressed.svg" : hovered ? "qrc:/images/MediaSizeButtonCheckedHovered.svg" : "qrc:/images/MediaSizeButtonChecked.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
