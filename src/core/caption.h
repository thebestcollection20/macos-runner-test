#pragma once

#include <QString>
#include <QFont>
#include <QColor>
#include <QPointF>
#include <QJsonObject>
#include <QUuid>

struct CaptionStyle {
    QFont font = QFont("Arial", 32, QFont::Bold);
    QColor textColor = QColor(255, 255, 255);
    QColor backgroundColor = QColor(0, 0, 0, 180);
    QColor outlineColor = QColor(0, 0, 0);
    qreal outlineWidth = 2.0;
    QColor shadowColor = QColor(0, 0, 0, 128);
    qreal shadowOffsetX = 2.0;
    qreal shadowOffsetY = 2.0;
    qreal shadowBlur = 4.0;
    bool showBackground = true;
    qreal backgroundPadding = 8.0;
    qreal backgroundRadius = 4.0;
    int alignment = Qt::AlignHCenter | Qt::AlignBottom;
    qreal lineSpacing = 1.2;
    qreal letterSpacing = 0.0;
    bool allCaps = false;
    bool italic = false;
    bool underline = false;

    QJsonObject toJson() const;
    static CaptionStyle fromJson(const QJsonObject &obj);
};

enum class CaptionAnimation {
    None,
    FadeIn,
    FadeOut,
    SlideUp,
    SlideDown,
    TypeWriter,
    WordByWord,
    Pop
};

class Caption {
public:
    Caption();
    Caption(const QString &text, qint64 startTime, qint64 duration);

    QUuid id() const { return m_id; }
    QString text() const { return m_text; }
    void setText(const QString &text) { m_text = text; }

    qint64 startTime() const { return m_startTime; }
    void setStartTime(qint64 ms) { m_startTime = ms; }

    qint64 duration() const { return m_duration; }
    void setDuration(qint64 ms) { m_duration = ms; }

    qint64 endTime() const { return m_startTime + m_duration; }

    QPointF position() const { return m_position; }
    void setPosition(const QPointF &pos) { m_position = pos; }

    CaptionStyle style() const { return m_style; }
    void setStyle(const CaptionStyle &style) { m_style = style; }

    CaptionAnimation animationIn() const { return m_animIn; }
    void setAnimationIn(CaptionAnimation a) { m_animIn = a; }

    CaptionAnimation animationOut() const { return m_animOut; }
    void setAnimationOut(CaptionAnimation a) { m_animOut = a; }

    qreal animationDuration() const { return m_animDuration; }
    void setAnimationDuration(qreal d) { m_animDuration = d; }

    int trackIndex() const { return m_trackIndex; }
    void setTrackIndex(int idx) { m_trackIndex = idx; }

    bool isSelected() const { return m_selected; }
    void setSelected(bool s) { m_selected = s; }

    QJsonObject toJson() const;
    static Caption fromJson(const QJsonObject &obj);

private:
    QUuid m_id;
    QString m_text;
    qint64 m_startTime = 0;
    qint64 m_duration = 3000;
    QPointF m_position = QPointF(0.5, 0.85); // normalized coords
    CaptionStyle m_style;
    CaptionAnimation m_animIn = CaptionAnimation::FadeIn;
    CaptionAnimation m_animOut = CaptionAnimation::FadeOut;
    qreal m_animDuration = 300;
    int m_trackIndex = 0;
    bool m_selected = false;
};
