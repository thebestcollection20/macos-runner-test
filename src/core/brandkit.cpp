#include "brandkit.h"
#include <QFile>
#include <QJsonDocument>

// BrandKit
BrandKit::BrandKit()
    : m_id(QUuid::createUuid())
{
}

BrandKit::BrandKit(const QString &name)
    : m_id(QUuid::createUuid())
    , m_name(name)
{
}

void BrandKit::removeColor(int index)
{
    if (index >= 0 && index < m_colors.size())
        m_colors.removeAt(index);
}

QColor BrandKit::primaryColor() const
{
    for (const auto &c : m_colors) {
        if (c.isPrimary)
            return c.color;
    }
    return m_colors.isEmpty() ? QColor(66, 133, 244) : m_colors.first().color;
}

void BrandKit::removeFont(int index)
{
    if (index >= 0 && index < m_fonts.size())
        m_fonts.removeAt(index);
}

QString BrandKit::primaryFontFamily() const
{
    for (const auto &f : m_fonts) {
        if (f.isPrimary)
            return f.fontFamily;
    }
    return m_fonts.isEmpty() ? "Arial" : m_fonts.first().fontFamily;
}

void BrandKit::removeLogo(int index)
{
    if (index >= 0 && index < m_logoPaths.size())
        m_logoPaths.removeAt(index);
}

QJsonObject BrandKit::toJson() const
{
    QJsonObject obj;
    obj["id"] = m_id.toString();
    obj["name"] = m_name;

    QJsonArray colorsArr;
    for (const auto &c : m_colors) {
        QJsonObject co;
        co["name"] = c.name;
        co["color"] = c.color.name(QColor::HexArgb);
        co["isPrimary"] = c.isPrimary;
        colorsArr.append(co);
    }
    obj["colors"] = colorsArr;

    QJsonArray fontsArr;
    for (const auto &f : m_fonts) {
        QJsonObject fo;
        fo["name"] = f.name;
        fo["fontFamily"] = f.fontFamily;
        fo["defaultSize"] = f.defaultSize;
        fo["isPrimary"] = f.isPrimary;
        fontsArr.append(fo);
    }
    obj["fonts"] = fontsArr;

    QJsonArray logosArr;
    for (const auto &l : m_logoPaths)
        logosArr.append(l);
    obj["logos"] = logosArr;

    obj["watermarkPath"] = m_watermarkPath;
    obj["watermarkOpacity"] = m_watermarkOpacity;

    return obj;
}

BrandKit BrandKit::fromJson(const QJsonObject &obj)
{
    BrandKit kit;
    kit.m_id = QUuid::fromString(obj["id"].toString());
    kit.m_name = obj["name"].toString();

    for (const auto &val : obj["colors"].toArray()) {
        QJsonObject co = val.toObject();
        BrandColor bc;
        bc.name = co["name"].toString();
        bc.color = QColor(co["color"].toString());
        bc.isPrimary = co["isPrimary"].toBool();
        kit.m_colors.append(bc);
    }

    for (const auto &val : obj["fonts"].toArray()) {
        QJsonObject fo = val.toObject();
        BrandFont bf;
        bf.name = fo["name"].toString();
        bf.fontFamily = fo["fontFamily"].toString();
        bf.defaultSize = fo["defaultSize"].toInt(24);
        bf.isPrimary = fo["isPrimary"].toBool();
        kit.m_fonts.append(bf);
    }

    for (const auto &val : obj["logos"].toArray())
        kit.m_logoPaths.append(val.toString());

    kit.m_watermarkPath = obj["watermarkPath"].toString();
    kit.m_watermarkOpacity = obj["watermarkOpacity"].toDouble(0.3);

    return kit;
}

// BrandKitManager
BrandKitManager &BrandKitManager::instance()
{
    static BrandKitManager mgr;
    return mgr;
}

BrandKitManager::BrandKitManager()
{
    // Create a default brand kit
    BrandKit defaultKit("Default Brand");
    defaultKit.addColor({"Primary", QColor(66, 133, 244), true});
    defaultKit.addColor({"Secondary", QColor(52, 168, 83), false});
    defaultKit.addColor({"Accent", QColor(251, 188, 4), false});
    defaultKit.addColor({"Dark", QColor(30, 30, 30), false});
    defaultKit.addColor({"Light", QColor(245, 245, 245), false});
    defaultKit.addFont({"Heading", "Arial", 36, true});
    defaultKit.addFont({"Body", "Helvetica", 24, false});
    defaultKit.addFont({"Accent", "Georgia", 28, false});
    m_kits.append(defaultKit);
    m_activeKitId = defaultKit.id();
}

void BrandKitManager::addKit(const BrandKit &kit)
{
    m_kits.append(kit);
}

void BrandKitManager::removeKit(const QUuid &id)
{
    m_kits.erase(
        std::remove_if(m_kits.begin(), m_kits.end(),
                        [&id](const BrandKit &k) { return k.id() == id; }),
        m_kits.end());
}

void BrandKitManager::updateKit(const BrandKit &kit)
{
    for (int i = 0; i < m_kits.size(); ++i) {
        if (m_kits[i].id() == kit.id()) {
            m_kits[i] = kit;
            return;
        }
    }
}

BrandKit BrandKitManager::kitById(const QUuid &id) const
{
    for (const auto &k : m_kits) {
        if (k.id() == id)
            return k;
    }
    return BrandKit();
}

BrandKit BrandKitManager::activeKit() const
{
    return kitById(m_activeKitId);
}

void BrandKitManager::setActiveKit(const QUuid &id)
{
    m_activeKitId = id;
}

void BrandKitManager::loadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();
    QJsonArray arr = root["kits"].toArray();
    m_kits.clear();
    for (const auto &val : arr)
        m_kits.append(BrandKit::fromJson(val.toObject()));

    if (root.contains("activeKit"))
        m_activeKitId = QUuid::fromString(root["activeKit"].toString());
}

void BrandKitManager::saveToFile(const QString &path) const
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonObject root;
    QJsonArray arr;
    for (const auto &k : m_kits)
        arr.append(k.toJson());
    root["kits"] = arr;
    root["activeKit"] = m_activeKitId.toString();
    file.write(QJsonDocument(root).toJson());
}
