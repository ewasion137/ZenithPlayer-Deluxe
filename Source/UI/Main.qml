import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    width: 1150
    height: 760
    minimumWidth: 900
    minimumHeight: 620
    visible: true
    title: "Zenith Player Deluxe"
    color: "#08090d"
    flags: Qt.Window | Qt.FramelessWindowHint

    readonly property color cBgMain: "#08090d"
    readonly property color cPanelBg: "#10121a"
    readonly property color cSurface: "#181a26"
    readonly property color cAccent: "#8b5cf6"
    readonly property color cAccentGlow: "#3b2d66"
    readonly property color cTextMain: "#f8fafc"
    readonly property color cTextDim: "#717b94"
    readonly property color cBorder: "#1e2235"

    readonly property var eqLabels: ["60", "170", "350", "1k", "3.5k", "10k", "14k"]
    property string lastTrackPath: ""

    function saveCurrentTrack() {
        if (library.currentTrackPath !== "") {
            settingsStore.saveTrackSettings(
                library.currentTrackPath,
                audioEngine.volume,
                audioEngine.speed,
                audioEngine.eqGains
            )
        }
    }

    function loadTrack(path) {
        if (path === "") return

        var cfg = settingsStore.getTrackSettings(path)

        audioEngine.setVolume(cfg.volume !== undefined ? cfg.volume : 1.0)
        audioEngine.setSpeed(cfg.speed !== undefined ? cfg.speed : 1.0)

        if (cfg.eq !== undefined && cfg.eq.length === 7) {
            audioEngine.setAllEqGains(cfg.eq)
        } else {
            audioEngine.resetEq()
        }
    }

    Connections {
        target: library
        function onCurrentTrackPathChanged() {
            root.loadTrack(library.currentTrackPath)
        }
    }

    Timer {
        id: autoSaveTimer
        interval: 5000
        repeat: true
        running: library.currentTrackPath !== ""
        onTriggered: root.saveCurrentTrack()
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
        height: 38
        color: "#0a0c14"
        z: 100

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            text: "ZENITH PLAYER"
            color: root.cAccent
            font.pixelSize: 11
            font.bold: true
            font.letterSpacing: 2.0
        }

        Rectangle {
            anchors.centerIn: parent
            width: 320
            height: 26
            color: root.cSurface
            radius: 13
            border.color: root.cBorder

            TextInput {
                id: searchField
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                verticalAlignment: TextInput.AlignVCenter
                color: root.cTextMain
                font.pixelSize: 11
                selectByMouse: true

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    text: "Search tracks or artists..."
                    color: root.cTextDim
                    font.pixelSize: 11
                    visible: parent.text === ""
                }

                onTextChanged: library.setSearchQuery(text)
            }
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
                background: Rectangle { color: parent.hovered ? "#1f2438" : "transparent" }
                onClicked: root.showMinimized()
            }
            Button {
                width: 44
                height: parent.height
                text: root.visibility === Window.Maximized ? "❐" : "□"
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 13
                }
                background: Rectangle { color: parent.hovered ? "#1f2438" : "transparent" }
                onClicked: {
                    if (root.visibility === Window.Maximized) {
                        root.showNormal()
                    } else {
                        root.showMaximized()
                    }
                }
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
                onClicked: {
                    root.saveCurrentTrack()
                    Qt.quit()
                }
            }
        }

        DragHandler {
            target: null
            onActiveChanged: if (active) root.startSystemMove()
        }
    }

    Item {
        id: mainWorkspace
        anchors.top: titleBar.bottom
        anchors.bottom: bottomPlayerBar.top
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            id: leftExplorer
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: 320
            color: root.cPanelBg
            border.color: root.cBorder

            Item {
                id: explorerHeader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 44

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: "LIBRARY"
                    color: root.cTextDim
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1.5
                }

                Button {
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    height: 26
                    text: "+ Open Folder"
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        font.pixelSize: 10
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

            ListView {
                id: folderListView
                anchors.top: explorerHeader.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 8
                clip: true
                model: library.folders
                spacing: 4
                property var collapsedFolders: ({})

                ScrollBar.vertical: ScrollBar {
                    active: true
                    width: 5
                    policy: ScrollBar.AsNeeded
                }

                delegate: Column {
                    id: folderGroupItem
                    width: folderListView.width - (folderListView.ScrollBar.vertical.visible ? 8 : 0)
                    spacing: 2
                    property var folderData: modelData
                    property bool isOpen: library.searchQuery !== "" || !folderListView.collapsedFolders[folderData.fullPath]

                    Rectangle {
                        width: parent.width
                        height: 28
                        radius: 4
                        color: root.cSurface
                        border.color: root.cBorder

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            spacing: 8

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: folderGroupItem.isOpen ? "▼" : "▶"
                                color: root.cAccent
                                font.pixelSize: 9
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: folderGroupItem.folderData.folderName + " (" + folderGroupItem.folderData.tracks.length + ")"
                                color: root.cTextMain
                                font.pixelSize: 11
                                font.bold: true
                                elide: Text.ElideRight
                                width: folderGroupItem.width - 40
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var updated = Object.assign({}, folderListView.collapsedFolders)
                                var key = folderGroupItem.folderData.fullPath
                                if (updated[key]) delete updated[key]
                                else updated[key] = true
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
                                height: 26
                                radius: 3
                                property var trackData: modelData
                                property bool isSelected: library.currentTrackPath === trackData.fullPath
                                color: isSelected ? "#231e3d" : (trackArea.containsMouse ? "#181a26" : "transparent")

                                Rectangle {
                                    width: 2
                                    height: parent.height
                                    color: root.cAccent
                                    visible: trackItem.isSelected
                                }

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 16
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

        Item {
            id: mainStage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: leftExplorer.right
            anchors.right: parent.right
            anchors.margins: 20

            Column {
                anchors.fill: parent
                spacing: 20

                Rectangle {
                    width: parent.width
                    height: 210
                    radius: 14
                    color: root.cPanelBg
                    border.color: root.cBorder
                    clip: true

                    Image {
                        anchors.fill: parent
                        source: library.currentCoverUrl
                        fillMode: Image.PreserveAspectCrop
                        opacity: 0.12
                        visible: library.currentCoverUrl !== ""
                    }

                    Row {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 24

                        Rectangle {
                            width: 170
                            height: 170
                            radius: 10
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
                                font.pixelSize: 48
                                visible: library.currentCoverUrl === ""
                            }
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 8
                            width: parent.width - 210

                            Text {
                                text: library.currentTitle
                                color: root.cTextMain
                                font.pixelSize: 24
                                font.bold: true
                                elide: Text.ElideRight
                                width: parent.width
                            }

                            Text {
                                text: library.currentArtist + (library.currentAlbum !== "" ? " • " + library.currentAlbum : "")
                                color: root.cAccent
                                font.pixelSize: 14
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
                    height: 210
                    radius: 14
                    color: root.cPanelBg
                    border.color: root.cBorder

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 10

                        Item {
                            width: parent.width
                            height: 24

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: "GRAPHIC EQUALIZER (7-BAND DSP)"
                                color: root.cTextDim
                                font.pixelSize: 11
                                font.bold: true
                                font.letterSpacing: 1.5
                            }

                            Button {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                height: 22
                                text: "Flat / Reset"
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
                                    audioEngine.resetEq()
                                    root.saveCurrentTrack()
                                }
                            }
                        }

                        Row {
                            width: parent.width
                            height: 140
                            spacing: (width - (7 * 48)) / 6

                            Repeater {
                                model: 7

                                delegate: Column {
                                    width: 48
                                    height: parent.height
                                    spacing: 4

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: (audioEngine.eqGains[index] > 0 ? "+" : "") + audioEngine.eqGains[index].toFixed(1)
                                        color: audioEngine.eqGains[index] !== 0 ? root.cAccent : root.cTextDim
                                        font.pixelSize: 10
                                        font.family: "monospace"
                                    }

                                    Slider {
                                        id: eqSlider
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        height: 96
                                        orientation: Qt.Vertical
                                        from: -12.0
                                        to: 12.0
                                        stepSize: 0.1

                                        Binding {
                                            target: eqSlider
                                            property: "value"
                                            value: audioEngine.eqGains[index]
                                            when: !eqSlider.pressed
                                        }

                                        onMoved: {
                                            audioEngine.setEqGain(index, value)
                                            root.saveCurrentTrack()
                                        }

                                        background: Rectangle {
                                            x: eqSlider.leftPadding + eqSlider.availableWidth / 2 - width / 2
                                            y: eqSlider.topPadding
                                            implicitWidth: 4
                                            implicitHeight: 96
                                            width: implicitWidth
                                            height: eqSlider.availableHeight
                                            radius: 2
                                            color: "#181a26"

                                            Rectangle {
                                                x: 0
                                                width: parent.width
                                                y: eqSlider.position >= 0.5 ? (1.0 - eqSlider.position) * parent.height : parent.height / 2
                                                height: Math.abs(eqSlider.position - 0.5) * parent.height
                                                color: root.cAccent
                                                radius: 2
                                            }
                                        }

                                        handle: Rectangle {
                                            x: eqSlider.leftPadding + eqSlider.availableWidth / 2 - width / 2
                                            y: eqSlider.topPadding + (1.0 - eqSlider.position) * (eqSlider.availableHeight - height)
                                            implicitWidth: 16
                                            implicitHeight: 8
                                            radius: 2
                                            color: eqSlider.pressed ? "#ffffff" : root.cAccent
                                        }
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
    }

    Rectangle {
        id: bottomPlayerBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 76
        color: "#0c0e17"
        border.color: root.cBorder

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            width: 260
            spacing: 12

            Rectangle {
                width: 48
                height: 48
                radius: 6
                color: root.cSurface
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
                    font.pixelSize: 20
                    visible: library.currentCoverUrl === ""
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - 64
                spacing: 3

                Text {
                    text: library.currentTitle
                    color: root.cTextMain
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                    width: parent.width
                }

                Text {
                    text: library.currentArtist
                    color: root.cTextDim
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    width: parent.width
                }
            }
        }

        Column {
            anchors.centerIn: parent
            width: 480
            spacing: 4

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 14

                Button {
                    width: 32
                    height: 32
                    text: "⏮"
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 13
                    }
                    background: Rectangle {
                        radius: 16
                        color: parent.hovered ? "#1f2438" : "transparent"
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
                    width: 36
                    height: 36
                    text: audioEngine.isPlaying ? "❚❚" : "▶"
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                        font.bold: true
                    }
                    background: Rectangle {
                        radius: 18
                        color: root.cAccent
                    }
                    onClicked: audioEngine.togglePlayPause()
                }

                Button {
                    width: 32
                    height: 32
                    text: "⏭"
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 13
                    }
                    background: Rectangle {
                        radius: 16
                        color: parent.hovered ? "#1f2438" : "transparent"
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
                    font.pixelSize: 10
                    font.family: "monospace"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Slider {
                    id: progressSlider
                    width: parent.width - 80
                    from: 0.0
                    to: Math.max(0.1, audioEngine.duration)
                    value: pressed ? value : audioEngine.position
                    anchors.verticalCenter: parent.verticalCenter
                    onMoved: audioEngine.seek(value)

                    background: Rectangle {
                        x: progressSlider.leftPadding
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: progressSlider.availableWidth
                        height: 4
                        radius: 2
                        color: "#181a26"

                        Rectangle {
                            width: progressSlider.visualPosition * parent.width
                            height: parent.height
                            color: root.cAccent
                            radius: 2
                        }
                    }

                    handle: Rectangle {
                        x: progressSlider.leftPadding + progressSlider.visualPosition * (progressSlider.availableWidth - width)
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: progressSlider.hovered || progressSlider.pressed ? 10 : 0
                        height: width
                        radius: width / 2
                        color: "#ffffff"
                    }
                }

                Text {
                    text: audioEngine.formatTime(audioEngine.duration)
                    color: root.cTextDim
                    font.pixelSize: 10
                    font.family: "monospace"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 16

            Row {
                spacing: 6
                Text {
                    text: "VOL"
                    color: root.cTextDim
                    font.pixelSize: 9
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Slider {
                    id: volSlider
                    width: 70
                    from: 0.0
                    to: 2.0

                    Binding {
                        target: volSlider
                        property: "value"
                        value: audioEngine.volume
                        when: !volSlider.pressed
                    }

                    anchors.verticalCenter: parent.verticalCenter
                    onMoved: {
                        audioEngine.setVolume(value)
                        root.saveCurrentTrack()
                    }

                    background: Rectangle {
                        x: volSlider.leftPadding
                        y: volSlider.topPadding + volSlider.availableHeight / 2 - height / 2
                        width: volSlider.availableWidth
                        height: 3
                        radius: 1.5
                        color: "#181a26"

                        Rectangle {
                            width: volSlider.visualPosition * parent.width
                            height: parent.height
                            color: root.cTextMain
                            radius: 1.5
                        }
                    }

                    handle: Rectangle {
                        x: volSlider.leftPadding + volSlider.visualPosition * (volSlider.availableWidth - width)
                        y: volSlider.topPadding + volSlider.availableHeight / 2 - height / 2
                        width: 8
                        height: 8
                        radius: 4
                        color: "#ffffff"
                    }
                }
            }

            Row {
                spacing: 6
                Text {
                    text: "SPD"
                    color: root.cTextDim
                    font.pixelSize: 9
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Slider {
                    id: spdSlider
                    width: 70
                    from: 0.5
                    to: 2.0

                    Binding {
                        target: spdSlider
                        property: "value"
                        value: audioEngine.speed
                        when: !spdSlider.pressed
                    }

                    anchors.verticalCenter: parent.verticalCenter
                    onMoved: {
                        audioEngine.setSpeed(value)
                        root.saveCurrentTrack()
                    }

                    background: Rectangle {
                        x: spdSlider.leftPadding
                        y: spdSlider.topPadding + spdSlider.availableHeight / 2 - height / 2
                        width: spdSlider.availableWidth
                        height: 3
                        radius: 1.5
                        color: "#181a26"

                        Rectangle {
                            width: spdSlider.visualPosition * parent.width
                            height: parent.height
                            color: root.cAccent
                            radius: 1.5
                        }
                    }

                    handle: Rectangle {
                        x: spdSlider.leftPadding + spdSlider.visualPosition * (spdSlider.availableWidth - width)
                        y: spdSlider.topPadding + spdSlider.availableHeight / 2 - height / 2
                        width: 8
                        height: 8
                        radius: 4
                        color: "#ffffff"
                    }
                }
                Text {
                    text: Math.round(audioEngine.speed * 100) + "%"
                    color: root.cTextDim
                    font.pixelSize: 10
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}