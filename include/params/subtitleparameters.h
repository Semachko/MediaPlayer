#pragma once

#include <QObject>

class SubtitleParameters : public QObject {
        Q_OBJECT
    public:
        explicit SubtitleParameters(QObject* parent = nullptr) :
                QObject(parent) {}
        Q_PROPERTY(bool isToggle WRITE setIsToggle NOTIFY isToggleChanged)
        Q_PROPERTY(QStringList subtitles READ subtitles NOTIFY subtitlesChanged)
        Q_PROPERTY(qint64 index WRITE setIndex NOTIFY currentSubChanged)
        QStringList subtitles() const { return subs; }
        bool isToggle() const { return is_toggle; }
        void setIndex(qint64 newIndex) {
            index = newIndex;
            emit currentSubChanged();
        }
        void setIsToggle(bool isToggle) {
            is_toggle = isToggle;
            emit isToggleChanged();
        }
        void clear() {
            subs.clear();
            emit subtitlesChanged();
        }
        void set_subtitles(QStringList& new_subs) {
            subs = new_subs;
            emit subtitlesChanged();
            if (!subs.empty())
                setIndex(0);
        }
        void add_subs(QStringList& new_subs) {
            subs.append(new_subs);
            emit subtitlesChanged();
        }
        bool is_toggle = false;
        QStringList subs;
        qint64 index = -1;
    signals:
        void isToggleChanged();
        void subtitlesChanged();
        void currentSubChanged();
};