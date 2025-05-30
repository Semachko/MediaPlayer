// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

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

    QString fileToOpen;
    if (argc > 1){
        fileToOpen = QString::fromLocal8Bit(argv[1]);
        player->setFile(fileToOpen);
    }

    return app.exec();
}
