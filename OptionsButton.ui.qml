

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
    width: 65
    height: 65
    background: Item {
        Rectangle {
            id: rectangle
            visible: hovered
            anchors.fill: parent
            color: "#5C5D5D"
            radius: 5
            opacity: 0.2
        }
        Image {
            id: optionsBlack
            anchors.centerIn: parent
            source: pressed ? "qrc:/images/OptionsBlack.svg" : "qrc:/images/OptionsWhite.svg"
            fillMode: Image.PreserveAspectFit
        }
    }
}
