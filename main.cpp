#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "player.h"
int main(int argc, char *argv[])
{
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

    QUrl fileToOpen;
    if (argc > 1){
        fileToOpen = QUrl::fromLocalFile(QString::fromLocal8Bit(argv[1]).trimmed());
        qDebug()<<fileToOpen;
        player->setFile(fileToOpen);
    }

    return app.exec();
}
