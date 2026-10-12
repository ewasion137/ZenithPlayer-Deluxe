#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "Player/library.hpp"
#include "Audio/engine.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setOrganizationName("ZenithCollective");
    app.setApplicationName("ZenithPlayerDeluxe");

    QQuickStyle::setStyle("Basic");

    Player::Library library;
    Audio::Engine audioEngine;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("library", &library);
    engine.rootContext()->setContextProperty("audioEngine", &audioEngine);

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