#pragma once

#include <QString>
#include <QColor>
#include <QFont>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QUuid>

struct BrandColor {
    QString name;
    QColor color;
    bool isPrimary = false;
};

struct BrandFont {
    QString name;
    QString fontFamily;
    int defaultSize = 24;
    bool isPrimary = false;
};

class BrandKit {
public:
    BrandKit();
    BrandKit(const QString &name);

    QUuid id() const { return m_id; }
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    // Colors
    QVector<BrandColor> colors() const { return m_colors; }
    void addColor(const BrandColor &color) { m_colors.append(color); }
    void removeColor(int index);
    void setColors(const QVector<BrandColor> &colors) { m_colors = colors; }
    QColor primaryColor() const;

    // Fonts
    QVector<BrandFont> fonts() const { return m_fonts; }
    void addFont(const BrandFont &font) { m_fonts.append(font); }
    void removeFont(int index);
    void setFonts(const QVector<BrandFont> &fonts) { m_fonts = fonts; }
    QString primaryFontFamily() const;

    // Logos
    QStringList logoPaths() const { return m_logoPaths; }
    void addLogo(const QString &path) { m_logoPaths.append(path); }
    void removeLogo(int index);

    // Watermark
    QString watermarkPath() const { return m_watermarkPath; }
    void setWatermarkPath(const QString &path) { m_watermarkPath = path; }
    qreal watermarkOpacity() const { return m_watermarkOpacity; }
    void setWatermarkOpacity(qreal o) { m_watermarkOpacity = o; }

    QJsonObject toJson() const;
    static BrandKit fromJson(const QJsonObject &obj);

private:
    QUuid m_id;
    QString m_name;
    QVector<BrandColor> m_colors;
    QVector<BrandFont> m_fonts;
    QStringList m_logoPaths;
    QString m_watermarkPath;
    qreal m_watermarkOpacity = 0.3;
};

class BrandKitManager {
public:
    static BrandKitManager &instance();

    QVector<BrandKit> kits() const { return m_kits; }
    void addKit(const BrandKit &kit);
    void removeKit(const QUuid &id);
    void updateKit(const BrandKit &kit);
    BrandKit kitById(const QUuid &id) const;
    BrandKit activeKit() const;
    void setActiveKit(const QUuid &id);

    void loadFromFile(const QString &path);
    void saveToFile(const QString &path) const;

private:
    BrandKitManager();
    QVector<BrandKit> m_kits;
    QUuid m_activeKitId;
};
