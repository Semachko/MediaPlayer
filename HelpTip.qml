import QtQuick
import QtQuick.Controls.Basic

ToolTip {
    id: control
    delay: 200
    contentItem: Text {
        text: control.text
        font.family: "Tahoma"
        font.pointSize: 10
        color: "white"
    }

    background: Rectangle {
        color: "#242424"
        radius: 5
    }
}
