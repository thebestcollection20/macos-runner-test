#pragma once

#include <QString>
#include <QUuid>
#include <QJsonObject>
#include <QColor>

enum class ClipType {
    Video,
    Audio,
    Image,
    Caption,
    Transition
};

class Clip {
public:
    Clip();
    Clip(ClipType type, const QString &sourcePath = QString());

    QUuid id() const { return m_id; }
    ClipType type() const { return m_type; }
    QString sourcePath() const { return m_sourcePath; }
    QString name() const { return m_name; }

    qint64 startTime() const { return m_startTime; }
    void setStartTime(qint64 ms) { m_startTime = ms; }

    qint64 duration() const { return m_duration; }
    void setDuration(qint64 ms) { m_duration = ms; }

    qint64 endTime() const { return m_startTime + m_duration; }

    int trackIndex() const { return m_trackIndex; }
    void setTrackIndex(int idx) { m_trackIndex = idx; }

    void setName(const QString &name) { m_name = name; }
    void setSourcePath(const QString &path) { m_sourcePath = path; }

    qreal volume() const { return m_volume; }
    void setVolume(qreal v) { m_volume = v; }

    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal o) { m_opacity = o; }

    QColor color() const { return m_color; }
    void setColor(const QColor &c) { m_color = c; }

    // Trim points (in-point / out-point within source)
    qint64 trimIn() const { return m_trimIn; }
    void setTrimIn(qint64 ms) { m_trimIn = ms; }

    qint64 trimOut() const { return m_trimOut; }
    void setTrimOut(qint64 ms) { m_trimOut = ms; }

    bool isSelected() const { return m_selected; }
    void setSelected(bool s) { m_selected = s; }

    QJsonObject toJson() const;
    static Clip fromJson(const QJsonObject &obj);

private:
    QUuid m_id;
    ClipType m_type = ClipType::Video;
    QString m_sourcePath;
    QString m_name;
    qint64 m_startTime = 0;
    qint64 m_duration = 5000; // 5 seconds default
    int m_trackIndex = 0;
    qreal m_volume = 1.0;
    qreal m_opacity = 1.0;
    QColor m_color;
    qint64 m_trimIn = 0;
    qint64 m_trimOut = 0;
    bool m_selected = false;
};
