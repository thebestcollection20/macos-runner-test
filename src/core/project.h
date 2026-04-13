#pragma once

#include "timeline.h"
#include <QString>
#include <QSize>
#include <QJsonObject>

class Project : public QObject {
    Q_OBJECT

public:
    explicit Project(QObject *parent = nullptr);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString filePath() const { return m_filePath; }
    void setFilePath(const QString &path) { m_filePath = path; }

    QSize resolution() const { return m_resolution; }
    void setResolution(const QSize &res) { m_resolution = res; }

    qreal frameRate() const { return m_frameRate; }
    void setFrameRate(qreal fps) { m_frameRate = fps; }

    Timeline *timeline() const { return m_timeline; }

    QStringList mediaFiles() const { return m_mediaFiles; }
    void addMediaFile(const QString &path);
    void removeMediaFile(const QString &path);

    bool isModified() const { return m_modified; }
    void setModified(bool m) { m_modified = m; }

    bool save();
    bool saveAs(const QString &path);
    static Project *load(const QString &path, QObject *parent = nullptr);

    QJsonObject toJson() const;
    static Project *fromJson(const QJsonObject &obj, QObject *parent = nullptr);

signals:
    void modified();
    void mediaFileAdded(const QString &path);
    void mediaFileRemoved(const QString &path);

private:
    QString m_name = "Untitled Project";
    QString m_filePath;
    QSize m_resolution = QSize(1920, 1080);
    qreal m_frameRate = 30.0;
    Timeline *m_timeline;
    QStringList m_mediaFiles;
    bool m_modified = false;
};
