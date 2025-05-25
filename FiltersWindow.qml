import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
    ColumnLayout {
        id: settings
        anchors{
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 30
            leftMargin: 30
            rightMargin: 30
        }
        spacing: 10
        height: parent.height*0.6

        RowLayout {
            Layout.fillWidth: true
            spacing: 30
            Item {
                width: 100
                height: brightness_label.implicitHeight
                Text {
                    id: brightness_label
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "brightness"
                    color: "white"
                    font.bold: true
                    font.pointSize: 16
                }
            }
            FilterSlider {
                id: brightness_slider
                Layout.fillWidth: true
                value: 0.5
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 30
            Item {
                width: 100
                height: contrast_label.implicitHeight
                Text {
                    id: contrast_label
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "contrast"
                    color: "white"
                    font.bold: true
                    font.pointSize: 16
                }
            }
            FilterSlider {
                id: contrast_slider
                Layout.fillWidth: true
                value: 0.5
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 30
            Item {
                width: 100
                height: saturation_label.implicitHeight
                Text {
                    id: saturation_label
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "saturation"
                    color: "white"
                    font.bold: true
                    font.pointSize: 16
                }
            }
            FilterSlider {
                id: saturation_slider
                Layout.fillWidth: true
                value: 0.5
            }
        }
    }
    RowLayout{
        anchors{
            top: settings.bottom
            bottom: root.bottom
            left: root.left
            right: root.right
            margins: 20
        }
        Button {
            id: resetbutton
            Layout.preferredWidth: root.width * 0.3
            Layout.preferredHeight: parent.height * 0.7

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
        Item { Layout.fillWidth: true }
        Button {
            id: closebutton
            Layout.preferredWidth: root.width * 0.3
            Layout.preferredHeight: parent.height * 0.7

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
