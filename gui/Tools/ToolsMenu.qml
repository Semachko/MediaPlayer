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
                label: "Filters (V)"
                iconSRC: "qrc:/resources/icons/filters.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.filtersClicked()
                }
            }
            ToolsMenuButton {
                id: equalizer
                label: "Equalizer (E)"
                iconSRC: "qrc:/resources/icons/equalizer.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.equalizerClicked()
                }
            }
            ToolsMenuButton {
                id: rotate
                label: "Rotate (R)"
                iconSRC: "qrc:/resources/icons/rotate.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.rotateClicked()
                }
                Shortcut {
                    sequence: "R"
                    onActivated: rotate.click()
                }
            }
            ToolsMenuButton {
                id: shuffle
                label: "Shuffle (S)"
                iconSRC: "qrc:/resources/icons/shuffle.svg"
                checkable: false
                onClicked: {
                    popup.close()
                    root.shuffleClicked()
                }
                Shortcut {
                    sequence: "S"
                    onActivated: shuffle.click()
                }
            }
            ToolsMenuButton {
                id: repeat
                label: "Repeat (T)"
                iconSRC: "qrc:/resources/icons/repeat.svg"
                onClicked: {
                    popup.close()
                    isRepeating = repeat.checked
                    root.repeatClicked()
                }
                Shortcut {
                    sequence: "T"
                    onActivated: repeat.click()
                }
            }
        }
    }
    onEnabledChanged: enabled ? opacity = 1 : opacity = 0.3
}
