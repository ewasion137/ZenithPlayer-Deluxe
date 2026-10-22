import QtQuick

Item {
    id: animator
    visible: false
    width: 0
    height: 0

    property Item targetItem: parent
    property string targetName: ""
    property bool isHovered: false
    property bool isPressed: false
    property bool isDragging: false
    property bool isPlaying: (typeof audioEngine !== "undefined" && audioEngine) ? audioEngine.isPlaying : false

    Connections {
        target: themeManager
        function onThemeUpdated() { animator.rebindAnimations() }
    }

    Component.onCompleted: rebindAnimations()

    property var dynamicAnims: []

    function rebindAnimations() {
        for (var i = dynamicAnims.length - 1; i >= 0; i--) {
            try {
                if (dynamicAnims[i]) {
                    if (typeof dynamicAnims[i].stop === "function") dynamicAnims[i].stop()
                    dynamicAnims[i].destroy()
                }
            } catch (e) {}
        }
        dynamicAnims = []

        if (!targetItem || targetName === "") return

        if (targetName === "card_cover" || targetName === "card_eq") {
            setupHyprBorder()
            setupGlassSheen()
        }

        var anims = themeManager.getAnimationsForTarget(targetName)
        for (var j = 0; j < anims.length; j++) {
            createAnimation(anims[j])
        }
    }

    function setupHyprBorder() {
        if (!themeManager || !themeManager.borders || !themeManager.borders[targetName] || !targetItem) return
        var cfg = themeManager.borders[targetName]
        var colors = cfg.colors || [themeManager.accent, themeManager.accentSecondary]
        var bWidth = cfg.width || 2
        var radiusVal = cfg.radius || (targetItem.radius !== undefined ? targetItem.radius : 8)

        var borderQml = `
            import QtQuick
            Item {
                x: -${bWidth}
                y: -${bWidth}
                width: parent ? parent.width + ${bWidth * 2} : 0
                height: parent ? parent.height + ${bWidth * 2} : 0
                z: -1

                Rectangle {
                    anchors.fill: parent
                    radius: ${radiusVal + bWidth}
                    clip: true
                    color: "transparent"

                    Rectangle {
                        anchors.centerIn: parent
                        width: Math.max(parent.width, parent.height) * 2
                        height: width
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: "${colors[0]}" }
                            GradientStop { position: 0.5; color: "${colors[1 % colors.length]}" }
                            GradientStop { position: 1.0; color: "${colors[0]}" }
                        }
                        RotationAnimator on rotation {
                            from: 0; to: 360
                            duration: ${cfg.speed || 3500}
                            loops: Animation.Infinite
                            running: true
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: ${bWidth}
                        radius: ${radiusVal}
                        color: targetItem.color !== undefined ? targetItem.color : "#10121a"
                    }
                }
            }
        `
        try {
            var obj = Qt.createQmlObject(borderQml, targetItem, "hypr_border_" + targetName)
            if (obj) dynamicAnims.push(obj)
        } catch (e) {}
    }

    function setupGlassSheen() {
        if (!themeManager || !themeManager.effects || !themeManager.effects.sheen || !targetItem) return
        var sheenQml = `
            import QtQuick
            Item {
                x: 0
                y: 0
                width: parent ? parent.width : 0
                height: parent ? parent.height : 0
                clip: true
                z: 99

                Rectangle {
                    id: sheenBar
                    visible: sheenAnim.running
                    width: parent.width * 0.4
                    height: parent.height * 2.5
                    y: -parent.height * 0.5
                    x: -width * 2
                    rotation: 25
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "#00ffffff" }
                        GradientStop { position: 0.5; color: "#25ffffff" }
                        GradientStop { position: 1.0; color: "#00ffffff" }
                    }
                    NumberAnimation {
                        id: sheenAnim
                        target: sheenBar
                        property: "x"
                        from: -sheenBar.width * 2
                        to: parent.width * 1.5
                        duration: 650
                        easing.type: Easing.OutCubic
                    }
                }
                Connections {
                    target: animator
                    function onIsHoveredChanged() {
                        if (animator.isHovered) sheenAnim.restart()
                    }
                }
            }
        `
        try {
            var obj = Qt.createQmlObject(sheenQml, targetItem, "glass_sheen_" + targetName)
            if (obj) dynamicAnims.push(obj)
        } catch (e) {}
    }

    function createAnimation(cfg) {
        if (!targetItem || !cfg.property) return

        var prop = cfg.property.trim()
        var propLower = prop.toLowerCase().replace(/[-_]/g, "")
        var cleanProp = prop
        var isColor = false

        if (propLower === "color") {
            cleanProp = "color"
            isColor = true
        } else if (propLower === "bordercolor" || propLower === "border.color") {
            cleanProp = "border.color"
            isColor = true
        } else if (propLower === "borderwidth" || propLower === "border.width") {
            cleanProp = "border.width"
        } else if (propLower === "letterspacing") {
            cleanProp = "font.letterSpacing"
        } else if (propLower === "fontsize" || propLower === "pixelsize") {
            cleanProp = "font.pixelSize"
        } else if (propLower === "scale" || propLower === "rotation") {
            try { targetItem.transformOrigin = Item.Center } catch (e) {}
        }

        var duration = cfg.duration > 0 ? cfg.duration : 200
        var trigger = cfg.trigger ? cfg.trigger.toLowerCase().trim() : "none"
        var isInfinite = cfg.loop === true
        var easingStr = "Easing.OutBack"
        var bezierClause = ""

        if (cfg.bezier && cfg.bezier.length === 4) {
            easingStr = "Easing.BezierSpline"
            bezierClause = `easing.bezierCurve: [${cfg.bezier[0]}, ${cfg.bezier[1]}, ${cfg.bezier[2]}, ${cfg.bezier[3]}, 1.0, 1.0]`
        } else if (cfg.easing === "linear") {
            easingStr = "Easing.Linear"
        } else if (cfg.easing === "inoutquad") {
            easingStr = "Easing.InOutQuad"
        } else if (cfg.easing === "outcubic") {
            easingStr = "Easing.OutCubic"
        }

        var overshootClause = (easingStr === "Easing.OutBack") ? `easing.overshoot: ${cfg.overshoot || 1.45}` : ""
        var isReactive = (trigger === "hover" || trigger === "press" || trigger === "drag")

        if (isReactive) {
            var componentStr = `
                import QtQuick
                Item {
                    property var animTarget: animator.targetItem
                    ${isColor ? "ColorAnimation" : "NumberAnimation"} {
                        id: fwdAnim
                        target: animTarget
                        property: "${cleanProp}"
                        to: ${isColor ? '"' + cfg.to + '"' : Number(cfg.to)}
                        duration: ${duration}
                        easing.type: ${easingStr}
                        ${overshootClause}
                        ${bezierClause}
                    }
                    ${isColor ? "ColorAnimation" : "NumberAnimation"} {
                        id: bwdAnim
                        target: animTarget
                        property: "${cleanProp}"
                        to: ${isColor ? '"' + cfg.from + '"' : Number(cfg.from)}
                        duration: ${Math.floor(duration * 0.8)}
                        easing.type: Easing.OutCubic
                    }
                    function activate() { bwdAnim.stop(); fwdAnim.start(); }
                    function deactivate() { fwdAnim.stop(); bwdAnim.start(); }
                    function stop() { fwdAnim.stop(); bwdAnim.stop(); }
                }
            `

            try {
                var reactiveObj = Qt.createQmlObject(componentStr, animator, "rx_" + cfg.name.replace(/[^a-zA-Z0-9_]/g, "_"))
                if (reactiveObj && typeof reactiveObj.activate === "function") {
                    dynamicAnims.push(reactiveObj)

                    var handler = function() {
                        var state = (trigger === "press") ? animator.isPressed : ((trigger === "drag") ? animator.isDragging : animator.isHovered)
                        if (state) reactiveObj.activate()
                        else reactiveObj.deactivate()
                    }

                    if (trigger === "press") animator.isPressedChanged.connect(handler)
                    else if (trigger === "drag") animator.isDraggingChanged.connect(handler)
                    else animator.isHoveredChanged.connect(handler)
                }
            } catch (err) {}
            return
        }

        var loopVal = isInfinite ? "Animation.Infinite" : "1"
        var loopComponentStr = `
            import QtQuick
            Item {
                property var animTarget: animator.targetItem
                SequentialAnimation {
                    id: loopSeq
                    loops: ${loopVal}
                    ${isColor ? "ColorAnimation" : "NumberAnimation"} {
                        target: animTarget
                        property: "${cleanProp}"
                        from: ${isColor ? '"' + cfg.from + '"' : Number(cfg.from)}
                        to: ${isColor ? '"' + cfg.to + '"' : Number(cfg.to)}
                        duration: ${Math.floor(duration / 2)}
                        easing.type: ${easingStr}
                        ${overshootClause}
                        ${bezierClause}
                    }
                    ${isColor ? "ColorAnimation" : "NumberAnimation"} {
                        target: animTarget
                        property: "${cleanProp}"
                        from: ${isColor ? '"' + cfg.to + '"' : Number(cfg.to)}
                        to: ${isColor ? '"' + cfg.from + '"' : Number(cfg.from)}
                        duration: ${Math.floor(duration / 2)}
                        easing.type: ${easingStr}
                        ${overshootClause}
                        ${bezierClause}
                    }
                }
                function start() { loopSeq.restart() }
                function stop() { loopSeq.stop() }
            }
        `

        try {
            var loopObj = Qt.createQmlObject(loopComponentStr, animator, "loop_" + cfg.name.replace(/[^a-zA-Z0-9_]/g, "_"))
            if (loopObj) {
                dynamicAnims.push(loopObj)

                if (trigger === "playing") {
                    var playHandler = function() {
                        if (animator.isPlaying) loopObj.start()
                        else loopObj.stop()
                    }
                    animator.isPlayingChanged.connect(playHandler)
                    if (animator.isPlaying) loopObj.start()
                } else {
                    loopObj.start()
                }
            }
        } catch (err) {}
    }
}