// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "mediacontext.h"
#include "media.h"

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
    engine.loadFromModule("MediaPlayer", "Main");

    // MediaContext* mediaplayer = new MediaContext();
    // engine.rootContext()->setContextProperty("mediaplayer", mediaplayer);
    Media* media = new Media();
    engine.rootContext()->setContextProperty("media", media);

    return app.exec();
}
