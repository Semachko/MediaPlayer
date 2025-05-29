import QtQuick
import QtQuick.Controls.Basic
import Qt.labs.platform 1.1

Button {
    id: root
    width: 100
    height: 50
    text: qsTr("CHOOSE FILE")
    contentItem: Item {
        anchors.fill: parent
        Text {
            anchors.fill: parent
            anchors.leftMargin: 25
            anchors.bottomMargin: 5
            font.bold: true
            text: root.text
            font.pointSize: 16
            color: "white"
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    background: Rectangle {
        color: hovered ? "#535353" : "#2B2B2B"
        opacity: 0.5
        border.width: pressed ? 3 : 0
        border.color: "white"
        radius: 10
    }
}
