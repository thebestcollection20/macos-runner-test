#pragma once

#include <QObject>
#include <QString>
#include <QSize>
#include "core/project.h"

struct ExportSettings {
    QString outputPath;
    QSize resolution = QSize(1920, 1080);
    qreal frameRate = 30.0;
    QString format = "MP4";
    QString codec = "H.264";
    int quality = 80; // 0-100
    int audioBitrate = 192; // kbps
    bool exportCaptionsOnly = false;
    QString captionFormat = "SRT"; // SRT, VTT, ASS
};

class Exporter : public QObject {
    Q_OBJECT

public:
    explicit Exporter(QObject *parent = nullptr);

    void setProject(Project *project) { m_project = project; }
    void setSettings(const ExportSettings &settings) { m_settings = settings; }

    // Export operations
    void exportProject();
    void exportCaptions();
    void cancel();

    bool isExporting() const { return m_exporting; }
    qreal progress() const { return m_progress; }

signals:
    void exportStarted();
    void progressChanged(qreal progress);
    void exportCompleted(const QString &outputPath);
    void exportFailed(const QString &error);
    void exportCancelled();

private:
    void exportSRT(const QString &path);
    void exportVTT(const QString &path);
    void exportASS(const QString &path);
    QString formatTimeSRT(qint64 ms) const;
    QString formatTimeVTT(qint64 ms) const;
    QString formatTimeASS(qint64 ms) const;

    Project *m_project = nullptr;
    ExportSettings m_settings;
    bool m_exporting = false;
    bool m_cancelled = false;
    qreal m_progress = 0.0;
};
