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
        height: parent.height * 0.65
        spacing: 15

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            EqualizerSlider {
                id: low_slider
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                value: 0.5
            }
            Text {
                Layout.fillWidth: true
                text: "low"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            EqualizerSlider {
                id: mid_slider
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                value: 0.5
            }
            Text {
                Layout.fillWidth: true
                text: "mid"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            EqualizerSlider {
                id: high_slider
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                value: 0.5
            }
            Text {
                Layout.fillWidth: true
                text: "high"
                color: "white"
                font.pointSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
    RowLayout {
        anchors {
            top: settings.bottom
            bottom: root.bottom
            left: root.left
            right: root.right
            topMargin: 30
            bottomMargin: 10
            leftMargin: 20
            rightMargin: 20
        }

        spacing: 20

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
                low_slider.value = 0.5
                mid_slider.value = 0.5
                high_slider.value = 0.5
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
