#include "timeline.h"
#include <algorithm>
#include <QJsonArray>

Timeline::Timeline(QObject *parent)
    : QObject(parent)
{
    // Default tracks
    Track videoTrack;
    videoTrack.name = "Video 1";
    videoTrack.type = Track::Video;
    m_tracks.append(videoTrack);

    Track captionTrack;
    captionTrack.name = "Captions";
    captionTrack.type = Track::Caption;
    m_tracks.append(captionTrack);

    Track audioTrack;
    audioTrack.name = "Audio 1";
    audioTrack.type = Track::Audio;
    m_tracks.append(audioTrack);
}

Track Timeline::track(int index) const
{
    if (index >= 0 && index < m_tracks.size())
        return m_tracks[index];
    return Track();
}

void Timeline::addTrack(const Track &track)
{
    m_tracks.append(track);
    emit trackAdded(m_tracks.size() - 1);
}

void Timeline::removeTrack(int index)
{
    if (index >= 0 && index < m_tracks.size()) {
        m_tracks.removeAt(index);
        emit trackRemoved(index);
    }
}

void Timeline::setTrackVisible(int index, bool visible)
{
    if (index >= 0 && index < m_tracks.size())
        m_tracks[index].visible = visible;
}

void Timeline::setTrackLocked(int index, bool locked)
{
    if (index >= 0 && index < m_tracks.size())
        m_tracks[index].locked = locked;
}

void Timeline::setTrackMuted(int index, bool muted)
{
    if (index >= 0 && index < m_tracks.size())
        m_tracks[index].muted = muted;
}

void Timeline::addClip(const Clip &clip)
{
    m_clips.append(clip);
    emit clipAdded(clip);
    emit durationChanged(duration());
}

void Timeline::removeClip(const QUuid &id)
{
    m_clips.erase(
        std::remove_if(m_clips.begin(), m_clips.end(),
                        [&id](const Clip &c) { return c.id() == id; }),
        m_clips.end());
    emit clipRemoved(id);
    emit durationChanged(duration());
}

void Timeline::moveClip(const QUuid &id, qint64 newStart, int newTrack)
{
    for (auto &clip : m_clips) {
        if (clip.id() == id) {
            clip.setStartTime(newStart);
            clip.setTrackIndex(newTrack);
            emit clipMoved(id);
            return;
        }
    }
}

Clip *Timeline::clipById(const QUuid &id)
{
    for (auto &clip : m_clips) {
        if (clip.id() == id)
            return &clip;
    }
    return nullptr;
}

QVector<Clip> Timeline::clipsOnTrack(int trackIndex) const
{
    QVector<Clip> result;
    for (const auto &clip : m_clips) {
        if (clip.trackIndex() == trackIndex)
            result.append(clip);
    }
    return result;
}

void Timeline::addCaption(const Caption &caption)
{
    m_captions.append(caption);
    emit captionAdded(caption);
}

void Timeline::removeCaption(const QUuid &id)
{
    m_captions.erase(
        std::remove_if(m_captions.begin(), m_captions.end(),
                        [&id](const Caption &c) { return c.id() == id; }),
        m_captions.end());
    emit captionRemoved(id);
}

Caption *Timeline::captionById(const QUuid &id)
{
    for (auto &cap : m_captions) {
        if (cap.id() == id)
            return &cap;
    }
    return nullptr;
}

QVector<Caption> Timeline::captionsAtTime(qint64 time) const
{
    QVector<Caption> result;
    for (const auto &cap : m_captions) {
        if (time >= cap.startTime() && time < cap.endTime())
            result.append(cap);
    }
    return result;
}

qint64 Timeline::duration() const
{
    qint64 maxEnd = 0;
    for (const auto &clip : m_clips)
        maxEnd = std::max(maxEnd, clip.endTime());
    for (const auto &cap : m_captions)
        maxEnd = std::max(maxEnd, cap.endTime());
    return std::max(maxEnd, qint64(30000)); // minimum 30s
}

void Timeline::setCurrentTime(qint64 ms)
{
    m_currentTime = std::max(qint64(0), ms);
    emit timeChanged(m_currentTime);
}

void Timeline::setZoom(qreal z)
{
    m_zoom = std::max(0.1, std::min(10.0, z));
    emit zoomChanged(m_zoom);
}

void Timeline::splitClipAtTime(const QUuid &clipId, qint64 time)
{
    Clip *clip = clipById(clipId);
    if (!clip)
        return;

    if (time <= clip->startTime() || time >= clip->endTime())
        return;

    qint64 originalEnd = clip->endTime();
    qint64 splitPoint = time - clip->startTime();

    // Resize original clip
    clip->setDuration(splitPoint);

    // Create new clip for the second half
    Clip newClip(clip->type(), clip->sourcePath());
    newClip.setStartTime(time);
    newClip.setDuration(originalEnd - time);
    newClip.setTrackIndex(clip->trackIndex());
    newClip.setName(clip->name() + " (split)");
    newClip.setVolume(clip->volume());
    newClip.setOpacity(clip->opacity());
    newClip.setTrimIn(clip->trimIn() + splitPoint);
    newClip.setTrimOut(clip->trimOut());

    addClip(newClip);
}

QJsonObject Timeline::toJson() const
{
    QJsonObject obj;

    QJsonArray tracksArr;
    for (const auto &t : m_tracks) {
        QJsonObject to;
        to["name"] = t.name;
        to["visible"] = t.visible;
        to["locked"] = t.locked;
        to["muted"] = t.muted;
        to["height"] = t.height;
        to["type"] = static_cast<int>(t.type);
        tracksArr.append(to);
    }
    obj["tracks"] = tracksArr;

    QJsonArray clipsArr;
    for (const auto &c : m_clips)
        clipsArr.append(c.toJson());
    obj["clips"] = clipsArr;

    QJsonArray capsArr;
    for (const auto &c : m_captions)
        capsArr.append(c.toJson());
    obj["captions"] = capsArr;

    obj["currentTime"] = m_currentTime;
    obj["zoom"] = m_zoom;
    obj["snapEnabled"] = m_snapEnabled;

    return obj;
}

Timeline *Timeline::fromJson(const QJsonObject &obj, QObject *parent)
{
    auto *tl = new Timeline(parent);
    tl->m_tracks.clear();

    for (const auto &val : obj["tracks"].toArray()) {
        QJsonObject to = val.toObject();
        Track t;
        t.name = to["name"].toString();
        t.visible = to["visible"].toBool(true);
        t.locked = to["locked"].toBool(false);
        t.muted = to["muted"].toBool(false);
        t.height = to["height"].toDouble(40.0);
        t.type = static_cast<Track::Type>(to["type"].toInt());
        tl->m_tracks.append(t);
    }

    for (const auto &val : obj["clips"].toArray())
        tl->m_clips.append(Clip::fromJson(val.toObject()));

    for (const auto &val : obj["captions"].toArray())
        tl->m_captions.append(Caption::fromJson(val.toObject()));

    tl->m_currentTime = static_cast<qint64>(obj["currentTime"].toDouble());
    tl->m_zoom = obj["zoom"].toDouble(1.0);
    tl->m_snapEnabled = obj["snapEnabled"].toBool(true);

    return tl;
}
