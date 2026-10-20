import QtQuick

Item {
    id: animator
    property Item targetItem: parent
    property string targetName: ""
    property bool isHovered: false
    property bool isPlaying: (typeof audioEngine !== "undefined" && audioEngine) ? audioEngine.isPlaying : false

    // Auto-update when theme changes
    Connections {
        target: themeManager
        function onThemeUpdated() {
            animator.rebindAnimations()
        }
    }

    Component.onCompleted: rebindAnimations()

    function rebindAnimations() {
        for (var i = dynamicAnims.length - 1; i >= 0; i--) {
            try {
                if (dynamicAnims[i]) {
                    dynamicAnims[i].stop()
                    dynamicAnims[i].destroy()
                }
            } catch (e) {}
        }
        dynamicAnims = []

        if (!targetItem || targetName === "") return

        var anims = themeManager.getAnimationsForTarget(targetName)
        for (var j = 0; j < anims.length; j++) {
            createAnimation(anims[j])
        }
    }

    property var dynamicAnims: []

    function getEasingString(easingName, fallback) {
        if (!easingName) return fallback || "Easing.InOutQuad"
        var e = easingName.toLowerCase().replace(/[-_]/g, "")
        switch (e) {
            case "linear": return "Easing.Linear"
            case "inquad": return "Easing.InQuad"
            case "outquad": return "Easing.OutQuad"
            case "inoutquad": return "Easing.InOutQuad"
            case "incubic": return "Easing.InCubic"
            case "outcubic": return "Easing.OutCubic"
            case "inoutcubic": return "Easing.InOutCubic"
            case "inquart": return "Easing.InQuart"
            case "outquart": return "Easing.OutQuart"
            case "inoutquart": return "Easing.InOutQuart"
            case "insine": return "Easing.InSine"
            case "outsine": return "Easing.OutSine"
            case "inoutsine": return "Easing.InOutSine"
            case "outback": case "back": return "Easing.OutBack"
            case "inoutback": return "Easing.InOutBack"
            case "inback": return "Easing.InBack"
            case "outbounce": case "bounce": return "Easing.OutBounce"
            case "inoutbounce": return "Easing.InOutBounce"
            case "outelastic": case "elastic": return "Easing.OutElastic"
            case "spring": return "Easing.OutBack"
            default: return fallback || "Easing.InOutQuad"
        }
    }

    function createAnimation(cfg) {
        if (!targetItem || !cfg.property) return

        var rawProp = cfg.property.trim()
        var propLower = rawProp.toLowerCase().replace(/[-_]/g, "")

        // Map user property name to QML property
        var qmlProp = rawProp
        var isColor = false
        if (propLower === "color") {
            qmlProp = "color"
            isColor = true
        } else if (propLower === "bordercolor" || propLower === "border.color") {
            qmlProp = "border.color"
            isColor = true
        } else if (propLower === "letterspacing") {
            qmlProp = "font.letterSpacing"
        } else if (propLower === "fontsize" || propLower === "pixelsize") {
            qmlProp = "font.pixelSize"
        } else if (propLower === "scale" || propLower === "rotation") {
            // Keep transform centered
            try { targetItem.transformOrigin = Item.Center } catch (e) {}
        }

        var fromVal = cfg.from
        var toVal = cfg.to
        var duration = cfg.duration > 0 ? cfg.duration : 1000
        var isInfinite = cfg.loop === true
        var trigger = cfg.trigger ? cfg.trigger.toLowerCase().trim() : "none"
        var direction = cfg.direction ? cfg.direction.toLowerCase().trim() : (propLower === "rotation" ? "forward" : "alternate")
        var delay = cfg.delay > 0 ? cfg.delay : 0

        var defaultEasing = isColor ? "Easing.InOutQuad" : (trigger === "hover" ? "Easing.OutBack" : "Easing.InOutQuad")
        var easingStr = getEasingString(cfg.easing, defaultEasing)

        var componentStr = ""

        // === HOVER TRIGGER (Smooth interactive spring/glide like Linear & macOS) ===
        if (trigger === "hover") {
            if (isColor) {
                componentStr = `
                    import QtQuick
                    Item {
                        id: animBox
                        property var animTarget: null

                        ColorAnimation {
                            id: enterAnim
                            target: animBox.animTarget
                            property: "${qmlProp}"
                            to: "${toVal}"
                            duration: ${duration}
                            easing.type: ${easingStr}
                        }
                        ColorAnimation {
                            id: exitAnim
                            target: animBox.animTarget
                            property: "${qmlProp}"
                            to: "${fromVal}"
                            duration: ${Math.min(duration, 250)}
                            easing.type: Easing.OutCubic
                        }

                        function onEnter() {
                            exitAnim.stop()
                            enterAnim.from = animBox.animTarget ? animBox.animTarget["${qmlProp}"] : "${fromVal}"
                            enterAnim.start()
                        }
                        function onExit() {
                            enterAnim.stop()
                            exitAnim.from = animBox.animTarget ? animBox.animTarget["${qmlProp}"] : "${toVal}"
                            exitAnim.start()
                        }
                        function stop() {
                            enterAnim.stop()
                            exitAnim.stop()
                        }
                    }
                `
            } else {
                componentStr = `
                    import QtQuick
                    Item {
                        id: animBox
                        property var animTarget: null

                        NumberAnimation {
                            id: enterAnim
                            target: animBox.animTarget
                            property: "${qmlProp}"
                            to: ${Number(toVal)}
                            duration: ${duration}
                            easing.type: ${easingStr}
                        }
                        NumberAnimation {
                            id: exitAnim
                            target: animBox.animTarget
                            property: "${qmlProp}"
                            to: ${Number(fromVal)}
                            duration: ${Math.min(duration, 250)}
                            easing.type: Easing.OutCubic
                        }

                        function onEnter() {
                            exitAnim.stop()
                            enterAnim.from = animBox.animTarget ? animBox.animTarget["${qmlProp}"] : ${Number(fromVal)}
                            enterAnim.start()
                        }
                        function onExit() {
                            enterAnim.stop()
                            exitAnim.from = animBox.animTarget ? animBox.animTarget["${qmlProp}"] : ${Number(toVal)}
                            exitAnim.start()
                        }
                        function stop() {
                            enterAnim.stop()
                            exitAnim.stop()
                        }
                    }
                `
            }
        }
        // === AMBIENT / LOOPING / PLAYING ANIMATIONS ===
        else {
            var loopVal = isInfinite ? "Animation.Infinite" : "1"

            if (isColor) {
                if (direction === "forward") {
                    componentStr = `
                        import QtQuick
                        Item {
                            id: animBox
                            property var animTarget: null
                            SequentialAnimation {
                                id: mainSeq
                                loops: ${loopVal}
                                ColorAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: "${fromVal}"
                                    to: "${toVal}"
                                    duration: ${duration}
                                    easing.type: ${easingStr}
                                }
                            }
                            function start() { mainSeq.restart() }
                            function stop() { mainSeq.stop() }
                        }
                    `
                } else {
                    // Alternate (ping-pong)
                    componentStr = `
                        import QtQuick
                        Item {
                            id: animBox
                            property var animTarget: null
                            SequentialAnimation {
                                id: mainSeq
                                loops: ${loopVal}
                                ColorAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: "${fromVal}"
                                    to: "${toVal}"
                                    duration: ${Math.floor(duration / 2)}
                                    easing.type: ${easingStr}
                                }
                                ColorAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: "${toVal}"
                                    to: "${fromVal}"
                                    duration: ${Math.floor(duration / 2)}
                                    easing.type: ${easingStr}
                                }
                            }
                            function start() { mainSeq.restart() }
                            function stop() { mainSeq.stop() }
                        }
                    `
                }
            } else {
                if (direction === "forward" || propLower === "rotation") {
                    componentStr = `
                        import QtQuick
                        Item {
                            id: animBox
                            property var animTarget: null
                            SequentialAnimation {
                                id: mainSeq
                                loops: ${loopVal}
                                NumberAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: ${Number(fromVal)}
                                    to: ${Number(toVal)}
                                    duration: ${duration}
                                    easing.type: ${easingStr}
                                }
                            }
                            function start() { mainSeq.restart() }
                            function stop() { mainSeq.stop() }
                        }
                    `
                } else {
                    // Alternate (ping-pong)
                    componentStr = `
                        import QtQuick
                        Item {
                            id: animBox
                            property var animTarget: null
                            SequentialAnimation {
                                id: mainSeq
                                loops: ${loopVal}
                                NumberAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: ${Number(fromVal)}
                                    to: ${Number(toVal)}
                                    duration: ${Math.floor(duration / 2)}
                                    easing.type: ${easingStr}
                                }
                                NumberAnimation {
                                    target: animBox.animTarget
                                    property: "${qmlProp}"
                                    from: ${Number(toVal)}
                                    to: ${Number(fromVal)}
                                    duration: ${Math.floor(duration / 2)}
                                    easing.type: ${easingStr}
                                }
                            }
                            function start() { mainSeq.restart() }
                            function stop() { mainSeq.stop() }
                        }
                    `
                }
            }
        }

        try {
            var animObj = Qt.createQmlObject(componentStr, animator, "dyn_" + cfg.name + "_" + Math.random().toString(36).substring(7))
            if (animObj) {
                animObj.animTarget = targetItem
                dynamicAnims.push(animObj)

                if (trigger === "hover") {
                    var hoverHandler = function() {
                        if (animator.isHovered) {
                            animObj.onEnter()
                        } else {
                            animObj.onExit()
                        }
                    }
                    animator.isHoveredChanged.connect(hoverHandler)
                    if (animator.isHovered) animObj.onEnter()
                } else if (trigger === "playing") {
                    var playHandler = function() {
                        if (animator.isPlaying) {
                            animObj.start()
                        } else {
                            animObj.stop()
                        }
                    }
                    animator.isPlayingChanged.connect(playHandler)
                    if (animator.isPlaying) animObj.start()
                } else {
                    // "none", "always", "loop"
                    animObj.start()
                }
            }
        } catch (err) {
            console.warn("Theme animation instantiation error for " + cfg.name + ":", err)
        }
    }
}