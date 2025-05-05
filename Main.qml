import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtMultimedia
import MediaPlayer
import QtQuick.Dialogs

Window {
    id: root
    width: 1280
    height: 720
    minimumWidth: 900
    minimumHeight: 600
    color: "#202020"
    visible: true
    title: "MediaPlayer"


    Image {
        id: backgroundlogo
        anchors.centerIn: parent
        scale: 2.5
        source: "qrc:/images/BackGroundLogo.svg"
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
    }

    MenuBar{
        id: menubar_background
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 140
        opacity: 0.2
    }

    Row{
        id: timelinebar
        height: timeslider.height
        anchors.top: menubar_background.top
        anchors.left: menubar_background.left
        anchors.right: menubar_background.right
        anchors.topMargin: 5
        Text{
            id: current_time
            text: "00:00:00"
            color: "white"
            font.pointSize: 15
            font.bold: true
            anchors.left: parent.left
            anchors.leftMargin: 15
        }
        TimelineSlider{
            id: timeslider
            anchors.left: current_time.right
            anchors.right: media_time.left
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            onPressedChanged:{
                if (pressed) {
                    media.playORpause()
                } else {
                    media.timeChanged(position)
                    media.playORpause()
                }
            }
        }
        Text{
            id: media_time
            text: "00:00:00"
            color: "white"
            font.pointSize: 15
            font.bold: true
            anchors.right: parent.right
            anchors.rightMargin: 15
        }
    }

    Row{
        id: mediacontrolsbar
        anchors.horizontalCenter: menubar_background.horizontalCenter
        anchors.top: timelinebar.bottom
        anchors.bottom: menubar_background.bottom
        spacing: 3
        ChangeMediaButton{scale: -0.6
        anchors.verticalCenter: parent.verticalCenter}
        ChangeTimeButtonLeft{scale: 0.8
        anchors.verticalCenter: parent.verticalCenter}
        PlayButton{scale: 0.8
            anchors.verticalCenter: parent.verticalCenter
            onPressed: {
                media.playORpause()
            }
        }
        ChangeTimeButtonRight{scale: 0.8
        anchors.verticalCenter: parent.verticalCenter}
        ChangeMediaButton{scale: 0.6
        anchors.verticalCenter: parent.verticalCenter}
    }

    Row{
        id: volumebar
        anchors.top: timelinebar.bottom
        anchors.bottom: menubar_background.bottom
        anchors.right: menubar_background.right
        anchors.rightMargin: 10
        anchors.left: mediacontrolsbar.right
        anchors.leftMargin: (root.width-volumebar.x)*0.2
        spacing: 1
        MuteButton{
            id: mutebutton
            anchors.verticalCenter: parent.verticalCenter
            scale: 0.8
            onPressed: {
                media.muteORunmute()
            }
        }

        VolumeSlider{
            id: volumeslider
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: mutebutton.right
            anchors.leftMargin: -10
            anchors.right: resizebutton.left
            anchors.rightMargin: 20
            onMoved:{
                media.volumeChanged(volumeslider.value)
            }
        }

        ResizeButton{
            id: resizebutton
            scale: 0.7
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            property int prevWidth: 900
            property int prevHeight: 600
            property bool isMaximized: false
            onPressed: {
                checked=!checked
                if (root.visibility == Window.FullScreen){
                    if (isMaximized)
                        root.showMaximized()
                    else{
                        root.showNormal()
                        root.width = prevWidth
                        root.height = prevHeight
                    }
                }
                else{
                    if (root.visibility == Window.Maximized)
                        isMaximized = true
                    else
                        isMaximized = false
                    prevWidth = root.width
                    prevHeight = root.height
                    root.showFullScreen()
                }
            }
        }
    }
    FileDialog {
        id: fileDialog
        title: "Choose file"
        nameFilters: ["All (*)"]
        onAccepted: {
            console.log("Selected file:", fileDialog.selectedFile)
            filenamebar.text = fileDialog.selectedFile.toString().split("/").pop().split(".")[0]

            media.videoSink = videoOutput.videoSink
            media.setFile(fileDialog.selectedFile)
        }
    }
    FilenameBar{
        id: filenamebar
        onClicked: fileDialog.open()
        anchors.left: menubar_background.left
        anchors.leftMargin: 30
        anchors.right: toolsmenu.left
        anchors.rightMargin: 25
        anchors.top: timelinebar.bottom
        anchors.topMargin: 22
        anchors.bottom: menubar_background.bottom
        anchors.bottomMargin: 22
        anchors.verticalCenter: menubar_background.verticalCenter
    }

    ToolsMenu{
        id: toolsmenu
        anchors.right: mediacontrolsbar.left
        anchors.rightMargin: 85
        y: menubar_background.y+55
    }
}
