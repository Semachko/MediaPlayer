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

    function get_time_by_ms(ms)
    {
        var totalSeconds = Math.floor(ms / 1000);
        var hours = Math.floor(totalSeconds / 3600);
        var minutes = Math.floor((totalSeconds % 3600) / 60);
        var seconds = totalSeconds % 60;
        return ("0" + hours).slice(-2) + ":" +
                ("0" + minutes).slice(-2) + ":" +
                ("0" + seconds).slice(-2);
    }

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
    RowLayout{
        id: timelinebar
        height: timeslider.height
        anchors.top: menubar_background.top
        anchors.left: menubar_background.left
        anchors.right: menubar_background.right
        anchors.topMargin: 5
        anchors.leftMargin: 15
        anchors.rightMargin: 15
        Text{
            id: current_time
            Layout.bottomMargin: 3
            text: "00:00:00"
            color: "white"
            font.pointSize: 15
            font.bold: true
            Connections {
                target: media
                function onNewTime(time) {
                    current_time.text = root.get_time_by_ms(time)
                }
            }
        }
        TimelineSlider{
            id: timeslider
            Layout.fillWidth: true
            Layout.fillHeight: true
            value: media.currentPosition
            onPressedChanged:{
                if (pressed) {
                    if (playbutton.checked)
                        media.sliderPause()
                } else {
                    media.timeChanged(position)
                }
            }
        }
        Text{
            id: media_time
            Layout.bottomMargin: 3
            text: "00:00:00"
            color: "white"
            font.pointSize: 15
            font.bold: true
            Connections {
                target: media
                function onGlobalTime(time) {
                    media_time.text = root.get_time_by_ms(time)
                }
            }
        }
    }
    RowLayout{
        id: optionsbar
        anchors.top: timelinebar.bottom
        anchors.bottom: menubar_background.bottom
        anchors.left: menubar_background.left
        anchors.right: mediacontrolsbar.left
        anchors.margins: 5
        anchors.leftMargin: 20
        spacing: 2
        FilenameBar{
            id: filenamebar
            Layout.preferredWidth: parent.width * 0.7
            Layout.preferredHeight: parent.height * 0.55
            onClicked: fileDialog.open()
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
        }
        ToolsMenu{
            id: toolsmenu
            Layout.fillWidth: true
            onFiltersClicked: filterswindow.visible = true
            onEqualizerClicked: equalizerwindow.visible = true
        }

    }
    RowLayout {
        id: mediacontrolsbar
        anchors{
            top: timelinebar.bottom
            bottom: menubar_background.bottom
            horizontalCenter: menubar_background.horizontalCenter
        }
        spacing: 0
        ChangeMediaButton {
            Layout.fillWidth: true
            scale: -0.6
        }
        ChangeTimeButtonLeft {
            Layout.fillWidth: true
            scale: 0.8
        }
        PlayButton {
            id: playbutton
            Layout.fillWidth: true
            scale: 0.7
            onPressed: {
                media.playORpause()
            }
        }
        ChangeTimeButtonRight {
            Layout.fillWidth: true
            scale: 0.8
        }
        ChangeMediaButton {
            Layout.fillWidth: true
            scale: 0.6
        }
    }


    RowLayout{
        id: speed_volume_resize_bar
        anchors.top: timelinebar.bottom
        anchors.bottom: menubar_background.bottom
        anchors.left: mediacontrolsbar.right
        anchors.right: menubar_background.right
        anchors.margins: 5
        anchors.leftMargin: 40
        anchors.rightMargin: 20

        SpeedButton{
            id: speedbutton
            scale: 0.8
            SpeedSlider{
                visible: speedbutton.checked
                anchors.bottom: speedbutton.top
                anchors.bottomMargin: 5
                anchors.horizontalCenter: speedbutton.horizontalCenter
                width: speedbutton.width*0.4
                height: 250
                scale: -1
            }
        }

        MuteButton{
            id: mutebutton
            scale: 0.75
            onPressed: {
                media.muteORunmute()
            }
        }

        VolumeSlider{
            id: volumeslider
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height*0.3
            Layout.leftMargin: -15
            Layout.rightMargin: 12
            onMoved:{
                media.volumeChanged(volumeslider.value)
            }
        }
        ResizeButton{
            id: resizebutton
            scale: 0.7
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

    FiltersWindow{
        id: filterswindow
        visible: false
        anchors.centerIn: backgroundlogo

        onBrightnessChanged: media.changeBrightness(filterswindow.brightness)
        onContrastChanged: media.changeContrast(filterswindow.contrast)
        onSaturationChanged: media.changeSaturation(filterswindow.saturation)
    }
    EqualizerWindow{
        id: equalizerwindow
        visible: false
        anchors.centerIn: backgroundlogo

        onLowChanged: media.changeLowSounds(equalizerwindow.low)
        onMidChanged: media.changeMidSounds(equalizerwindow.mid)
        onHighChanged: media.changeHighSounds(equalizerwindow.high)
    }

}
