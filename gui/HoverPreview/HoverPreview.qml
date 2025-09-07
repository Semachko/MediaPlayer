import QtQuick
import QtMultimedia


Item{
    id: root
    property string time: "00:00:00"
    readonly property var videosink: video.videoSink
    implicitWidth: time_background.width
    implicitHeight: time_background.height
    Rectangle {
        id: time_background
        width: 80
        height: 30
        color: "black"
        opacity: 0.5
        radius: 10
    }
    Text{
        id: time_text
        text: root.time
        font.bold: true
        font.pointSize: 12
        verticalAlignment: Text.AlignVCenter
        anchors.centerIn: time_background
        color: "white"
    }
    VideoOutput{
        id: video
        width: 300
        height: 200
        anchors.bottom: time_background.top
        anchors.horizontalCenter: time_background.horizontalCenter
    }
}

