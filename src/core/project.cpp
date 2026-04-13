#include "project.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

Project::Project(QObject *parent)
    : QObject(parent)
    , m_timeline(new Timeline(this))
{
}

void Project::addMediaFile(const QString &path)
{
    if (!m_mediaFiles.contains(path)) {
        m_mediaFiles.append(path);
        m_modified = true;
        emit mediaFileAdded(path);
        emit modified();
    }
}

void Project::removeMediaFile(const QString &path)
{
    if (m_mediaFiles.removeAll(path) > 0) {
        m_modified = true;
        emit mediaFileRemoved(path);
        emit modified();
    }
}

bool Project::save()
{
    if (m_filePath.isEmpty())
        return false;
    return saveAs(m_filePath);
}

bool Project::saveAs(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    m_filePath = path;
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    m_modified = false;
    return true;
}

Project *Project::load(const QString &path, QObject *parent)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    Project *proj = fromJson(doc.object(), parent);
    if (proj)
        proj->m_filePath = path;
    return proj;
}

QJsonObject Project::toJson() const
{
    QJsonObject obj;
    obj["name"] = m_name;
    obj["resolutionW"] = m_resolution.width();
    obj["resolutionH"] = m_resolution.height();
    obj["frameRate"] = m_frameRate;
    obj["timeline"] = m_timeline->toJson();

    QJsonArray mediaArr;
    for (const auto &f : m_mediaFiles)
        mediaArr.append(f);
    obj["mediaFiles"] = mediaArr;

    return obj;
}

Project *Project::fromJson(const QJsonObject &obj, QObject *parent)
{
    auto *proj = new Project(parent);
    proj->m_name = obj["name"].toString("Untitled Project");
    proj->m_resolution = QSize(obj["resolutionW"].toInt(1920),
                                obj["resolutionH"].toInt(1080));
    proj->m_frameRate = obj["frameRate"].toDouble(30.0);

    delete proj->m_timeline;
    proj->m_timeline = Timeline::fromJson(obj["timeline"].toObject(), proj);

    for (const auto &val : obj["mediaFiles"].toArray())
        proj->m_mediaFiles.append(val.toString());

    return proj;
}
