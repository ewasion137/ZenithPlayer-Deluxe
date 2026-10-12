import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    width: 1100
    height: 720
    minimumWidth: 850
    minimumHeight: 550
    visible: true
    title: "Zenith Player Deluxe"
    color: "#060709"
    flags: Qt.Window | Qt.FramelessWindowHint

    readonly property color cBgMain: "#060709"
    readonly property color cPanelBg: "#0d0f17"
    readonly property color cSurface: "#151824"
    readonly property color cAccent: "#8b5cf6"
    readonly property color cTextMain: "#f8fafc"
    readonly property color cTextDim: "#64748b"
    readonly property color cBorder: "#1e2230"

    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 34
        color: "#0a0c10"
        z: 100

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            text: "ZENITH PLAYER DELUXE"
            color: root.cTextDim
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
                text: "-"
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                background: Rectangle { color: parent.hovered ? "#1f2433" : "transparent" }
                onClicked: root.showMinimized()
            }

            Button {
                width: 44
                height: parent.height
                text: "✕"
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                }
                background: Rectangle { color: parent.hovered ? "#ef4444" : "transparent" }
                onClicked: Qt.quit()
            }
        }

        DragHandler {
            target: null
            onActiveChanged: if (active) root.startSystemMove()
        }
    }

    Item {
        id: workspace
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: statusBar.top

        Item {
            id: playerStage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: sidebar.left
            anchors.margins: 20

            Column {
                anchors.fill: parent
                spacing: 16

                Rectangle {
                    width: parent.width
                    height: 110
                    radius: 12
                    color: root.cPanelBg
                    border.color: root.cBorder

                    Row {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 16

                        Rectangle {
                            width: 78
                            height: 78
                            radius: 8
                            color: root.cSurface
                            border.color: root.cBorder

                            Text {
                                anchors.centerIn: parent
                                text: "♪"
                                color: root.cAccent
                                font.pixelSize: 28
                            }
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 4
                            width: parent.width - 100

                            Text {
                                text: library.currentTrackName
                                color: root.cTextMain
                                font.pixelSize: 18
                                font.bold: true
                                elide: Text.ElideRight
                                width: parent.width
                            }

                            Text {
                                text: library.currentTrackPath === "" ? "Ready to play" : library.currentTrackPath
                                color: root.cTextDim
                                font.pixelSize: 11
                                elide: Text.ElideMiddle
                                width: parent.width
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 80
                    radius: 12
                    color: root.cPanelBg
                    border.color: root.cBorder

                    Row {
                        anchors.centerIn: parent
                        spacing: 16

                        Button {
                            width: 38
                            height: 38
                            text: "⏮"
                            contentItem: Text {
                                text: parent.text
                                color: root.cTextMain
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 14
                            }
                            background: Rectangle {
                                radius: 19
                                color: parent.hovered ? "#23283b" : root.cSurface
                                border.color: root.cBorder
                            }
                        }

                        Button {
                            width: 48
                            height: 48
                            text: audioEngine.isPlaying ? "❚❚" : "▶"
                            contentItem: Text {
                                text: parent.text
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 16
                                font.bold: true
                            }
                            background: Rectangle {
                                radius: 24
                                color: root.cAccent
                            }
                            onClicked: audioEngine.togglePlayPause()
                        }

                        Button {
                            width: 38
                            height: 38
                            text: "⏭"
                            contentItem: Text {
                                text: parent.text
                                color: root.cTextMain
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pixelSize: 14
                            }
                            background: Rectangle {
                                radius: 19
                                color: parent.hovered ? "#23283b" : root.cSurface
                                border.color: root.cBorder
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: sidebar
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: 340
            color: "#0a0c12"
            border.color: root.cBorder

            Rectangle {
                id: explorerHeader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 44
                color: root.cPanelBg
                border.color: root.cBorder

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                    text: "EXPLORER"
                    color: root.cTextDim
                    font.pixelSize: 11
                    font.bold: true
                    font.letterSpacing: 1.5
                }

                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    height: 28
                    text: "Open Folder"
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        font.pixelSize: 11
                        font.bold: true
                    }
                    background: Rectangle {
                        radius: 4
                        color: parent.hovered ? root.cAccent : root.cSurface
                        border.color: root.cBorder
                    }
                    onClicked: library.selectFolder()
                }
            }

            ScrollView {
                anchors.top: explorerHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10
                clip: true

                ListView {
                id: folderListView
                anchors.top: explorerHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 10
                clip: true
                model: library.folders
                spacing: 6
                property var collapsedFolders: ({})

                ScrollBar.vertical: ScrollBar {
                    active: true
                    width: 6
                    policy: ScrollBar.AsNeeded
                }

                delegate: Column {
                    id: folderGroupItem
                    width: folderListView.width - (folderListView.ScrollBar.vertical.visible ? 10 : 0)
                    spacing: 2
                    property var folderData: modelData
                    property bool isOpen: !folderListView.collapsedFolders[folderData.fullPath]

                    Rectangle {
                        width: parent.width
                        height: 32
                        radius: 4
                        color: root.cSurface
                        border.color: root.cBorder

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            spacing: 8

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: folderGroupItem.isOpen ? "▼" : "▶"
                                color: root.cAccent
                                font.pixelSize: 10
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: folderGroupItem.folderData.folderName + " (" + folderGroupItem.folderData.tracks.length + ")"
                                color: root.cTextMain
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                                width: folderGroupItem.width - 45
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var updated = Object.assign({}, folderListView.collapsedFolders)
                                var key = folderGroupItem.folderData.fullPath
                                if (updated[key]) {
                                    delete updated[key]
                                } else {
                                    updated[key] = true
                                }
                                folderListView.collapsedFolders = updated
                            }
                        }
                    }

                    Column {
                        width: parent.width
                        visible: folderGroupItem.isOpen
                        spacing: 1

                        Repeater {
                            model: folderGroupItem.isOpen ? folderGroupItem.folderData.tracks : []

                            delegate: Rectangle {
                                id: trackItem
                                width: parent.width
                                height: 28
                                radius: 3
                                property var trackData: modelData
                                property bool isSelected: library.currentTrackPath === trackData.fullPath
                                color: isSelected ? "#231f3d" : (trackArea.containsMouse ? "#121520" : "transparent")

                                Rectangle {
                                    width: 2
                                    height: parent.height
                                    color: root.cAccent
                                    visible: trackItem.isSelected
                                }

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 18
                                    anchors.right: parent.right
                                    anchors.rightMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: trackItem.trackData.name
                                    color: trackItem.isSelected ? "#ffffff" : root.cTextDim
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }

                                MouseArea {
                                    id: trackArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        library.setCurrentTrackPath(trackItem.trackData.fullPath)
                                        audioEngine.play(trackItem.trackData.fullPath)
                                    }
                                }
                            }
                        }
                    }
                }
                }
            }
        }
    }

    Rectangle {
        id: statusBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 28
        color: "#0a0c10"
        border.color: root.cBorder

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: "Ready"
            color: root.cTextDim
            font.pixelSize: 10
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: "C++20 ENGINE"
            color: root.cAccent
            font.pixelSize: 10
            font.bold: true
        }
    }
}