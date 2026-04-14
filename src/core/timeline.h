#pragma once

#include "clip.h"
#include "caption.h"
#include <QObject>
#include <QVector>
#include <QJsonObject>

struct Track {
    QString name;
    bool visible = true;
    bool locked = false;
    bool muted = false;
    qreal height = 40.0;
    enum Type { Video, Audio, Caption } type = Video;
};

class Timeline : public QObject {
    Q_OBJECT

public:
    explicit Timeline(QObject *parent = nullptr);

    // Tracks
    int trackCount() const { return m_tracks.size(); }
    Track track(int index) const;
    void addTrack(const Track &track);
    void removeTrack(int index);
    void setTrackVisible(int index, bool visible);
    void setTrackLocked(int index, bool locked);
    void setTrackMuted(int index, bool muted);

    // Clips
    QVector<Clip> clips() const { return m_clips; }
    void addClip(const Clip &clip);
    void removeClip(const QUuid &id);
    void moveClip(const QUuid &id, qint64 newStart, int newTrack);
    Clip *clipById(const QUuid &id);
    QVector<Clip> clipsOnTrack(int trackIndex) const;
    void deselectAllClips();

    // Captions
    QVector<Caption> captions() const { return m_captions; }
    void addCaption(const Caption &caption);
    void removeCaption(const QUuid &id);
    Caption *captionById(const QUuid &id);
    QVector<Caption> captionsAtTime(qint64 time) const;
    void deselectAllCaptions();

    // Playback
    qint64 duration() const;
    qint64 currentTime() const { return m_currentTime; }
    void setCurrentTime(qint64 ms);

    // Zoom
    qreal zoom() const { return m_zoom; }
    void setZoom(qreal z);

    // Snap
    bool snapEnabled() const { return m_snapEnabled; }
    void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }

    // Split clip at current time
    void splitClipAtTime(const QUuid &clipId, qint64 time);

    // Serialization
    QJsonObject toJson() const;
    static Timeline *fromJson(const QJsonObject &obj, QObject *parent = nullptr);

signals:
    void timeChanged(qint64 time);
    void clipAdded(const Clip &clip);
    void clipRemoved(const QUuid &id);
    void clipMoved(const QUuid &id);
    void captionAdded(const Caption &caption);
    void captionRemoved(const QUuid &id);
    void trackAdded(int index);
    void trackRemoved(int index);
    void zoomChanged(qreal zoom);
    void durationChanged(qint64 duration);

private:
    QVector<Track> m_tracks;
    QVector<Clip> m_clips;
    QVector<Caption> m_captions;
    qint64 m_currentTime = 0;
    qreal m_zoom = 1.0;
    bool m_snapEnabled = true;
};
