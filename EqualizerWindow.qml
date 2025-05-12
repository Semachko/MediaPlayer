import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item{
    id: root
    height: parent.height * 0.5
    width: parent.height * 0.4
    property real low: low_slider.value
    property real mid: mid_slider.value
    property real high: high_slider.value

    Rectangle{
        id: background
        color: "black"
        anchors.fill: parent
        radius: 25
        opacity: 0.5
    }
    RowLayout {
        id: settings
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            topMargin: 30
            leftMargin: 30
            rightMargin: 30
        }
        spacing: 30
        height: parent.height * 0.6

        Column {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Slider {
                id: low_slider
                orientation: Qt.Vertical
                value: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "low"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Slider {
                id: mid_slider
                orientation: Qt.Vertical
                value: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "mid"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Slider {
                id: high_slider
                orientation: Qt.Vertical
                value: 0.5
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: "high"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
    Row{
        anchors{
            top: settings.bottom
            bottom: root.bottom
            left: root.left
            right: root.right
            leftMargin: 20
            rightMargin: 20
            topMargin: 30
            bottomMargin: 10
        }
        Button {
            id: resetbutton
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: root.width*0.3
            height: parent.height*0.7

            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                text: "Reset"
                color: "white"
                font.bold: true
                font.pointSize: 14
            }
            background: Rectangle {
                color: resetbutton.hovered ? "#535353" : "#3A3A3A"
                border.width: resetbutton.pressed ? 3 : 0
                border.color: "white"
                radius: 55
                opacity: 0.5
            }
            onClicked: {
                low_slider.value = 0.5;
                mid_slider.value = 0.5;
                high_slider.value = 0.5;
            }
        }
        Button {
            id: closebutton
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: root.width*0.3
            height: parent.height*0.7

            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                bottomPadding: 5
                text: "Close"
                color: "white"
                font.bold: true
                font.pointSize: 14
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
