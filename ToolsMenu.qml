import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    id: root
    OptionsButton {
        id: optionsbutton
        anchors.centerIn: root
        onClicked: {
            if (popup.closed) {
                popup.x = x
                popup.y = y - popup.height - 60
                popup.open()
            } else {
                popup.close()
            }
        }
    }
    signal filtersClicked
    signal equalizerClicked
    signal rotateClicked
    signal shuffleClicked
    signal repeatClicked
    property bool isRepeating: false
    Popup {
        id: popup
        modal: false
        focus: true
        background: Rectangle {
            id: background
            anchors.fill: elements
            color: "black"
            radius: 10
            opacity: 0.5
        }
        contentItem: ColumnLayout {
            id: elements
            spacing: 0
            //Layout.fillHeight: true
            ToolsMenuButton {
                id: filters
                label: "Filters"
                iconSRC: "qrc:/images/filters.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.filtersClicked()
                }
            }
            ToolsMenuButton {
                id: equalizer
                label: "Equalizer"
                iconSRC: "qrc:/images/equalizer.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.equalizerClicked()
                }
            }
            ToolsMenuButton {
                id: rotate
                label: "Rotate"
                iconSRC: "qrc:/images/rotate.svg"
                checkable: false
                onClicked: {
                    console.log("ROT CLICKED!!!");
                    popup.close()
                    root.rotateClicked()
                }
            }
            ToolsMenuButton {
                id: shuffle
                label: "Shuffle"
                iconSRC: "qrc:/images/shuffle.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.shuffleClicked()
                }
            }
            ToolsMenuButton {
                id: repeat
                label: "Repeat"
                iconSRC: "qrc:/images/repeat.svg"
                onClicked: {
                    popup.close()
                    isRepeating = !isRepeating
                    root.repeatClicked()
                }
            }
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
