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

    QObject::connect(&audioEngine, &Audio::Engine::trackEnded, [&]() {
        const QString next = library.getNextTrack();
        if (!next.isEmpty()) {
            library.setCurrentTrackPath(next);
            audioEngine.play(next);
        }
    });

    QObject::connect(&streamManager, &Network::StreamManager::streamReady,
                     [&](const QString& filePath, const QString& title, const QString& artist, const QString& coverPath, const QString& originalUrl) {
        settingsStore.addWebTrack(filePath, originalUrl, title, artist, coverPath);
        library.setCurrentTrackPath(filePath);
        library.setWebMetadata(title, artist, coverPath);
        audioEngine.play(filePath);
    });

    QQmlApplicationEngine engine;
    engine.addImageProvider("covers", new CoverProvider(&library));

    engine.rootContext()->setContextProperty("library", &library);
    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("settingsStore", &settingsStore);
    engine.rootContext()->setContextProperty("streamManager", &streamManager);

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