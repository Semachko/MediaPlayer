#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "media/player.h"
int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    Player* player = new Player();
    engine.rootContext()->setContextProperty("player", player);
    engine.loadFromModule("MediaPlayer", "Main");

    if (app.arguments().size() > 1)
        player->setFiles(app.arguments().mid(1));

    return app.exec();
}
