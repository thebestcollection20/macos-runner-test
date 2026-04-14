#include "clip.h"
#include <QFileInfo>

Clip::Clip()
    : m_id(QUuid::createUuid())
{
}

Clip::Clip(ClipType type, const QString &sourcePath)
    : m_id(QUuid::createUuid())
    , m_type(type)
    , m_sourcePath(sourcePath)
{
    if (!sourcePath.isEmpty()) {
        m_name = QFileInfo(sourcePath).fileName();
    } else {
        switch (type) {
        case ClipType::Video:    m_name = "Video Clip"; break;
        case ClipType::Audio:    m_name = "Audio Clip"; break;
        case ClipType::Image:    m_name = "Image"; break;
        case ClipType::Caption:  m_name = "Caption"; break;
        case ClipType::Transition: m_name = "Transition"; break;
        }
    }
}

QJsonObject Clip::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["type"] = static_cast<int>(m_type);
    obj["sourcePath"] = m_sourcePath;
    obj["name"] = m_name;
    obj["startTime"] = m_startTime;
    obj["duration"] = m_duration;
    obj["trackIndex"] = m_trackIndex;
    obj["volume"] = m_volume;
    obj["opacity"] = m_opacity;
    obj["color"] = m_color.name();
    obj["trimIn"] = m_trimIn;
    obj["trimOut"] = m_trimOut;
    return obj;
}

Clip Clip::fromJson(const QJsonObject &obj)
{
    Clip clip;
    clip.m_id = QUuid::fromString(obj["id"].toString());
    clip.m_type = static_cast<ClipType>(obj["type"].toInt());
    clip.m_sourcePath = obj["sourcePath"].toString();
    clip.m_name = obj["name"].toString();
    clip.m_startTime = static_cast<qint64>(obj["startTime"].toDouble());
    clip.m_duration = static_cast<qint64>(obj["duration"].toDouble());
    clip.m_trackIndex = obj["trackIndex"].toInt();
    clip.m_volume = obj["volume"].toDouble(1.0);
    clip.m_opacity = obj["opacity"].toDouble(1.0);
    clip.m_color = QColor(obj["color"].toString());
    clip.m_trimIn = static_cast<qint64>(obj["trimIn"].toDouble());
    clip.m_trimOut = static_cast<qint64>(obj["trimOut"].toDouble());
    return clip;
}
