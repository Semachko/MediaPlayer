

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Basic

// Rectangle {
//     id: popup
//     width: 200
//     height: 200
//     color: "black"
// }
Item {
    OptionsButton {
        onClicked: {
            popup.x = x
            popup.y = y - popup.height - 60
            popup.open()
        }
    }

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
            Button {
                id: filters
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50
                contentItem: Text {
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 50
                    text: "Filters"
                    color: "white"
                    font.pointSize: 18
                }
                background: Rectangle {
                    Image {
                        scale: 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        source: "qrc:/images/filters.svg"
                        fillMode: Image.PreserveAspectFit
                    }
                    radius: 5
                    color: filters.hovered ? "#5C5D5D" : "transparent"
                    border.width: filters.pressed ? 3 : 0
                    border.color: "white"
                }
            }
            Button {
                id: equalizer
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50
                contentItem: Text {
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 50
                    text: "Equalizer"
                    color: "white"
                    font.pointSize: 18
                }
                background: Rectangle {
                    Image {
                        scale: 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        source: "qrc:/images/equalizer.svg"
                        fillMode: Image.PreserveAspectFit
                    }
                    radius: 5
                    color: equalizer.hovered ? "#5C5D5D" : "transparent"
                    border.width: equalizer.pressed ? 3 : 0
                    border.color: "white"
                }
            }
            Button {
                id: shuffle
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50
                contentItem: Text {
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 50
                    text: "Shuffle"
                    color: "white"
                    font.pointSize: 18
                }
                background: Rectangle {
                    Image {
                        scale: 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        source: "qrc:/images/shuffle.svg"
                        fillMode: Image.PreserveAspectFit
                    }
                    radius: 5
                    color: shuffle.hovered ? "#5C5D5D" : "transparent"
                    border.width: shuffle.pressed ? 3 : 0
                    border.color: "white"
                }
            }
            Button {
                id: repeat
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50
                contentItem: Text {
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 50
                    text: "Repeat"
                    color: "white"
                    font.pointSize: 18
                }
                background: Rectangle {
                    Image {
                        scale: 0.8
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        source: "qrc:/images/repeat.svg"
                        fillMode: Image.PreserveAspectFit
                    }
                    radius: 5
                    color: repeat.hovered ? "#5C5D5D" : "transparent"
                    border.width: repeat.pressed ? 3 : 0
                    border.color: "white"
                }
            }
        }
    }
}
