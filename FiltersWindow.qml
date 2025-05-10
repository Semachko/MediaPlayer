import QtQuick
import QtQuick.Controls
Item{
    id: root
    height: parent.height * 0.5
    width: parent.height * 0.6
    property real brightness: brightness_slider.value
    property real contrast: contrast_slider.value
    property real saturation: saturation_slider.value

    Rectangle{
        id: background
        color: "black"
        anchors.fill: parent
        radius: 25
        opacity: 0.5
    }
    Column {
        id: settings
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 30
            leftMargin: 30
            rightMargin: 30
        }
        spacing: 0
        height: parent.height*0.6

        Row {
            width: parent.width
            height: parent.height*0.3
            Text {
                id: brightness_label
                text: "brightness"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                id: brightness_slider
                width: parent.width*0.55
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                value: 0.5
            }
        }
        Row {
            width: parent.width
            height: parent.height*0.3
            Text {
                id: contrast_label
                text: "contrast"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                id: contrast_slider
                width: parent.width*0.55
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                value: 0.5
            }
        }
        Row {
            width: parent.width
            height: parent.height*0.3
            Text {
                id: saturation_label
                text: "saturation"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                id: saturation_slider
                width: parent.width*0.55
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                value: 0.5
            }
        }
    }
    Row{
        anchors{
            top: settings.bottom
            bottom: root.bottom
            left: root.left
            right: root.right
            margins: 20
        }
        Button {
            id: resetbutton
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: root.width*0.3
            height: parent.height*0.7

            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                text: "Reset"
                color: "white"
                font.bold: true
                font.pointSize: 16
            }
            background: Rectangle {
                color: resetbutton.hovered ? "#535353" : "#3A3A3A"
                border.width: resetbutton.pressed ? 3 : 0
                border.color: "white"
                radius: 55
                opacity: 0.5
            }
            onClicked: {
                brightness_slider.value = 0.5;
                contrast_slider.value = 0.5;
                saturation_slider.value = 0.5;
            }
        }
        Button {
            id: closebutton
            anchors.right: parent.right
            anchors.rightMargin: 10
            width: root.width*0.3
            height: parent.height*0.7

            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                text: "Close"
                color: "white"
                font.bold: true
                font.pointSize: 16
            }
            background: Rectangle {
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
