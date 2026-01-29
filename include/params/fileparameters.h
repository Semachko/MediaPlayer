#pragma once

#include <QString>

class FileParameters : public QObject {
        Q_OBJECT
    public:
        explicit FileParameters(QObject* parent = nullptr) :
                QObject(parent) {}
        Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(qreal globalTime READ getGlobalTime WRITE setGlobalTime NOTIFY globalTimeChanged)
        Q_PROPERTY(qreal timeStep READ getTimeStep WRITE setTimeStep NOTIFY timeStepChanged)
        QString getPath() const { return path; };
        QString getName() const { return name; };
        qreal getGlobalTime() const { return globalTime; };
        qreal getTimeStep() const { return timeStep; };
        void setPath(const QString& newPath) {
            path = newPath;
            emit pathChanged();
        };
        void setName(const QString& newName) {
            name = newName;
            emit nameChanged();
        };
        void setGlobalTime(const qreal newTime) {
            globalTime = newTime;
            emit globalTimeChanged();
        };
        void setTimeStep(const qreal newStep) {
            timeStep = newStep;
            emit timeStepChanged();
        };
    signals:
        void pathChanged();
        void nameChanged();
        void globalTimeChanged();
        void timeStepChanged();

    public:
        QString path;
        QString name = "CHOOSE FILE";
        std::atomic<qreal> globalTime;
        std::atomic<qreal> timeStep = 0.001;
};