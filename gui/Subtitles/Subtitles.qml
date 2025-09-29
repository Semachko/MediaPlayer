import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property alias model: combo.model
    property alias currentIndex: combo.currentIndex
    height: parent.height * 0.5
    width: parent.height * 0.6
    Rectangle{
        id: background
        color: "black"
        anchors.fill: parent
        radius: 25
        opacity: 0.5
    }
    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        Text{
           Layout.alignment: Qt.AlignHCenter
           color: "white"
            text: "Subtitles"
            font.bold: true
            font.pixelSize: parent.height * 0.1
        }
        SubComboBox{
            id: combo
            Layout.preferredHeight: parent.height*0.13
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        Item { Layout.fillHeight: true }
        Button {
            id: closebutton
            implicitHeight: root.height * 0.15
            implicitWidth: root.width * 0.5
            Layout.alignment: Qt.AlignHCenter
            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                text: "Close"
                color: "white"
                font.bold: true
                font.pixelSize: parent.height * 0.5
            }
            background: Rectangle {
                anchors.fill: closebutton
                color: closebutton.hovered ? "#535353" : "#3A3A3A"
                border.width: closebutton.pressed ? 3 : 0
                border.color: "white"
                radius: 55
                opacity: 0.5
            }
            onClicked: root.visible = false
        }
    }
}
