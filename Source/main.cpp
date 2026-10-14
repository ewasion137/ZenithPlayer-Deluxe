#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QQuickStyle>

#include "Player/library.hpp"
#include "Audio/engine.hpp"
#include "Core/settings.hpp"

class CoverProvider : public QQuickImageProvider {
public:
    CoverProvider(Player::Library* lib)
        : QQuickImageProvider(QQuickImageProvider::Image), m_library(lib) {}

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override {
        Q_UNUSED(id);
        QImage img = m_library->getCoverImage(id);
        if (size) *size = img.size();
        if (requestedSize.isValid() && !img.isNull()) {
            return img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        return img;
    }

private:
    Player::Library* m_library;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setOrganizationName("ZenithCollective");
    app.setApplicationName("ZenithPlayerDeluxe");

    QQuickStyle::setStyle("Basic");

    Player::Library library;
    Audio::Engine audioEngine;
    Core::Settings settingsStore;

    QObject::connect(&audioEngine, &Audio::Engine::trackEnded, [&]() {
        const QString next = library.getNextTrack();
        if (!next.isEmpty()) {
            library.setCurrentTrackPath(next);
            audioEngine.play(next);
        }
    });

    QQmlApplicationEngine engine;
    engine.addImageProvider("covers", new CoverProvider(&library));

    engine.rootContext()->setContextProperty("library", &library);
    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);
    engine.rootContext()->setContextProperty("settingsStore", &settingsStore);

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