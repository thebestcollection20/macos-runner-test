#pragma once

#include "caption.h"
#include <QString>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class CaptionPreset {
public:
    CaptionPreset();
    CaptionPreset(const QString &name, const CaptionStyle &style);

    QUuid id() const { return m_id; }
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString category() const { return m_category; }
    void setCategory(const QString &cat) { m_category = cat; }

    CaptionStyle style() const { return m_style; }
    void setStyle(const CaptionStyle &style) { m_style = style; }

    CaptionAnimation animationIn() const { return m_animIn; }
    void setAnimationIn(CaptionAnimation a) { m_animIn = a; }

    CaptionAnimation animationOut() const { return m_animOut; }
    void setAnimationOut(CaptionAnimation a) { m_animOut = a; }

    bool isBuiltIn() const { return m_builtIn; }
    void setBuiltIn(bool b) { m_builtIn = b; }

    QString previewText() const { return m_previewText; }
    void setPreviewText(const QString &t) { m_previewText = t; }

    QJsonObject toJson() const;
    static CaptionPreset fromJson(const QJsonObject &obj);

private:
    QUuid m_id;
    QString m_name;
    QString m_category = "Custom";
    CaptionStyle m_style;
    CaptionAnimation m_animIn = CaptionAnimation::FadeIn;
    CaptionAnimation m_animOut = CaptionAnimation::FadeOut;
    bool m_builtIn = false;
    QString m_previewText = "Sample Caption";
};

class PresetManager {
public:
    static PresetManager &instance();

    QVector<CaptionPreset> presets() const { return m_presets; }
    QVector<CaptionPreset> presetsInCategory(const QString &category) const;
    QStringList categories() const;

    void addPreset(const CaptionPreset &preset);
    void removePreset(const QUuid &id);
    void updatePreset(const CaptionPreset &preset);
    CaptionPreset presetById(const QUuid &id) const;

    void loadFromFile(const QString &path);
    void saveToFile(const QString &path) const;

    void loadBuiltInPresets();

private:
    PresetManager();
    QVector<CaptionPreset> m_presets;
};
