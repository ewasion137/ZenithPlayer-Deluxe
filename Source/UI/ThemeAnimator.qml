import QtQuick

Item {
    id: animator
    property Item targetItem: parent
    property string targetName: ""
    property bool isHovered: false
    property bool isPlaying: audioEngine.isPlaying

    // Автоматическая реакция на смену темы
    Connections {
        target: themeManager
        function onThemeUpdated() {
            animator.rebindAnimations()
        }
    }

    Component.onCompleted: rebindAnimations()

    function rebindAnimations() {
        // Очищаем старые динамические анимации
        for (var i = dynamicAnims.length - 1; i >= 0; i--) {
            dynamicAnims[i].stop()
            dynamicAnims[i].destroy()
        }
        dynamicAnims = []

        var anims = themeManager.getAnimationsForTarget(targetName)
        for (var j = 0; j < anims.length; j++) {
            createAnimation(anims[j])
        }
    }

    property var dynamicAnims: []

    function createAnimation(cfg) {
        if (!targetItem || !cfg.property) return

        var prop = cfg.property.toLowerCase()
        var isColor = (prop === "color" || prop === "border.color")
        var animQml = ""

        var fromVal = cfg.from
        var toVal = cfg.to
        var duration = cfg.duration > 0 ? cfg.duration : 1000
        var isInfinite = cfg.loop === true
        var trigger = cfg.trigger ? cfg.trigger.toLowerCase() : "none"

        var componentStr = ""

        if (isColor) {
            componentStr = `
                import QtQuick
                SequentialAnimation {
                    loops: ${isInfinite ? "Animation.Infinite" : "1"}
                    ColorAnimation {
                        target: targetItem
                        property: "${cfg.property}"
                        from: "${fromVal}"
                        to: "${toVal}"
                        duration: ${duration / 2}
                        easing.type: Easing.InOutQuad
                    }
                    ColorAnimation {
                        target: targetItem
                        property: "${cfg.property}"
                        from: "${toVal}"
                        to: "${fromVal}"
                        duration: ${duration / 2}
                        easing.type: Easing.InOutQuad
                    }
                }
            `
        } else if (prop === "rotation" && isInfinite) {
            // Для бесконечного вращения (например, пластинки/обложки)
            componentStr = `
                import QtQuick
                NumberAnimation {
                    target: targetItem
                    property: "${cfg.property}"
                    from: ${Number(fromVal)}
                    to: ${Number(toVal)}
                    duration: ${duration}
                    loops: Animation.Infinite
                }
            `
        } else {
            componentStr = `
                import QtQuick
                SequentialAnimation {
                    loops: ${isInfinite ? "Animation.Infinite" : "1"}
                    NumberAnimation {
                        target: targetItem
                        property: "${cfg.property}"
                        from: ${Number(fromVal)}
                        to: ${Number(toVal)}
                        duration: ${duration / 2}
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: targetItem
                        property: "${cfg.property}"
                        from: ${Number(toVal)}
                        to: ${Number(fromVal)}
                        duration: ${duration / 2}
                        easing.type: Easing.InOutQuad
                    }
                }
            `
        }

        var animObj = Qt.createQmlObject(componentStr, animator, "dynamicAnim_" + cfg.name)
        if (animObj) {
            dynamicAnims.push(animObj)

            // Управление триггерами:
            if (trigger === "playing") {
                animator.isPlayingChanged.connect(function() {
                    if (animator.isPlaying) animObj.start()
                    else animObj.stop()
                })
                if (animator.isPlaying) animObj.start()
            } else if (trigger === "hover") {
                animator.isHoveredChanged.connect(function() {
                    if (animator.isHovered) animObj.start()
                    else animObj.stop()
                })
            } else {
                // Безусловный запуск
                animObj.start()
            }
        }
    }
}