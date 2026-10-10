import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    width: 1100
    height: 750
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: "Zenith Player Deluxe"
    color: "#060709"
    flags: Qt.Window | Qt.FramelessWindowHint

    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 34
        color: "#0a0c10"

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: "ZENITH PLAYER DELUXE"
            color: "#64748b"
            font.pixelSize: 11
            font.bold: true
            font.letterSpacing: 1.5
        }

        Row {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Button {
                width: 44
                height: parent.height
                text: "—"
                flat: true
                onClicked: root.showMinimized()
            }

            Button {
                width: 44
                height: parent.height
                text: "✕"
                flat: true
                onClicked: Qt.quit()
            }
        }

        DragHandler {
            target: null
            onActiveChanged: if (active) root.startSystemMove()
        }
    }

    Item {
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Text {
            anchors.centerIn: parent
            text: "STAGE 1: ENGINE READY"
            color: "#8b5cf6"
            font.pixelSize: 14
            font.bold: true
            font.letterSpacing: 2.0
        }
    }
}