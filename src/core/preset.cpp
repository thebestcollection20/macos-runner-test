#include "preset.h"
#include <QFile>
#include <QJsonDocument>

// CaptionPreset
CaptionPreset::CaptionPreset()
    : m_id(QUuid::createUuid())
{
}

CaptionPreset::CaptionPreset(const QString &name, const CaptionStyle &style)
    : m_id(QUuid::createUuid())
    , m_name(name)
    , m_style(style)
{
}

QJsonObject CaptionPreset::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["name"] = m_name;
    obj["category"] = m_category;
    obj["style"] = m_style.toJson();
    obj["animIn"] = static_cast<int>(m_animIn);
    obj["animOut"] = static_cast<int>(m_animOut);
    obj["builtIn"] = m_builtIn;
    obj["previewText"] = m_previewText;
    return obj;
}

CaptionPreset CaptionPreset::fromJson(const QJsonObject &obj)
{
    CaptionPreset p;
    p.m_id = QUuid::fromString(obj["id"].toString());
    p.m_name = obj["name"].toString();
    p.m_category = obj["category"].toString("Custom");
    p.m_style = CaptionStyle::fromJson(obj["style"].toObject());
    p.m_animIn = static_cast<CaptionAnimation>(obj["animIn"].toInt());
    p.m_animOut = static_cast<CaptionAnimation>(obj["animOut"].toInt());
    p.m_builtIn = obj["builtIn"].toBool(false);
    p.m_previewText = obj["previewText"].toString("Sample Caption");
    return p;
}

// PresetManager
PresetManager &PresetManager::instance()
{
    static PresetManager mgr;
    return mgr;
}

PresetManager::PresetManager()
{
    loadBuiltInPresets();
}

QVector<CaptionPreset> PresetManager::presetsInCategory(const QString &category) const
{
    QVector<CaptionPreset> result;
    for (const auto &p : m_presets) {
        if (p.category() == category)
            result.append(p);
    }
    return result;
}

QStringList PresetManager::categories() const
{
    QStringList cats;
    for (const auto &p : m_presets) {
        if (!cats.contains(p.category()))
            cats.append(p.category());
    }
    return cats;
}

void PresetManager::addPreset(const CaptionPreset &preset)
{
    m_presets.append(preset);
}

void PresetManager::removePreset(const QUuid &id)
{
    m_presets.erase(
        std::remove_if(m_presets.begin(), m_presets.end(),
                        [&id](const CaptionPreset &p) { return p.id() == id; }),
        m_presets.end());
}

void PresetManager::updatePreset(const CaptionPreset &preset)
{
    for (int i = 0; i < m_presets.size(); ++i) {
        if (m_presets[i].id() == preset.id()) {
            m_presets[i] = preset;
            return;
        }
    }
}

CaptionPreset PresetManager::presetById(const QUuid &id) const
{
    for (const auto &p : m_presets) {
        if (p.id() == id)
            return p;
    }
    return CaptionPreset();
}

void PresetManager::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray arr = doc.array();
    for (const auto &val : arr) {
        CaptionPreset p = CaptionPreset::fromJson(val.toObject());
        if (!p.isBuiltIn())
            m_presets.append(p);
    }
}

void PresetManager::saveToFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonArray arr;
    for (const auto &p : m_presets) {
        arr.append(p.toJson());
    }
    file.write(QJsonDocument(arr).toJson());
}

void PresetManager::loadBuiltInPresets()
{
    // Social Media Bold
    {
        CaptionStyle s;
        s.font = QFont("Impact", 36, QFont::Bold);
        s.textColor = QColor(255, 255, 255);
        s.outlineColor = QColor(0, 0, 0);
        s.outlineWidth = 3.0;
        s.showBackground = false;
        s.allCaps = true;
        CaptionPreset p("Social Bold", s);
        p.setCategory("Social Media");
        p.setBuiltIn(true);
        p.setPreviewText("FOLLOW FOR MORE");
        m_presets.append(p);
    }

    // Minimal Subtitle
    {
        CaptionStyle s;
        s.font = QFont("Helvetica", 24);
        s.textColor = QColor(255, 255, 255);
        s.backgroundColor = QColor(0, 0, 0, 160);
        s.showBackground = true;
        s.backgroundPadding = 6.0;
        s.backgroundRadius = 3.0;
        s.outlineWidth = 0;
        CaptionPreset p("Minimal Sub", s);
        p.setCategory("Subtitles");
        p.setBuiltIn(true);
        p.setPreviewText("Clean minimal subtitle");
        m_presets.append(p);
    }

    // Neon Glow
    {
        CaptionStyle s;
        s.font = QFont("Arial", 40, QFont::Bold);
        s.textColor = QColor(0, 255, 200);
        s.outlineColor = QColor(0, 200, 160);
        s.outlineWidth = 2.0;
        s.shadowColor = QColor(0, 255, 200, 100);
        s.shadowBlur = 12.0;
        s.shadowOffsetX = 0;
        s.shadowOffsetY = 0;
        s.showBackground = false;
        CaptionPreset p("Neon Glow", s);
        p.setCategory("Creative");
        p.setBuiltIn(true);
        p.setPreviewText("NEON VIBES");
        p.setAnimationIn(CaptionAnimation::Pop);
        m_presets.append(p);
    }

    // YouTube Title
    {
        CaptionStyle s;
        s.font = QFont("Arial Black", 44, QFont::Bold);
        s.textColor = QColor(255, 255, 0);
        s.outlineColor = QColor(0, 0, 0);
        s.outlineWidth = 4.0;
        s.showBackground = false;
        s.allCaps = true;
        CaptionPreset p("YouTube Title", s);
        p.setCategory("Social Media");
        p.setBuiltIn(true);
        p.setPreviewText("TOP 10 THINGS");
        m_presets.append(p);
    }

    // Elegant Script
    {
        CaptionStyle s;
        s.font = QFont("Georgia", 30, QFont::Normal, true);
        s.textColor = QColor(240, 220, 180);
        s.outlineWidth = 0;
        s.showBackground = false;
        s.shadowColor = QColor(0, 0, 0, 80);
        s.shadowOffsetX = 1;
        s.shadowOffsetY = 1;
        s.shadowBlur = 3;
        s.italic = true;
        CaptionPreset p("Elegant Script", s);
        p.setCategory("Cinematic");
        p.setBuiltIn(true);
        p.setPreviewText("A beautiful moment");
        p.setAnimationIn(CaptionAnimation::FadeIn);
        m_presets.append(p);
    }

    // Typewriter
    {
        CaptionStyle s;
        s.font = QFont("Courier New", 28);
        s.textColor = QColor(200, 200, 200);
        s.showBackground = false;
        s.outlineWidth = 0;
        CaptionPreset p("Typewriter", s);
        p.setCategory("Creative");
        p.setBuiltIn(true);
        p.setPreviewText("typing effect...");
        p.setAnimationIn(CaptionAnimation::TypeWriter);
        m_presets.append(p);
    }

    // Word Highlight
    {
        CaptionStyle s;
        s.font = QFont("Arial", 34, QFont::Bold);
        s.textColor = QColor(255, 255, 255);
        s.outlineColor = QColor(0, 0, 0);
        s.outlineWidth = 2.0;
        s.showBackground = false;
        CaptionPreset p("Word Pop", s);
        p.setCategory("Creative");
        p.setBuiltIn(true);
        p.setPreviewText("Word by word reveal");
        p.setAnimationIn(CaptionAnimation::WordByWord);
        m_presets.append(p);
    }

    // Lower Third
    {
        CaptionStyle s;
        s.font = QFont("Segoe UI", 22);
        s.textColor = QColor(255, 255, 255);
        s.backgroundColor = QColor(30, 30, 30, 220);
        s.showBackground = true;
        s.backgroundPadding = 12.0;
        s.backgroundRadius = 0;
        s.outlineWidth = 0;
        s.alignment = Qt::AlignLeft | Qt::AlignBottom;
        CaptionPreset p("Lower Third", s);
        p.setCategory("Professional");
        p.setBuiltIn(true);
        p.setPreviewText("John Smith | CEO");
        p.setAnimationIn(CaptionAnimation::SlideUp);
        m_presets.append(p);
    }
}
