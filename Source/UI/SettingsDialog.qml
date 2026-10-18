import QtQuick
import QtQuick.Controls

Rectangle {
    id: dialogRoot
    anchors.fill: parent
    color: "#b0000000"
    visible: false
    z: 9999

    MouseArea {
        anchors.fill: parent
        onClicked: dialogRoot.visible = false
    }

    Rectangle {
        id: modalBox
        anchors.centerIn: parent
        width: 500
        height: 560
        radius: 12
        color: "#10121a"
        border.color: "#1e2235"
        border.width: 1

        MouseArea { anchors.fill: parent }

        Column {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 12

            Item {
                width: parent.width
                height: 26

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: "ZENITH DELUXE PREFERENCES"
                    color: "#8b5cf6"
                    font.pixelSize: 12
                    font.bold: true
                    font.letterSpacing: 1.5
                }

                Button {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    text: "✕"
                    contentItem: Text {
                        text: parent.text
                        color: "#717b94"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.hovered ? "#ef4444" : "transparent"
                        radius: 12
                    }
                    onClicked: dialogRoot.visible = false
                }
            }

            Rectangle { width: parent.width; height: 1; color: "#1e2235" }

            ScrollView {
                width: parent.width
                height: parent.height - 50
                clip: true

                Column {
                    width: parent.width - 12
                    spacing: 10

                    Text { text: "AUDIO & ENGINE"; color: "#717b94"; font.pixelSize: 10; font.bold: true }

                    component SettingSwitch: Rectangle {
                        id: switchRow
                        property string title: ""
                        property string subtitle: ""
                        property bool checkedState: false
                        signal switched(bool val)

                        width: parent.width
                        height: 38
                        radius: 6
                        color: rowArea.containsMouse ? "#161824" : "transparent"

                        MouseArea {
                            id: rowArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: switchRow.switched(!switchRow.checkedState)
                        }

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            spacing: 12

                            Rectangle {
                                width: 36
                                height: 20
                                radius: 10
                                color: switchRow.checkedState ? "#8b5cf6" : "#1e2235"
                                border.color: switchRow.checkedState ? "#8b5cf6" : "#2d334d"
                                anchors.verticalCenter: parent.verticalCenter

                                Rectangle {
                                    width: 14
                                    height: 14
                                    radius: 7
                                    color: "#ffffff"
                                    anchors.verticalCenter: parent.verticalCenter
                                    x: switchRow.checkedState ? parent.width - width - 3 : 3
                                    Behavior on x { NumberAnimation { duration: 100 } }
                                }
                            }

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 1
                                Text { text: switchRow.title; color: "#f8fafc"; font.pixelSize: 11; font.bold: true }
                                Text { text: switchRow.subtitle; color: "#717b94"; font.pixelSize: 9 }
                            }
                        }
                    }

                    SettingSwitch {
                        title: "FL Studio Return Mode"
                        subtitle: "Spacebar jumps back to anchor position and stops"
                        checkedState: settingsStore.flStudioMode
                        onSwitched: (v) => settingsStore.flStudioMode = v
                    }

                    SettingSwitch {
                        title: "Auto-Play Next Track"
                        subtitle: "Automatically play next track when current finishes"
                        checkedState: settingsStore.autoAdvance
                        onSwitched: (v) => settingsStore.autoAdvance = v
                    }

                    Rectangle { width: parent.width; height: 1; color: "#181a26" }

                    Text { text: "VISUALS & PERFORMANCE (LOW-GFX)"; color: "#717b94"; font.pixelSize: 10; font.bold: true }

                    SettingSwitch {
                        title: "Background FFT Spectrogram"
                        subtitle: "Real-time spectrum bars (disable for 0% GPU)"
                        checkedState: settingsStore.showSpectrogram
                        onSwitched: (v) => settingsStore.showSpectrogram = v
                    }

                    Item {
                        width: parent.width
                        height: 36

                        Column {
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2
                            Text { text: "Spectrogram Frame Rate Cap"; color: "#f8fafc"; font.pixelSize: 11; font.bold: true }
                            Text { text: "Limit visualizer refresh rate to save power"; color: "#717b94"; font.pixelSize: 9 }
                        }

                        Row {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 4

                            Repeater {
                                model: [30, 60, 120]

                                delegate: Button {
                                    width: 44
                                    height: 24
                                    text: modelData + ""
                                    contentItem: Text {
                                        text: parent.text
                                        color: settingsStore.spectrogramFps === modelData ? "#ffffff" : "#717b94"
                                        font.pixelSize: 10
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    background: Rectangle {
                                        radius: 4
                                        color: settingsStore.spectrogramFps === modelData ? "#8b5cf6" : "#181a26"
                                        border.color: settingsStore.spectrogramFps === modelData ? "#8b5cf6" : "#2d334d"
                                    }
                                    onClicked: settingsStore.spectrogramFps = modelData
                                }
                            }
                        }
                    }

                    SettingSwitch {
                        title: "Waveform Seek Bar"
                        subtitle: "Draw audio amplitude peaks on timeline"
                        checkedState: settingsStore.showWaveform
                        onSwitched: (v) => settingsStore.showWaveform = v
                    }

                    SettingSwitch {
                        title: "Ambient Cover Glow"
                        subtitle: "Subtle backdrop glow behind album artwork"
                        checkedState: settingsStore.showCoverGlow
                        onSwitched: (v) => settingsStore.showCoverGlow = v
                    }

                    Rectangle { width: parent.width; height: 1; color: "#181a26" }

                    Text { text: "SYSTEM & INTEGRATIONS"; color: "#717b94"; font.pixelSize: 10; font.bold: true }

                    SettingSwitch {
                        title: "Discord Rich Presence"
                        subtitle: "Broadcast track, artist and progress to Discord"
                        checkedState: settingsStore.enableDiscordRpc
                        onSwitched: (v) => settingsStore.enableDiscordRpc = v
                    }

                    SettingSwitch {
                        title: "Global System Hotkeys"
                        subtitle: "Alt+P, Alt+Arrows and Media keys work in background"
                        checkedState: settingsStore.globalHotkeys
                        onSwitched: (v) => settingsStore.globalHotkeys = v
                    }

                    SettingSwitch {
                        title: "Auto-Save Track Profiles"
                        subtitle: "Remember Volume, Speed and EQ per file in JSON"
                        checkedState: settingsStore.autoSaveTracks
                        onSwitched: (v) => settingsStore.autoSaveTracks = v
                    }

                    Item { width: parent.width; height: 4 }

                    Button {
                        width: parent.width
                        height: 30
                        text: "Clear All Track Profiles (Reset JSON)"
                        contentItem: Text {
                            text: parent.text
                            color: "#ef4444"
                            font.pixelSize: 10
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            radius: 4
                            color: "#181a26"
                            border.color: parent.hovered ? "#ef4444" : "#2d334d"
                        }
                        onClicked: {
                            settingsStore.clearAllTrackSettings()
                            audioEngine.setVolume(1.0)
                            audioEngine.setSpeed(1.0)
                            audioEngine.resetEq()
                        }
                    }
                    Rectangle { width: parent.width; height: 1; color: "#181a26" }

                    Text { text: "THEMES & VISUAL CUSTOMIZATION"; color: "#717b94"; font.pixelSize: 10; font.bold: true }

                    Row {
                        width: parent.width
                        spacing: 8

                        Button {
                            height: 28
                            text: "📂 Open Themes Folder"
                            contentItem: Text { text: parent.text; color: "#f8fafc"; font.pixelSize: 10; font.bold: true }
                            background: Rectangle { radius: 4; color: parent.hovered ? "#8b5cf6" : "#181a26"; border.color: "#2d334d" }
                            onClicked: themeManager.openThemesFolder()
                        }

                        Button {
                            height: 28
                            text: "↻ Reload"
                            contentItem: Text { text: parent.text; color: "#717b94"; font.pixelSize: 10 }
                            background: Rectangle { radius: 4; color: "#181a26"; border.color: "#2d334d" }
                            onClicked: themeManager.refreshThemes()
                        }
                    }

                    Column {
                        width: parent.width
                        spacing: 4

                        Text { text: "Installed Themes (.zss):"; color: "#717b94"; font.pixelSize: 9 }

                        Repeater {
                            model: themeManager.availableThemes

                            delegate: Rectangle {
                                width: parent.width
                                height: 26
                                radius: 4
                                color: themeManager.currentTheme === modelData ? "#251b42" : (themeRowArea.containsMouse ? "#161824" : "transparent")
                                border.color: themeManager.currentTheme === modelData ? "#8b5cf6" : "transparent"

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData
                                    color: themeManager.currentTheme === modelData ? "#ffffff" : "#717b94"
                                    font.pixelSize: 10
                                    font.bold: themeManager.currentTheme === modelData
                                }

                                MouseArea {
                                    id: themeRowArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: themeManager.applyTheme(modelData)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}