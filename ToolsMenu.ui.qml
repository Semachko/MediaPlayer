import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root
    OptionsButton {
        id: optionsbutton
        onClicked: {
            if(popup.closed){
                popup.x = x
                popup.y = y - popup.height - 60
                popup.open()
            }
            else{
                popup.close()
            }
        }
    }

    signal filtersClicked()
    signal equalizerClicked()
    signal shuffleClicked()
    signal repeatClicked()
    Popup {
        id: popup
        width: 230
        height: 200
        modal: false
        focus: true
        background: Rectangle {
            id: background
            anchors.fill: parent
            color: "black"
            radius: 10
            opacity: 0.5
        }
        contentItem: Column {
            anchors.fill: parent
            ToolsMenuButton {
                id: filters
                label: "Filters"
                iconSRC: "qrc:/images/filters.svg"
                onClicked: {
                    popup.close()
                    root.onFiltersClicked();
                }
            }
            ToolsMenuButton {
                id: equalizer
                label: "Equalizer"
                iconSRC: "qrc:/images/equalizer.svg"
                onClicked: {
                    popup.close()
                    onEqualizerClicked();
                }
            }
            ToolsMenuButton {
                id: shuffle
                label: "Shuffle"
                iconSRC: "qrc:/images/shuffle.svg"
                onClicked: {
                    popup.close()
                    onShuffleClicked();
                }
            }
            ToolsMenuButton {
                id: repeat
                label: "Repeat"
                iconSRC: "qrc:/images/repeat.svg"
                onClicked: {
                    popup.close()
                    onRepeatClicked();
                }
            }
        }
    }
}
