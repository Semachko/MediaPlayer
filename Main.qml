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
    property real mouseX: 0
    property real mouseY: 0

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
    Component.onCompleted: {
        player.videoSink = videoOutput.videoSink
    }
    Item {
        id: video
        width: parent.width
        height: parent.height
        transformOrigin: Item.TopLeft
        VideoOutput {
            id: videoOutput
            transformOrigin: Item.Center
            width: parent.width
            height: parent.height
        }
    }

    Item{
        id: controls_menu
        anchors.fill: parent
        Behavior on opacity {
            NumberAnimation {
                duration: 150
                easing.type: Easing.InOutQuad
            }
        }

        MenuBar{
            id: menubar_background
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 140
            opacity: 0.4
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
                    target: player
                    function onNewTime(time) {
                        current_time.text = root.get_time_by_ms(time)
                    }
                }
            }
            TimelineSlider{
                id: timeslider
                Layout.fillWidth: true
                Layout.fillHeight: true
                value: player.currentPosition
                onPressedChanged:{
                    if (pressed) {
                        if (playbutton.checked){
                            player.sliderPause()
                        }
                    } else {
                        player.timeChanged(position)
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
                    target: player
                    function onGlobalTime(time) {
                        media_time.text = root.get_time_by_ms(time)
                    }
                }
            }
        }
        RowLayout{
            id: optionsbar
            anchors{
                top: timelinebar.bottom
                bottom: menubar_background.bottom
                left: menubar_background.left
                right: mediacontrolsbar.left
                margins: 5
                leftMargin: 20
            }
            spacing: 2
            FilenameBar{
                id: filenamebar
                text: player.filename
                Layout.preferredWidth: parent.width * 0.7
                Layout.preferredHeight: parent.height * 0.55
                onClicked: fileDialog.open()
                Connections {
                    target: player
                    onFileNameChanged: (filename) => {
                        filenamebar.text = filename
                    }
                }
                FileDialog {
                    id: fileDialog
                    title: "CHOOSE FILE"
                    nameFilters: [
                        "Media (*.mp4 *.mkv *.avi *.mov *.flv *.webm *.ts *.mpeg *.mpg *.3gp *.m4v *.wmv *.mp3 *.aac *.wav *.flac *.ogg *.opus *.wma *.alac *.ac3 *.dts)",
                        "Video (*.mp4 *.mkv *.avi *.mov *.flv *.webm *.ts *.mpeg *.mpg *.3gp *.m4v *.wmv)",
                        "Audio (*.mp3 *.aac *.wav *.flac *.ogg *.opus *.wma *.alac *.ac3 *.dts)"
                    ]
                    onAccepted: {
                        console.log("Selected file:", fileDialog.selectedFile)
                        //filenamebar.text = fileDialog.selectedFile.toString().split("/").pop().split(".")[0]
                        player.setFile(fileDialog.selectedFile,playbutton.checked)
                    }
                }
                Shortcut {
                    sequence: "C"
                    onActivated: {filenamebar.click();
                    console.log("text: ",filenamebar.text)}
                }
            }
            ToolsMenu{
                id: toolsmenu
                Layout.fillWidth: true
                onFiltersClicked: filterswindow.visible = true
                onEqualizerClicked: equalizerwindow.visible = true
                onRotateClicked: videoOutput.rotation += 90
                onShuffleClicked: player.shuffleMedia(playbutton.checked)
                onRepeatClicked: player.isRepeating = toolsmenu.isRepeating
                Shortcut {
                    sequence: "E"
                    onActivated: equalizerwindow.visible = !equalizerwindow.visible
                }
                Shortcut {
                    sequence: "V"
                    onActivated: filterswindow.visible = !filterswindow.visible
                }
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
                id: changemediabtnleft
                Layout.fillWidth: true
                scale: -0.6
                onClicked: player.prevMedia(playbutton.checked)
                Shortcut {
                    sequence: "ctrl+left"
                    onActivated: changemediabtnleft.click()
                }
                HelpTip{
                    x: 157
                    y: 148
                    visible: parent.hovered
                    text: "Previous file (Ctrl + ←)"
                }
            }
            ChangeTimeButtonLeft {
                id: changetimebtnleft
                Layout.fillWidth: true
                scale: 0.8
                onClicked: player.subtruct5sec()
                Shortcut {
                    sequence: "left"
                    onActivated: changetimebtnleft.click()
                }
                HelpTip{
                    x: -12
                    y: -50
                    visible: parent.hovered
                    text: "-5 sec (←)"
                }
            }
            PlayButton {
                id: playbutton
                Layout.fillWidth: true
                scale: 0.7
                onCheckedChanged: {
                    player.isPaused = !playbutton.checked
                }
                Connections {
                    target: player
                    onPaused: playbutton.click()
                }
                Shortcut {
                    sequence: "space"
                    onActivated: playbutton.click()
                }
                HelpTip{
                    x: -28
                    y: -60
                    visible: parent.hovered
                    text: parent.checked ? "Pause (SPACE)" : "Play    (SPACE)"
                }

            }
            ChangeTimeButtonRight {
                id: changetimebtnright
                Layout.fillWidth: true
                scale: 0.8
                onClicked: player.add5sec()
                Shortcut {
                    sequence: "right"
                    onActivated: changetimebtnright.click()
                }
                HelpTip{
                    x: -12
                    y: -50
                    visible: parent.hovered
                    text: "+5 sec (→)"
                }

            }
            ChangeMediaButton {
                id: changemediabtnright
                Layout.fillWidth: true
                scale: 0.6
                onClicked: player.nextMedia(playbutton.checked)
                Shortcut {
                    sequence: "ctrl+right"
                    onActivated: changemediabtnright.click()
                }
                HelpTip{
                    x: -65
                    y: -75
                    visible: parent.hovered
                    text: "Next file (Ctrl + →)"
                }
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
                    id: speedslider
                    visible: speedbutton.checked
                    anchors.bottom: speedbutton.top
                    anchors.bottomMargin: 5
                    anchors.horizontalCenter: speedbutton.horizontalCenter
                    width: speedbutton.width*0.4
                    height: 230
                    onMoved:{
                        player.speed = speedslider.value
                    }
                }
                HelpTip{
                    x: -30
                    y: -50
                    visible: parent.hovered
                    text: "Speed (Shift + ↕ )"
                }
            }

            MuteButton{
                id: mutebutton
                scale: 0.75
                onClicked: {
                    player.isMuted = mutebutton.checked
                }
                Shortcut {
                    sequence: "M"
                    onActivated: mutebutton.click()
                }
                HelpTip{
                    x: -12
                    y: -50
                    visible: parent.hovered
                    text: mutebutton.checked ? "Unmute (M)" : "Mute     (M)"
                }
            }

            VolumeSlider{
                id: volumeslider
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height*0.3
                Layout.leftMargin: -15
                Layout.rightMargin: 12
                onMoved:{
                    player.volume = volumeslider.value
                }
                HelpTip{
                    x: parent.x - 160
                    y: parent.y - 70
                    visible: parent.hovered
                    text: "Volume ( ↕ )"
                }
            }
            ResizeButton{
                id: resizebutton
                scale: 0.7
                property int prevWidth: 900
                property int prevHeight: 600
                property bool isMaximized: false
                onClicked: {
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
                Shortcut {
                    sequence: "F"
                    onActivated: resizebutton.click()
                }
                Shortcut {
                    sequence: "ESC"
                    onActivated: if (root.visibility == Window.FullScreen) resizebutton.click()
                }
                HelpTip{
                    x: resizebutton.checked ? parent.x - 650 : parent.x - 400
                    y: parent.y - 60
                    visible: parent.hovered
                    text: resizebutton.checked ? "Exit fullscreen (F)" : "Full screen      (F)"
                }
            }
        }

        FiltersWindow{
            id: filterswindow
            visible: false
            anchors.centerIn: parent
            onBrightnessChanged: player.brightness = filterswindow.brightness
            onContrastChanged: player.contrast = filterswindow.contrast
            onSaturationChanged: player.saturation = filterswindow.saturation
        }
        EqualizerWindow{
            id: equalizerwindow
            visible: false
            anchors.centerIn: parent
            onLowChanged: player.low = equalizerwindow.low
            onMidChanged: player.mid = equalizerwindow.mid
            onHighChanged: player.high = equalizerwindow.high
        }
    }



    Timer {
        id: hideControlsTimer
        interval: 3500
        repeat: false
        onTriggered: {
            if (controls_menu.opacity === 1.0){
                controls_menu.opacity = 0.0
            }
        }
    }
    HoverHandler {
        id: hoverHandler
        onPointChanged: {
            if (point.position.x !== mouseX || point.position.y !== mouseY){
                if (controls_menu.opacity === 0.0)
                    controls_menu.opacity = 1.0;
                hideControlsTimer.restart();
            }
            mouseX = point.position.x
            mouseY = point.position.y
        }
    }
    WheelHandler {
        onWheel: (wheel)=> {
            if (wheel.modifiers & Qt.ControlModifier)
            {
                const oldZoom = video.scale;
                const scale = wheel.angleDelta.y > 0 ? 0.1 : -0.1;
                const newZoom = oldZoom + scale
                if (newZoom < 1) {
                    video.x = 0;
                    video.y = 0;
                    video.scale = 1.0
                    return;
                }
                video.scale = Math.min(newZoom, 5.0);
                const scaling = video.scale / oldZoom - 1;

                const dx = mouseX - video.x;
                const dy = mouseY - video.y;
                video.x -= dx * scaling;
                video.y -= dy * scaling;
            }
            else if (wheel.modifiers & Qt.ShiftModifier)
            {
                if (wheel.angleDelta.y > 0)
                    speedslider.increase();
                else
                    speedslider.decrease();
                speedslider.moved()
            }
            else
            {
                if (wheel.angleDelta.y > 0)
                    volumeslider.increase();
                else
                    volumeslider.decrease();
                volumeslider.moved()
            }
        }
    }
}
