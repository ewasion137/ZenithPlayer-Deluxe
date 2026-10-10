#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "Player/library.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setOrganizationName("ZenithCollective");
    app.setApplicationName("ZenithPlayerDeluxe");

    Player::Library library;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("library", &library);

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