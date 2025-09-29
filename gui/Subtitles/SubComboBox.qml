import QtQuick
import QtQuick.Controls.Basic


ComboBox {
    id: control
    indicator: Item {}
    model: []
    delegate: ItemDelegate {
        id: delegate
        width: control.width
        height: control.height
        required property var model
        required property int index
        contentItem: Text {
            text: delegate.model[control.textRole]
            color: "white"
            font.bold: true
            font.pixelSize: delegate.height * 0.4
            leftPadding: delegate.height * 0.5
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: delegate.highlighted ? "#313131" : "#212121"
            radius: delegate.height * 0.25
        }
        highlighted: control.highlightedIndex === index
    }

    contentItem: Text {
        text: control.displayText
        color: "white"
        font.bold: true
        font.pixelSize: height * 0.5
        leftPadding: control.height * 0.5
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
    background: Rectangle {
        color: "#212121"
        radius: control.height * 0.25
    }
    popup: Popup {
        y: control.height
        width: control.width
        height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        padding: 0
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
            ScrollIndicator.vertical: ScrollIndicator { }
        }
        background: Rectangle {
            color: "#212121"
            radius: control.height * 0.25
        }
    }
}
