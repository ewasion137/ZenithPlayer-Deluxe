#include <QApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QQuickStyle>

#include "Player/library.hpp"
#include "Audio/engine.hpp"
#include "Core/settings.hpp"
#include "Network/stream.hpp"
#include "Network/discord.hpp"
#include "Utils/hotkeys.hpp"

class CoverProvider : public QQuickImageProvider {
public:
    CoverProvider(Player::Library* lib)
        : QQuickImageProvider(QQuickImageProvider::Image), m_library(lib) {}

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override {
        Q_UNUSED(id);
        QImage img = m_library->getCoverImage(id);
        if (size) {
            *size = img.size();
        }
        if (requestedSize.width() > 0 && requestedSize.height() > 0 && !img.isNull()) {
            return img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        return img;
    }

private:
    Player::Library* m_library;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath(QLibraryInfo::path(QLibraryInfo::PluginsPath));

    app.setOrganizationName("ZenithCollective");
    app.setApplicationName("ZenithPlayerDeluxe");

    QQuickStyle::setStyle("Basic");

    Player::Library library;
    Audio::Engine audioEngine;
    Core::Settings settingsStore;
    Network::StreamManager streamManager;
    Network::DiscordRPC discordRPC;
    Utils::GlobalHotkeys globalHotkeys;

    // Синхронизация тумблеров настроек со звуковым движком и сервисами
    audioEngine.setFlStudioMode(settingsStore.flStudioMode());
    audioEngine.setFpsCap(settingsStore.spectrogramFps());

    QObject::connect(&settingsStore, &Core::Settings::appSettingsChanged, [&]() {
        audioEngine.setFlStudioMode(settingsStore.flStudioMode());
        audioEngine.setFpsCap(settingsStore.spectrogramFps());
        if (!settingsStore.enableDiscordRpc()) {
            discordRPC.clearActivity();
        }
    });

    auto syncDiscordActivity = [&]() {
        if (!settingsStore.enableDiscordRpc() || !discordRPC.isConnected()) return;
        if (library.currentTrackPath().isEmpty()) {
            discordRPC.clearActivity();
            return;
        }

        discordRPC.updateActivity(
            library.currentTitle(),
            library.currentArtist(),
            library.currentAlbum(),
            audioEngine.duration(),
            audioEngine.position(),
            audioEngine.speed(),
            audioEngine.isPlaying(),
            library.currentOnlineCoverUrl()
        );
    };

    QObject::connect(&audioEngine, &Audio::Engine::stateChanged, syncDiscordActivity);
    QObject::connect(&audioEngine, &Audio::Engine::speedChanged, syncDiscordActivity);
    QObject::connect(&library, &Player::Library::currentTrackPathChanged, syncDiscordActivity);

    // Автоподключение сохраненного Discord Client ID при старте
    const QString savedId = settingsStore.getDiscordClientId();
    if (!savedId.isEmpty() && settingsStore.enableDiscordRpc()) {
        discordRPC.setClientId(savedId);
        discordRPC.connectRpc(savedId);
    }

    // Автопереход на следующий трек с проверкой настройки autoAdvance
    QObject::connect(&audioEngine, &Audio::Engine::trackEnded, [&]() {
        if (!settingsStore.autoAdvance()) return;
        const QString next = library.getNextTrack();
        if (!next.isEmpty()) {
            library.setCurrentTrackPath(next);
            audioEngine.play(next);
        }
    });

    // Глобальные хоткеи Windows с проверкой флага globalHotkeys
    QObject::connect(&globalHotkeys, &Utils::GlobalHotkeys::playPauseTriggered, [&]() {
        if (settingsStore.globalHotkeys()) {
            audioEngine.togglePlayPause();
        }
    });

    QObject::connect(&globalHotkeys, &Utils::GlobalHotkeys::nextTriggered, [&]() {
        if (!settingsStore.globalHotkeys()) return;
        const QString next = library.getNextTrack();
        if (!next.isEmpty()) {
            library.setCurrentTrackPath(next);
            audioEngine.play(next);
        }
    });

    QObject::connect(&globalHotkeys, &Utils::GlobalHotkeys::prevTriggered, [&]() {
        if (!settingsStore.globalHotkeys()) return;
        if (audioEngine.position() > 3.0) {
            audioEngine.seek(0.0);
        } else {
            const QString prev = library.getPreviousTrack();
            if (!prev.isEmpty()) {
                library.setCurrentTrackPath(prev);
                audioEngine.play(prev);
            }
        }
    });

    // Обработка готовности сетевого стрима с YouTube
    QObject::connect(&streamManager, &Network::StreamManager::streamReady,
                     [&](const QString& filePath, const QString& title, const QString& artist, const QString& coverPath, const QString& originalUrl, const QString& remoteCoverUrl) {
        settingsStore.addWebTrack(filePath, originalUrl, title, artist, remoteCoverUrl);
        library.setCurrentTrackPath(filePath);
        library.setWebMetadata(title, artist, coverPath, remoteCoverUrl);
        audioEngine.play(filePath);
    });

    QQmlApplicationEngine engine;
    engine.addImageProvider("covers", new CoverProvider(&library));

    engine.rootContext()->setContextProperty("library", &library);
    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("settingsStore", &settingsStore);
    engine.rootContext()->setContextProperty("streamManager", &streamManager);
    engine.rootContext()->setContextProperty("discordRPC", &discordRPC);

    const QUrl url(QStringLiteral("qrc:/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}