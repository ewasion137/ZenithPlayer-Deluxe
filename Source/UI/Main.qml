import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: root
    width: 1180
    height: 760
    minimumWidth: 920
    minimumHeight: 620
    visible: true
    title: "Zenith Player Deluxe"
    color: "#08090d"
    flags: Qt.Window | Qt.FramelessWindowHint

    readonly property color cBgMain: (typeof themeManager !== "undefined" && themeManager) ? themeManager.bgMain : "#08090d"
    readonly property color cPanelBg: (typeof themeManager !== "undefined" && themeManager) ? themeManager.panelBg : "#10121a"
    readonly property color cSurface: (typeof themeManager !== "undefined" && themeManager) ? themeManager.surface : "#181a26"
    readonly property color cAccent: (typeof themeManager !== "undefined" && themeManager) ? themeManager.accent : "#8b5cf6"
    readonly property color cAccentGlow: (typeof themeManager !== "undefined" && themeManager) ? themeManager.accentGlow : "#3b2d66"
    readonly property color cTextMain: (typeof themeManager !== "undefined" && themeManager) ? themeManager.textMain : "#f8fafc"
    readonly property color cTextDim: (typeof themeManager !== "undefined" && themeManager) ? themeManager.textDim : "#717b94"
    readonly property color cBorder: (typeof themeManager !== "undefined" && themeManager) ? themeManager.border : "#1e2235"
    readonly property string cFont: (typeof themeManager !== "undefined" && themeManager && themeManager.fontFamily !== "") ? themeManager.fontFamily : "Segoe UI"

    readonly property var eqLabels: ["60", "170", "350", "1k", "3.5k", "10k", "14k"]
    property real sidebarWidth: 320


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

    Shortcut {
        sequence: "Alt+P"
        onActivated: audioEngine.togglePlayPause()
    }

    Shortcut {
        sequence: "Alt+Right"
        onActivated: {
            var next = library.getNextTrack()
            if (next !== "") {
                library.setCurrentTrackPath(next)
                audioEngine.play(next)
            }
        }
    }

    Shortcut {
        sequence: "Alt+Left"
        onActivated: {
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

    Shortcut {
        sequence: StandardKey.MediaPlay
        onActivated: audioEngine.togglePlayPause()
    }

    Shortcut {
        sequence: StandardKey.MediaNext
        onActivated: {
            var nextTrack = library.getNextTrack()
            if (nextTrack !== "") {
                library.setCurrentTrackPath(nextTrack)
                audioEngine.play(nextTrack)
            }
        }
    }

    Shortcut {
        sequence: StandardKey.MediaPrevious
        onActivated: {
            if (audioEngine.position > 3.0) {
                audioEngine.seek(0.0)
            } else {
                var prevTrack = library.getPreviousTrack()
                if (prevTrack !== "") {
                    library.setCurrentTrackPath(prevTrack)
                    audioEngine.play(prevTrack)
                }
            }
        }
    }

    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 38
        color: "#0a0c14"
        z: 100

        ThemeAnimator { targetItem: titleBar; targetName: "title_bar" }

        Text {
            id: titleLabel
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            text: (typeof themeManager !== "undefined" && themeManager && themeManager.textOverrides["window_title"]) ? themeManager.textOverrides["window_title"] : "ZENITH PLAYER"
            color: root.cAccent
            font.family: root.cFont
            font.pixelSize: 11
            font.bold: true
            font.letterSpacing: 2.0

            ThemeAnimator { targetItem: titleLabel; targetName: "title" }
        }

        Button {
            id: rpcBtn
            anchors.left: titleLabel.right
            anchors.leftMargin: 14
            anchors.verticalCenter: parent.verticalCenter
            height: 24
            text: "🎮 " + (discordRPC.isConnected ? "RPC: ON" : "RPC")

            ThemeAnimator {
                targetItem: rpcBtn
                targetName: "rpc_btn"
                isHovered: rpcBtn.hovered
            }

            contentItem: Text {
                text: parent.text
                color: discordRPC.isConnected ? "#10b981" : root.cTextDim
                font.family: root.cFont
                font.pixelSize: 10
                font.bold: true
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 4
                color: parent.hovered ? "#1f2438" : root.cSurface
                border.color: discordRPC.isConnected ? "#10b981" : root.cBorder
            }
            onClicked: rpcModal.visible = true
        }

        Button {
            id: settingsBtn
            anchors.left: rpcBtn.right
            anchors.leftMargin: 8
            anchors.verticalCenter: parent.verticalCenter
            height: 24
            text: "⚙ Settings"

            ThemeAnimator {
                targetItem: settingsBtn
                targetName: "settings_btn"
                isHovered: settingsBtn.hovered
            }

            contentItem: Text {
                text: parent.text
                color: root.cTextDim
                font.family: root.cFont
                font.pixelSize: 10
                font.bold: true
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                radius: 4
                color: parent.hovered ? "#1f2438" : root.cSurface
                border.color: root.cBorder
            }
            onClicked: settingsDialog.visible = true
        }

        Rectangle {
            id: searchContainer
            anchors.centerIn: parent
            width: 320
            height: 26
            color: root.cSurface
            radius: 13
            border.color: root.cBorder

            ThemeAnimator { targetItem: searchContainer; targetName: "search_bar" }

            TextInput {
                id: searchField
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                verticalAlignment: TextInput.AlignVCenter
                color: root.cTextMain
                font.family: root.cFont
                font.pixelSize: 11
                selectByMouse: true

                ThemeAnimator { targetItem: searchField; targetName: "search_input" }

                Text {
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    text: streamManager.isBusy ? streamManager.statusText : "Search library or paste YouTube/Spotify link..."
                    color: streamManager.isBusy ? root.cAccent : root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 11
                    visible: parent.text === ""
                }

                onAccepted: {
                    var query = text.trim()
                    if (query.startsWith("http://") || query.startsWith("https://")) {
                        streamManager.resolveAndDownload(query)
                        text = ""
                    }
                }

                onTextChanged: library.setSearchQuery(text)
            }
        }

        Row {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Button {
                id: btnMin
                width: 44
                height: parent.height
                text: "-"
                ThemeAnimator { targetItem: btnMin; targetName: "btn_min"; isHovered: btnMin.hovered }
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    font.family: root.cFont
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
                background: Rectangle { color: parent.hovered ? "#1f2438" : "transparent" }
                onClicked: root.showMinimized()
            }

            Button {
                id: btnMax
                width: 44
                height: parent.height
                text: root.visibility === Window.Maximized ? "❐" : "□"
                ThemeAnimator { targetItem: btnMax; targetName: "btn_max"; isHovered: btnMax.hovered }
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    font.family: root.cFont
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 13
                }
                background: Rectangle { color: parent.hovered ? "#1f2438" : "transparent" }
                onClicked: {
                    if (root.visibility === Window.Maximized) root.showNormal()
                    else root.showMaximized()
                }
            }

            Button {
                id: btnClose
                width: 44
                height: parent.height
                text: "✕"
                ThemeAnimator { targetItem: btnClose; targetName: "btn_close"; isHovered: btnClose.hovered }
                contentItem: Text {
                    text: parent.text
                    color: root.cTextDim
                    font.family: root.cFont
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

        TapHandler {
            onDoubleTapped: {
                if (root.visibility === Window.Maximized) root.showNormal()
                else root.showMaximized()
            }
        }
    }

    Item {
        id: mainWorkspace
        anchors.top: titleBar.bottom
        anchors.bottom: bottomPlayerBar.top
        anchors.left: parent.left
        anchors.right: parent.right

        Image {
            id: bgCustomImg
            anchors.fill: parent
            source: (typeof themeManager !== "undefined" && themeManager) ? themeManager.bgImageUrl : ""
            fillMode: Image.PreserveAspectCrop
            opacity: 0.18
            visible: (typeof themeManager !== "undefined" && themeManager && themeManager.bgImageUrl !== "")
            ThemeAnimator { targetItem: bgCustomImg; targetName: "background_image" }
        }

        Rectangle {
            id: leftExplorer
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: root.sidebarWidth
            color: root.cPanelBg
            border.color: root.cBorder

            ThemeAnimator { targetItem: leftExplorer; targetName: "sidebar" }

            Item {
                id: explorerHeader
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 44

                ThemeAnimator { targetItem: explorerHeader; targetName: "sidebar_header" }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    anchors.verticalCenter: parent.verticalCenter
                    text: "LIBRARY"
                    color: root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1.5
                }

                Row {
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 6

                    Button {
                        id: btnAddUrl
                        height: 26
                        text: "🔗 + URL"
                        ThemeAnimator { targetItem: btnAddUrl; targetName: "btn_add_url"; isHovered: btnAddUrl.hovered }
                        contentItem: Text {
                            text: parent.text
                            color: root.cAccent
                            font.family: root.cFont
                            font.pixelSize: 10
                            font.bold: true
                        }
                        background: Rectangle {
                            radius: 4
                            color: parent.hovered ? "#231f3d" : root.cSurface
                            border.color: root.cAccent
                        }
                        onClicked: urlModal.visible = true
                    }

                    Button {
                        id: btnAddFolder
                        height: 26
                        text: "+ Folder"
                        ThemeAnimator { targetItem: btnAddFolder; targetName: "btn_add_folder"; isHovered: btnAddFolder.hovered }
                        contentItem: Text {
                            text: parent.text
                            color: root.cTextMain
                            font.family: root.cFont
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
                property var openFolders: ({})

                ThemeAnimator { targetItem: folderListView; targetName: "folder_list" }

                ScrollBar.vertical: ScrollBar {
                    active: true
                    width: 5
                    policy: ScrollBar.AsNeeded
                }

                header: Column {
                    id: webHeaderCol
                    width: folderListView.width - (folderListView.ScrollBar.vertical.visible ? 8 : 0)
                    spacing: 2
                    visible: settingsStore.webTracks.length > 0
                    property bool isOpen: true

                    Rectangle {
                        id: webHeaderBanner
                        width: parent.width
                        height: 28
                        radius: 4
                        color: root.cSurface
                        border.color: root.cAccent

                        ThemeAnimator { targetItem: webHeaderBanner; targetName: "web_header" }

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            spacing: 8

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: webHeaderCol.isOpen ? "▼" : "▶"
                                color: root.cAccent
                                font.pixelSize: 9
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "🌐 Online Streams (" + settingsStore.webTracks.length + ")"
                                color: root.cAccent
                                font.family: root.cFont
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: webHeaderCol.isOpen = !webHeaderCol.isOpen
                        }
                    }

                    Column {
                        width: parent.width
                        visible: webHeaderCol.isOpen
                        spacing: 1

                        Repeater {
                            model: settingsStore.webTracks

                            delegate: Rectangle {
                                id: webTrackRow
                                width: parent.width
                                height: 26
                                radius: 3
                                property bool isSelected: library.currentTrackPath.indexOf(modelData.id) !== -1
                                color: isSelected ? "#231e3d" : (webArea.containsMouse ? "#181a26" : "transparent")

                                Rectangle {
                                    width: 2
                                    height: parent.height
                                    color: root.cAccent
                                    visible: webTrackRow.isSelected
                                }

                                Row {
                                    anchors.fill: parent
                                    anchors.leftMargin: 12
                                    anchors.rightMargin: 8
                                    spacing: 8

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: "#" + (settingsStore.webTracks.length - index)
                                        color: root.cAccent
                                        font.family: root.cFont
                                        font.pixelSize: 9
                                        font.bold: true
                                        width: 20
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: modelData.artist + " - " + modelData.name
                                        color: webTrackRow.isSelected ? "#ffffff" : root.cTextDim
                                        font.family: root.cFont
                                        font.pixelSize: 11
                                        elide: Text.ElideRight
                                        width: parent.width - 44
                                    }
                                }

                                MouseArea {
                                    id: webArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: streamManager.resolveAndDownload(modelData.url)
                                }
                            }
                        }
                    }

                    Item { width: parent.width; height: 8 }
                }

                delegate: Column {
                    id: folderGroupItem
                    width: folderListView.width - (folderListView.ScrollBar.vertical.visible ? 8 : 0)
                    spacing: 2
                    property var folderData: modelData
                    property bool isOpen: library.searchQuery !== "" || !!folderListView.openFolders[folderData.fullPath]

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
                                font.family: root.cFont
                                font.pixelSize: 11
                                font.bold: true
                                elide: Text.ElideRight
                                width: folderGroupItem.width - 40
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var updated = Object.assign({}, folderListView.openFolders)
                                var key = folderGroupItem.folderData.fullPath
                                if (updated[key]) delete updated[key]
                                else updated[key] = true
                                folderListView.openFolders = updated
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
                                color: isSelected ? "#231f3d" : (trackArea.containsMouse ? "#181a26" : "transparent")

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
                                    font.family: root.cFont
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

        Rectangle {
            id: sidebarSplitter
            anchors.left: leftExplorer.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 4
            color: splitterArea.containsMouse || splitterArea.drag.active ? root.cAccent : "transparent"

            ThemeAnimator {
                targetItem: sidebarSplitter
                targetName: "splitter"
                isHovered: splitterArea.containsMouse
            }

            MouseArea {
                id: splitterArea
                anchors.fill: parent
                cursorShape: Qt.SizeHorCursor
                hoverEnabled: true
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: 220
                drag.maximumX: 500
                onPositionChanged: {
                    if (drag.active) {
                        root.sidebarWidth = Math.max(220, Math.min(500, sidebarSplitter.x))
                    }
                }
            }
        }

        Item {
            id: mainStage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: sidebarSplitter.right
            anchors.right: parent.right
            anchors.margins: 20

            ThemeAnimator { targetItem: mainStage; targetName: "main_stage" }

            Canvas {
                id: specCanvas
                anchors.fill: parent
                opacity: 0.16
                visible: settingsStore.showSpectrogram && audioEngine.isPlaying

                ThemeAnimator { targetItem: specCanvas; targetName: "spectrum" }

                Connections {
                    target: audioEngine
                    function onSpectrumChanged() { specCanvas.requestPaint() }
                }

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    var bins = audioEngine.spectrum
                    if (!bins || bins.length === 0) return

                    var w = width
                    var h = height
                    var count = bins.length
                    var barW = (w / count) * 1.4

                    var grad = ctx.createLinearGradient(0, h, 0, h * 0.4)
                    grad.addColorStop(0, root.cAccent)
                    grad.addColorStop(1, "#38bdf8")
                    ctx.fillStyle = grad

                    for (var i = 0; i < count; i++) {
                        var val = bins[i]
                        var barH = val * (h * 0.45)
                        ctx.fillRect(i * (barW + 2), h - barH, barW, barH)
                    }
                }
            }

            Column {
                anchors.fill: parent
                spacing: 20

                Rectangle {
                    id: coverCard
                    width: parent.width
                    height: 210
                    radius: 14
                    color: root.cPanelBg
                    border.color: root.cBorder
                    clip: true

                    ThemeAnimator { targetItem: coverCard; targetName: "card_cover" }

                    Image {
                        id: coverGlowImg
                        anchors.fill: parent
                        source: library.currentCoverUrl
                        fillMode: Image.PreserveAspectCrop
                        opacity: 0.12
                        visible: settingsStore.showCoverGlow && library.currentCoverUrl !== ""
                        ThemeAnimator { targetItem: coverGlowImg; targetName: "cover_glow" }
                    }

                    Row {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 24

                        Rectangle {
                            id: coverArtBox
                            width: 170
                            height: 170
                            radius: 10
                            color: root.cSurface
                            border.color: root.cBorder
                            clip: true

                            ThemeAnimator {
                                targetItem: coverArtBox
                                targetName: "cover"
                            }

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
                                id: trackTitleText
                                text: library.currentTitle
                                color: root.cTextMain
                                font.family: root.cFont
                                font.pixelSize: 24
                                font.bold: true
                                elide: Text.ElideRight
                                width: parent.width
                                ThemeAnimator { targetItem: trackTitleText; targetName: "track_title" }
                            }

                            Text {
                                id: trackArtistText
                                text: library.currentArtist + (library.currentAlbum !== "" ? " • " + library.currentAlbum : "")
                                color: root.cAccent
                                font.family: root.cFont
                                font.pixelSize: 14
                                font.bold: true
                                elide: Text.ElideRight
                                width: parent.width
                                ThemeAnimator { targetItem: trackArtistText; targetName: "track_artist" }
                            }

                            Text {
                                id: trackPathText
                                text: library.currentTrackPath === "" ? "Ready to play" : library.currentTrackPath
                                color: root.cTextDim
                                font.family: root.cFont
                                font.pixelSize: 11
                                elide: Text.ElideMiddle
                                width: parent.width
                                ThemeAnimator { targetItem: trackPathText; targetName: "track_path" }
                            }
                        }
                    }
                }

                Rectangle {
                    id: eqCard
                    width: parent.width
                    height: 210
                    radius: 14
                    color: root.cPanelBg
                    border.color: root.cBorder

                    ThemeAnimator { targetItem: eqCard; targetName: "card_eq" }

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 10

                        Item {
                            id: eqHeader
                            width: parent.width
                            height: 24

                            ThemeAnimator { targetItem: eqHeader; targetName: "eq_header" }

                            Text {
                                anchors.left: parent.left
                                anchors.verticalCenter: parent.verticalCenter
                                text: "GRAPHIC EQUALIZER (7-BAND DSP)"
                                color: root.cTextDim
                                font.family: root.cFont
                                font.pixelSize: 11
                                font.bold: true
                                font.letterSpacing: 1.5
                            }

                            Button {
                                id: eqResetBtn
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                height: 22
                                text: "Flat / Reset"
                                ThemeAnimator { targetItem: eqResetBtn; targetName: "eq_reset_btn"; isHovered: eqResetBtn.hovered }
                                contentItem: Text {
                                    text: parent.text
                                    color: root.cTextDim
                                    font.family: root.cFont
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
                                    id: eqBandColumn
                                    width: 48
                                    height: parent.height
                                    spacing: 4

                                    ThemeAnimator {
                                        targetItem: eqBandColumn
                                        targetName: "eq_slider_" + index
                                    }
                                    ThemeAnimator {
                                        targetItem: eqBandColumn
                                        targetName: "eq_slider"
                                    }

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: (audioEngine.eqGains[index] > 0 ? "+" : "") + audioEngine.eqGains[index].toFixed(1)
                                        color: audioEngine.eqGains[index] !== 0 ? root.cAccent : root.cTextDim
                                        font.pixelSize: 10
                                        font.family: root.cFont
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
                                            implicitWidth: 6
                                            implicitHeight: 96
                                            width: implicitWidth
                                            height: eqSlider.availableHeight
                                            radius: 3
                                            color: "#181a26"

                                            Rectangle {
                                                x: 0
                                                width: parent.width
                                                y: eqSlider.position >= 0.5 ? (1.0 - eqSlider.position) * parent.height : parent.height / 2
                                                height: Math.abs(eqSlider.position - 0.5) * parent.height
                                                color: root.cAccent
                                                radius: 3
                                            }
                                        }

                                        handle: Rectangle {
                                            x: eqSlider.leftPadding + eqSlider.availableWidth / 2 - width / 2
                                            y: eqSlider.topPadding + (1.0 - eqSlider.position) * (eqSlider.availableHeight - height)
                                            implicitWidth: 18
                                            implicitHeight: 10
                                            radius: 3
                                            color: eqSlider.pressed ? "#ffffff" : root.cAccent
                                            border.color: "#ffffff"
                                            border.width: 1
                                        }
                                    }

                                    Text {
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        text: root.eqLabels[index]
                                        color: root.cTextDim
                                        font.family: root.cFont
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

        ThemeAnimator { targetItem: bottomPlayerBar; targetName: "bottom_bar" }

        Row {
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            width: 260
            spacing: 12

            Rectangle {
                id: bottomCoverBox
                width: 48
                height: 48
                radius: 6
                color: root.cSurface
                clip: true

                ThemeAnimator { targetItem: bottomCoverBox; targetName: "bottom_cover" }

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
                    id: bottomTitleText
                    text: library.currentTitle
                    color: root.cTextMain
                    font.family: root.cFont
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                    width: parent.width
                    ThemeAnimator { targetItem: bottomTitleText; targetName: "bottom_title" }
                }

                Text {
                    id: bottomArtistText
                    text: library.currentArtist
                    color: root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    width: parent.width
                    ThemeAnimator { targetItem: bottomArtistText; targetName: "bottom_artist" }
                }
            }
        }

        Column {
            anchors.centerIn: parent
            width: 480
            spacing: 4

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16

                Button {
                    id: btnPrev
                    width: 32
                    height: 32
                    text: "⏮"
                    ThemeAnimator { targetItem: btnPrev; targetName: "btn_prev"; isHovered: btnPrev.hovered }
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        font.family: root.cFont
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

                Rectangle {
                    id: playPauseBtn
                    width: 38
                    height: 38
                    radius: 19
                    color: root.cAccent

                    ThemeAnimator {
                        targetItem: playPauseBtn
                        targetName: "play_button"
                        isHovered: playMouseArea.containsMouse
                    }

                    Item {
                        anchors.centerIn: parent
                        width: 14
                        height: 14

                        Row {
                            anchors.centerIn: parent
                            spacing: 4
                            visible: audioEngine.isPlaying

                            Rectangle {
                                width: 3.5
                                height: 13
                                radius: 1
                                color: "#ffffff"
                            }
                            Rectangle {
                                width: 3.5
                                height: 13
                                radius: 1
                                color: "#ffffff"
                            }
                        }

                        Canvas {
                            anchors.fill: parent
                            visible: !audioEngine.isPlaying
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()
                                ctx.fillStyle = "#ffffff"
                                ctx.beginPath()
                                ctx.moveTo(2, 1)
                                ctx.lineTo(13, 7)
                                ctx.lineTo(2, 13)
                                ctx.closePath()
                                ctx.fill()
                            }
                        }
                    }

                    MouseArea {
                        id: playMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: audioEngine.togglePlayPause()
                    }
                }

                Button {
                    id: btnNext
                    width: 32
                    height: 32
                    text: "⏭"
                    ThemeAnimator { targetItem: btnNext; targetName: "btn_next"; isHovered: btnNext.hovered }
                    contentItem: Text {
                        text: parent.text
                        color: root.cTextMain
                        font.family: root.cFont
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
                    id: timePosText
                    text: audioEngine.formatTime(audioEngine.position)
                    color: root.cTextDim
                    font.pixelSize: 10
                    font.family: root.cFont
                    anchors.verticalCenter: parent.verticalCenter
                    ThemeAnimator { targetItem: timePosText; targetName: "time_pos" }
                }

                Slider {
                    id: progressSlider
                    width: parent.width - 80
                    from: 0.0
                    to: Math.max(0.1, audioEngine.duration)
                    value: pressed ? value : audioEngine.position
                    anchors.verticalCenter: parent.verticalCenter

                    ThemeAnimator { targetItem: progressSlider; targetName: "progress_slider" }

                    onPressedChanged: {
                        if (!pressed) {
                            audioEngine.seek(value)
                        }
                    }
                    onMoved: {
                        if (!pressed) {
                            audioEngine.seek(value)
                        }
                    }

                    background: Item {
                        x: progressSlider.leftPadding
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: progressSlider.availableWidth
                        height: 22

                        Canvas {
                            id: waveCanvas
                            anchors.fill: parent

                            ThemeAnimator { targetItem: waveCanvas; targetName: "waveform" }

                            Connections {
                                target: audioEngine
                                function onWaveformChanged() { waveCanvas.requestPaint() }
                                function onPositionChanged() { waveCanvas.requestPaint() }
                            }

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()
                                var peaks = audioEngine.waveform
                                var w = width
                                var h = height
                                var progress = progressSlider.visualPosition

                                if (!settingsStore.showWaveform || !peaks || peaks.length === 0) {
                                    ctx.fillStyle = "#181a26"
                                    ctx.fillRect(0, h / 2 - 2, w, 4)
                                    ctx.fillStyle = root.cAccent
                                    ctx.fillRect(0, h / 2 - 2, progress * w, 4)
                                    return
                                }

                                var count = peaks.length
                                var barW = (w / count) - 1.2
                                var progressX = progress * w

                                for (var i = 0; i < count; i++) {
                                    var x = i * (barW + 1.2)
                                    var barH = Math.max(2, peaks[i] * (h - 2))
                                    var y = (h - barH) / 2

                                    if (x <= progressX) {
                                        ctx.fillStyle = root.cAccent
                                    } else {
                                        ctx.fillStyle = "#222638"
                                    }
                                    ctx.fillRect(x, y, Math.max(1, barW), barH)
                                }
                            }
                        }
                    }

                    handle: Rectangle {
                        id: progressHandle
                        x: progressSlider.leftPadding + progressSlider.visualPosition * (progressSlider.availableWidth - width)
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: progressSlider.hovered || progressSlider.pressed ? 12 : 2
                        height: progressSlider.hovered || progressSlider.pressed ? 12 : 24
                        radius: progressSlider.hovered || progressSlider.pressed ? 6 : 1
                        color: "#ffffff"
                        ThemeAnimator { targetItem: progressHandle; targetName: "progress_handle" }
                    }
                }

                Text {
                    id: timeDurText
                    text: audioEngine.formatTime(audioEngine.duration)
                    color: root.cTextDim
                    font.pixelSize: 10
                    font.family: root.cFont
                    anchors.verticalCenter: parent.verticalCenter
                    ThemeAnimator { targetItem: timeDurText; targetName: "time_dur" }
                }
            }
        }

        Row {
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20

            Row {
                spacing: 8

                Text {
                    id: volLabel
                    text: "VOL"
                    color: root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 10
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                    ThemeAnimator { targetItem: volLabel; targetName: "vol_label" }
                }

                Slider {
                    id: volSlider
                    width: 160
                    from: 0.0
                    to: 2.0
                    stepSize: 0.01

                    ThemeAnimator { targetItem: volSlider; targetName: "vol_slider" }

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
                        height: 5
                        radius: 2.5
                        color: "#181a26"

                        Rectangle {
                            width: volSlider.visualPosition * parent.width
                            height: parent.height
                            color: root.cTextMain
                            radius: 2.5
                        }
                    }

                    handle: Rectangle {
                        x: volSlider.leftPadding + volSlider.visualPosition * (volSlider.availableWidth - width)
                        y: volSlider.topPadding + volSlider.availableHeight / 2 - height / 2
                        width: 14
                        height: 14
                        radius: 7
                        color: "#ffffff"
                    }
                }

                Rectangle {
                    id: volInputBox
                    width: 44
                    height: 22
                    color: root.cSurface
                    radius: 3
                    border.color: volInput.activeFocus ? root.cAccent : root.cBorder
                    anchors.verticalCenter: parent.verticalCenter

                    ThemeAnimator { targetItem: volInputBox; targetName: "vol_input" }

                    TextInput {
                        id: volInput
                        anchors.fill: parent
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 10
                        font.bold: true
                        selectByMouse: true
                        text: activeFocus ? text : Math.round(audioEngine.volume * 100) + "%"

                        onAccepted: {
                            var raw = parseInt(text.replace("%", "").trim())
                            if (!isNaN(raw)) {
                                var clamped = Math.max(0, Math.min(200, raw))
                                audioEngine.setVolume(clamped / 100.0)
                                root.saveCurrentTrack()
                            }
                            focus = false
                        }
                    }
                }
            }

            Row {
                spacing: 8

                Text {
                    id: spdLabel
                    text: "SPD"
                    color: root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 10
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                    ThemeAnimator { targetItem: spdLabel; targetName: "spd_label" }
                }

                Slider {
                    id: spdSlider
                    width: 160
                    from: 0.5
                    to: 2.0
                    stepSize: 0.01

                    ThemeAnimator { targetItem: spdSlider; targetName: "spd_slider" }

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
                        height: 5
                        radius: 2.5
                        color: "#181a26"

                        Rectangle {
                            width: spdSlider.visualPosition * parent.width
                            height: parent.height
                            color: root.cAccent
                            radius: 2.5
                        }
                    }

                    handle: Rectangle {
                        x: spdSlider.leftPadding + spdSlider.visualPosition * (spdSlider.availableWidth - width)
                        y: spdSlider.topPadding + spdSlider.availableHeight / 2 - height / 2
                        width: 14
                        height: 14
                        radius: 7
                        color: "#ffffff"
                    }
                }

                Rectangle {
                    id: spdInputBox
                    width: 44
                    height: 22
                    color: root.cSurface
                    radius: 3
                    border.color: spdInput.activeFocus ? root.cAccent : root.cBorder
                    anchors.verticalCenter: parent.verticalCenter

                    ThemeAnimator { targetItem: spdInputBox; targetName: "spd_input" }

                    TextInput {
                        id: spdInput
                        anchors.fill: parent
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 10
                        font.bold: true
                        selectByMouse: true
                        text: activeFocus ? text : Math.round(audioEngine.speed * 100) + "%"

                        onAccepted: {
                            var raw = parseInt(text.replace("%", "").trim())
                            if (!isNaN(raw)) {
                                var clamped = Math.max(50, Math.min(200, raw))
                                audioEngine.setSpeed(clamped / 100.0)
                                root.saveCurrentTrack()
                            }
                            focus = false
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: urlModal
        anchors.fill: parent
        color: "#b0000000"
        visible: false
        z: 9999

        MouseArea {
            anchors.fill: parent
            onClicked: if (!streamManager.isBusy) urlModal.visible = false
        }

        Rectangle {
            id: urlModalBox
            anchors.centerIn: parent
            width: 440
            height: 190
            radius: 12
            color: root.cPanelBg
            border.color: root.cAccent
            border.width: 1

            ThemeAnimator { targetItem: urlModalBox; targetName: "url_modal" }

            MouseArea { anchors.fill: parent }

            Column {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 12

                Item {
                    width: parent.width
                    height: 24

                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: "PLAY FROM WEB / YOUTUBE / SPOTIFY"
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.0
                    }

                    Button {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        text: "✕"
                        contentItem: Text {
                            text: parent.text
                            color: root.cTextDim
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle { color: parent.hovered ? "#ef4444" : "transparent"; radius: 12 }
                        onClicked: urlModal.visible = false
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 36
                    color: root.cSurface
                    radius: 6
                    border.color: root.cBorder

                    TextInput {
                        id: urlInputField
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        verticalAlignment: TextInput.AlignVCenter
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 12
                        selectByMouse: true

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Paste YouTube / Spotify URL or song title..."
                            color: root.cTextDim
                            font.family: root.cFont
                            font.pixelSize: 11
                            visible: parent.text === ""
                        }

                        onAccepted: downloadBtn.clicked()
                    }
                }

                Row {
                    width: parent.width
                    height: 24
                    spacing: 8

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Status: "
                        color: root.cTextDim
                        font.family: root.cFont
                        font.pixelSize: 11
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: streamManager.statusText
                        color: streamManager.isBusy ? root.cAccent : "#10b981"
                        font.family: root.cFont
                        font.pixelSize: 11
                        font.bold: true
                        elide: Text.ElideRight
                        width: parent.width - 60
                    }
                }

                Row {
                    anchors.right: parent.right
                    spacing: 10

                    Button {
                        height: 30
                        text: "Cancel"
                        contentItem: Text { text: parent.text; color: root.cTextDim; font.family: root.cFont; font.pixelSize: 11 }
                        background: Rectangle { radius: 4; color: root.cSurface; border.color: root.cBorder }
                        onClicked: urlModal.visible = false
                    }

                    Button {
                        id: downloadBtn
                        height: 30
                        width: 110
                        text: streamManager.isBusy ? "Loading..." : "Load & Play"
                        enabled: !streamManager.isBusy
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.family: root.cFont
                            font.pixelSize: 11
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: streamManager.isBusy ? "#4c3a7a" : root.cAccent
                        }
                        onClicked: {
                            if (urlInputField.text.trim() !== "") {
                                streamManager.resolveAndDownload(urlInputField.text.trim())
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: rpcModal
        anchors.fill: parent
        color: "#b0000000"
        visible: false
        z: 9999

        MouseArea {
            anchors.fill: parent
            onClicked: rpcModal.visible = false
        }

        Rectangle {
            id: rpcModalBox
            anchors.centerIn: parent
            width: 440
            height: 210
            radius: 12
            color: root.cPanelBg
            border.color: root.cAccent
            border.width: 1

            ThemeAnimator { targetItem: rpcModalBox; targetName: "rpc_modal" }

            MouseArea { anchors.fill: parent }

            Column {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 12

                Item {
                    width: parent.width
                    height: 24

                    Text {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: "DISCORD RICH PRESENCE"
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.0
                    }

                    Button {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        text: "✕"
                        contentItem: Text {
                            text: parent.text
                            color: root.cTextDim
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle { color: parent.hovered ? "#ef4444" : "transparent"; radius: 12 }
                        onClicked: rpcModal.visible = false
                    }
                }

                Text {
                    text: "Application Client ID:"
                    color: root.cTextDim
                    font.family: root.cFont
                    font.pixelSize: 10
                    font.bold: true
                }

                Rectangle {
                    width: parent.width
                    height: 36
                    color: root.cSurface
                    radius: 6
                    border.color: root.cBorder

                    TextInput {
                        id: rpcIdInput
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        verticalAlignment: TextInput.AlignVCenter
                        color: root.cTextMain
                        font.family: root.cFont
                        font.pixelSize: 12
                        selectByMouse: true
                        text: discordRPC.clientId

                        Text {
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            text: "Paste Discord Application Client ID..."
                            color: root.cTextDim
                            font.family: root.cFont
                            font.pixelSize: 11
                            visible: parent.text === ""
                        }
                    }
                }

                Row {
                    width: parent.width
                    height: 20
                    spacing: 8

                    Text {
                        text: "Status:"
                        color: root.cTextDim
                        font.family: root.cFont
                        font.pixelSize: 11
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: discordRPC.statusText
                        color: discordRPC.isConnected ? "#10b981" : (discordRPC.statusText.indexOf("Connecting") !== -1 ? "#f59e0b" : "#ef4444")
                        font.family: root.cFont
                        font.pixelSize: 11
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Row {
                    anchors.right: parent.right
                    spacing: 10

                    Button {
                        height: 30
                        text: "Disconnect"
                        enabled: discordRPC.isConnected
                        contentItem: Text {
                            text: parent.text
                            color: discordRPC.isConnected ? root.cTextMain : root.cTextDim
                            font.family: root.cFont
                            font.pixelSize: 11
                        }
                        background: Rectangle { radius: 4; color: root.cSurface; border.color: root.cBorder }
                        onClicked: discordRPC.disconnectRpc()
                    }

                    Button {
                        height: 30
                        width: 100
                        text: "Connect"
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.family: root.cFont
                            font.pixelSize: 11
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: root.cAccent
                        }
                        onClicked: {
                            const id = rpcIdInput.text.trim()
                            if (id !== "") {
                                settingsStore.setDiscordClientId(id)
                                discordRPC.connectRpc(id)
                            }
                        }
                    }
                }
            }
        }
    }

    SettingsDialog {
        id: settingsDialog
    }
}