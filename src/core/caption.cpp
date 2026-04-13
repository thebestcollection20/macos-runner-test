#include "caption.h"

// CaptionStyle serialization
QJsonObject CaptionStyle::toJson() const
{
    QJsonObject obj;
    obj["fontFamily"] = font.family();
    obj["fontSize"] = font.pointSize();
    obj["fontWeight"] = font.weight();
    obj["textColor"] = textColor.name(QColor::HexArgb);
    obj["backgroundColor"] = backgroundColor.name(QColor::HexArgb);
    obj["outlineColor"] = outlineColor.name(QColor::HexArgb);
    obj["outlineWidth"] = outlineWidth;
    obj["shadowColor"] = shadowColor.name(QColor::HexArgb);
    obj["shadowOffsetX"] = shadowOffsetX;
    obj["shadowOffsetY"] = shadowOffsetY;
    obj["shadowBlur"] = shadowBlur;
    obj["showBackground"] = showBackground;
    obj["backgroundPadding"] = backgroundPadding;
    obj["backgroundRadius"] = backgroundRadius;
    obj["alignment"] = alignment;
    obj["lineSpacing"] = lineSpacing;
    obj["letterSpacing"] = letterSpacing;
    obj["allCaps"] = allCaps;
    obj["italic"] = italic;
    obj["underline"] = underline;
    return obj;
}

CaptionStyle CaptionStyle::fromJson(const QJsonObject &obj)
{
    CaptionStyle s;
    s.font = QFont(obj["fontFamily"].toString("Arial"),
                   obj["fontSize"].toInt(32),
                   obj["fontWeight"].toInt(QFont::Bold));
    s.textColor = QColor(obj["textColor"].toString("#ffffffff"));
    s.backgroundColor = QColor(obj["backgroundColor"].toString("#b4000000"));
    s.outlineColor = QColor(obj["outlineColor"].toString("#ff000000"));
    s.outlineWidth = obj["outlineWidth"].toDouble(2.0);
    s.shadowColor = QColor(obj["shadowColor"].toString("#80000000"));
    s.shadowOffsetX = obj["shadowOffsetX"].toDouble(2.0);
    s.shadowOffsetY = obj["shadowOffsetY"].toDouble(2.0);
    s.shadowBlur = obj["shadowBlur"].toDouble(4.0);
    s.showBackground = obj["showBackground"].toBool(true);
    s.backgroundPadding = obj["backgroundPadding"].toDouble(8.0);
    s.backgroundRadius = obj["backgroundRadius"].toDouble(4.0);
    s.alignment = obj["alignment"].toInt(Qt::AlignHCenter | Qt::AlignBottom);
    s.lineSpacing = obj["lineSpacing"].toDouble(1.2);
    s.letterSpacing = obj["letterSpacing"].toDouble(0.0);
    s.allCaps = obj["allCaps"].toBool(false);
    s.italic = obj["italic"].toBool(false);
    s.underline = obj["underline"].toBool(false);
    return s;
}

// Caption
Caption::Caption()
    : m_id(QUuid::createUuid())
{
}

Caption::Caption(const QString &text, qint64 startTime, qint64 duration)
    : m_id(QUuid::createUuid())
    , m_text(text)
    , m_startTime(startTime)
    , m_duration(duration)
{
}

QJsonObject Caption::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["text"] = m_text;
    obj["startTime"] = m_startTime;
    obj["duration"] = m_duration;
    obj["posX"] = m_position.x();
    obj["posY"] = m_position.y();
    obj["style"] = m_style.toJson();
    obj["animIn"] = static_cast<int>(m_animIn);
    obj["animOut"] = static_cast<int>(m_animOut);
    obj["animDuration"] = m_animDuration;
    obj["trackIndex"] = m_trackIndex;
    return obj;
}

Caption Caption::fromJson(const QJsonObject &obj)
{
    Caption cap;
    cap.m_id = QUuid::fromString(obj["id"].toString());
    cap.m_text = obj["text"].toString();
    cap.m_startTime = static_cast<qint64>(obj["startTime"].toDouble());
    cap.m_duration = static_cast<qint64>(obj["duration"].toDouble());
    cap.m_position = QPointF(obj["posX"].toDouble(0.5), obj["posY"].toDouble(0.85));
    cap.m_style = CaptionStyle::fromJson(obj["style"].toObject());
    cap.m_animIn = static_cast<CaptionAnimation>(obj["animIn"].toInt());
    cap.m_animOut = static_cast<CaptionAnimation>(obj["animOut"].toInt());
    cap.m_animDuration = obj["animDuration"].toDouble(300);
    cap.m_trackIndex = obj["trackIndex"].toInt();
    return cap;
}
