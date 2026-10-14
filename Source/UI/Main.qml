import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    width: 1100
    height: 760
    minimumWidth: 880
    minimumHeight: 600
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

    readonly property var eqLabels: ["60", "170", "350", "1k", "3.5k", "10k", "14k"]
    property string saveButtonText: "Save"

    Connections {
        target: library
        function onCurrentTrackPathChanged() {
            var cfg = settingsStore.getTrackSettings(library.currentTrackPath)
            if (cfg.volume !== undefined) audioEngine.setVolume(cfg.volume)
            if (cfg.speed !== undefined) audioEngine.setSpeed(cfg.speed)
            if (cfg.eq !== undefined && cfg.eq.length === 7) {
                audioEngine.setAllEqGains(cfg.eq)
            } else {
                audioEngine.resetEq()
            }
        }
    }

    Connections {
        target: settingsStore
        function onSettingsSaved() {
            root.saveButtonText = "Saved!"
            savedResetTimer.restart()
        }
    }

    Timer {
        id: savedResetTimer
        interval: 1500
        onTriggered: root.saveButtonText = "Save"
    }

    Shortcut {
        sequence: "Space"
        onActivated: audioEngine.stopWithReturn()
    }

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
            anchors.margins: 18

            Column {
                anchors.fill: parent
                spacing: 14

                Rectangle {
                    width: parent.width
                    height: 115
                    radius: 12
                    color: root.cPanelBg
                    border.color: root.cBorder
                    clip: true

                    Image {
                        anchors.fill: parent
                        source: library.currentCoverUrl
                        fillMode: Image.PreserveAspectCrop
                        opacity: 0.15
                        visible: library.currentCoverUrl !== ""
                    }

                    Row {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 16

                        Rectangle {
                            width: 86
                            height: 86
                            radius: 8
                            color: root.cSurface
                            border.color: root.cBorder
                            clip: true

                            Image {
                                anchors.fill: parent
                                source: library.currentCoverUrl
                                fillMode: Image.PreserveAspectCrop
                                visible: library.currentCoverUrl !== ""
                            }

                            Text {
                                anchors.centerIn: parent
                                text: "♪"
                                color: root.cAccent
                                font.pixelSize: 30
                                visible: library.currentCoverUrl === ""
                            }
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 4
                            width: parent.width - 120

                            Text {
                                text: library.currentTitle
                                color: root.cTextMain
                                font.pixelSize: 18
                                font.bold: true
                                elide: Text.ElideRight
                                width: parent.width
                            }

                            Text {
                                text: library.currentArtist + (library.currentAlbum !== "" ? " • " + library.currentAlbum : "")
                                color: root.cAccent
                                font.pixelSize: 12
                                elide: Text.ElideRight
                                width: parent.width
                            }

                            Text {
                                text: library.currentTrackPath === "" ? "Ready" : library.currentTrackPath
                                color: root.cTextDim
                                font.pixelSize: 10
                                elide: Text.ElideMiddle
                                width: parent.width
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 135
                    radius: 12
                    color: root.cPanelBg
                    border.color: root.cBorder

                    Column {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 10

                        Row {
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 18

                            Button {
                                width: 36
                                height: 36
                                text: "⏮"
                                contentItem: Text {
                                    text: parent.text
                                    color: root.cTextMain
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.pixelSize: 14
                                }
                                background: Rectangle {
                                    radius: 18
                                    color: parent.hovered ? "#23283b" : root.cSurface
                                    border.color: root.cBorder
                                }
                                onClicked: {
                                    if (audioEngine.position > 3.0) {
                                        audioEngine.seek(0.0)
                                    } else {
                                        var prev = library.getPreviousTrack()
                                        if (prev !== "") {
                                            library.setCurrentTrackPath(prev)
                                            audioEngine.play(prev)
                                        }
                                    }
                                }
                            }

                            Button {
                                width: 44
                                height: 44
                                text: audioEngine.isPlaying ? "❚❚" : "▶"
                                contentItem: Text {
                                    text: parent.text
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.pixelSize: 15
                                    font.bold: true
                                }
                                background: Rectangle {
                                    radius: 22
                                    color: root.cAccent
                                }
                                onClicked: audioEngine.togglePlayPause()
                            }

                            Button {
                                width: 36
                                height: 36
                                text: "⏭"
                                contentItem: Text {
                                    text: parent.text
                                    color: root.cTextMain
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.pixelSize: 14
                                }
                                background: Rectangle {
                                    radius: 18
                                    color: parent.hovered ? "#23283b" : root.cSurface
                                    border.color: root.cBorder
                                }
                                onClicked: {
                                    var next = library.getNextTrack()
                                    if (next !== "") {
                                        library.setCurrentTrackPath(next)
                                        audioEngine.play(next)
                                    }
                                }
                            }
                        }

                        Row {
                            width: parent.width
                            spacing: 8

                            Text {
                                text: audioEngine.formatTime(audioEngine.position)
                                color: root.cTextDim
                                font.pixelSize: 11
                                font.family: "monospace"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Slider {
                                id: progressSlider
                                width: parent.width - 86
                                from: 0.0
                                to: Math.max(0.1, audioEngine.duration)
                                value: pressed ? value : audioEngine.position
                                anchors.verticalCenter: parent.verticalCenter
                                onMoved: audioEngine.seek(value)
                            }

                            Text {
                                text: audioEngine.formatTime(audioEngine.duration)
                                color: root.cTextDim
                                font.pixelSize: 11
                                font.family: "monospace"
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        Row {
                            width: parent.width
                            spacing: 20

                            Row {
                                spacing: 8
                                Text {
                                    text: "VOL"
                                    color: root.cTextDim
                                    font.pixelSize: 10
                                    font.bold: true
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Slider {
                                    width: 90
                                    from: 0.0
                                    to: 2.0
                                    value: audioEngine.volume
                                    anchors.verticalCenter: parent.verticalCenter
                                    onMoved: audioEngine.setVolume(value)
                                }
                                Text {
                                    text: Math.round(audioEngine.volume * 100) + "%"
                                    color: root.cTextDim
                                    font.pixelSize: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            Row {
                                spacing: 8
                                Text {
                                    text: "SPD"
                                    color: root.cTextDim
                                    font.pixelSize: 10
                                    font.bold: true
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Slider {
                                    width: 90
                                    from: 0.5
                                    to: 2.0
                                    value: audioEngine.speed
                                    anchors.verticalCenter: parent.verticalCenter
                                    onMoved: audioEngine.setSpeed(value)
                                }
                                Text {
                                    text: Math.round(audioEngine.speed * 100) + "%"
                                    color: root.cTextDim
                                    font.pixelSize: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            Item { width: 10; height: 1 }

                            Row {
                                spacing: 6
                                anchors.verticalCenter: parent.verticalCenter

                                Button {
                                    height: 24
                                    text: root.saveButtonText
                                    contentItem: Text {
                                        text: parent.text
                                        color: root.saveButtonText === "Saved!" ? "#10b981" : root.cTextMain
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                    background: Rectangle {
                                        radius: 4
                                        color: root.cSurface
                                        border.color: root.saveButtonText === "Saved!" ? "#10b981" : root.cBorder
                                    }
                                    onClicked: settingsStore.saveTrackSettings(library.currentTrackPath, audioEngine.volume, audioEngine.speed, audioEngine.eqGains)
                                }

                                Button {
                                    height: 24
                                    text: "Reset"
                                    contentItem: Text {
                                        text: parent.text
                                        color: root.cTextDim
                                        font.pixelSize: 10
                                    }
                                    background: Rectangle {
                                        radius: 4
                                        color: root.cSurface
                                        border.color: root.cBorder
                                    }
                                    onClicked: {
                                        settingsStore.resetTrackSettings(library.currentTrackPath)
                                        audioEngine.setVolume(1.0)
                                        audioEngine.setSpeed(1.0)
                                        audioEngine.resetEq()
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 160
                    radius: 12
                    color: root.cPanelBg
                    border.color: root.cBorder

                    Column {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        Text {
                            text: "EQUALIZER (7-BAND DSP)"
                            color: root.cTextDim
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1.5
                        }

                        Row {
                            width: parent.width
                            height: 110
                            spacing: (width - (7 * 42)) / 6

                            Repeater {
                                model: 7

                                delegate: Column {
                                    width: 42
                                    height: parent.height
                                    spacing: 4

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: (audioEngine.eqGains[index] > 0 ? "+" : "") + audioEngine.eqGains[index].toFixed(1)
                                        color: audioEngine.eqGains[index] !== 0 ? root.cAccent : root.cTextDim
                                        font.pixelSize: 9
                                        font.family: "monospace"
                                    }

                                    Slider {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        height: 72
                                        orientation: Qt.Vertical
                                        from: -12.0
                                        to: 12.0
                                        value: audioEngine.eqGains[index]
                                        onMoved: audioEngine.setEqGain(index, value)
                                    }

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: root.eqLabels[index]
                                        color: root.cTextDim
                                        font.pixelSize: 10
                                        font.bold: true
                                    }
                                }
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
                height: 80
                color: root.cPanelBg
                border.color: root.cBorder

                Item {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 40

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
                        height: 26
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

                Rectangle {
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 8
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    height: 28
                    color: root.cSurface
                    border.color: root.cBorder
                    radius: 4

                    TextInput {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        verticalAlignment: TextInput.AlignVCenter
                        color: root.cTextMain
                        font.pixelSize: 11
                        selectByMouse: true

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Search tracks..."
                            color: root.cTextDim
                            font.pixelSize: 11
                            visible: parent.text === ""
                        }

                        onTextChanged: library.setSearchQuery(text)
                    }
                }
            }

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
                    property bool isOpen: library.searchQuery !== "" || !folderListView.collapsedFolders[folderData.fullPath]

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
            text: "C++20 ENGINE • 7-BAND DSP"
            color: root.cAccent
            font.pixelSize: 10
            font.bold: true
        }
    }
}