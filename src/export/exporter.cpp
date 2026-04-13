#include "exporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

Exporter::Exporter(QObject *parent)
    : QObject(parent)
{
}

void Exporter::exportProject()
{
    if (!m_project) {
        emit exportFailed("No project loaded");
        return;
    }

    m_exporting = true;
    m_cancelled = false;
    m_progress = 0.0;
    emit exportStarted();

    // For now, export captions since full video export requires FFmpeg
    QString path = m_settings.outputPath;

    if (m_settings.captionFormat == "SRT") {
        exportSRT(path);
    } else if (m_settings.captionFormat == "VTT") {
        exportVTT(path);
    } else if (m_settings.captionFormat == "ASS") {
        exportASS(path);
    }

    m_exporting = false;
    m_progress = 1.0;
    emit progressChanged(1.0);
    emit exportCompleted(m_settings.outputPath);
}

void Exporter::exportCaptions()
{
    if (!m_project) {
        emit exportFailed("No project loaded");
        return;
    }

    m_exporting = true;
    emit exportStarted();

    QString path = m_settings.outputPath;

    if (m_settings.captionFormat == "SRT") {
        exportSRT(path);
    } else if (m_settings.captionFormat == "VTT") {
        exportVTT(path);
    } else if (m_settings.captionFormat == "ASS") {
        exportASS(path);
    }

    m_exporting = false;
    m_progress = 1.0;
    emit progressChanged(1.0);
    emit exportCompleted(path);
}

void Exporter::cancel()
{
    m_cancelled = true;
    m_exporting = false;
    emit exportCancelled();
}

void Exporter::exportSRT(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + path);
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    auto captions = m_project->timeline()->captions();
    std::sort(captions.begin(), captions.end(),
              [](const Caption &a, const Caption &b) { return a.startTime() < b.startTime(); });

    int index = 1;
    for (const auto &cap : captions) {
        out << index++ << "\n";
        out << formatTimeSRT(cap.startTime()) << " --> " << formatTimeSRT(cap.endTime()) << "\n";
        out << cap.text() << "\n\n";

        m_progress = static_cast<qreal>(index - 1) / captions.size();
        emit progressChanged(m_progress);

        if (m_cancelled) {
            emit exportCancelled();
            return;
        }
    }
}

void Exporter::exportVTT(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + path);
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "WEBVTT\n\n";

    auto captions = m_project->timeline()->captions();
    std::sort(captions.begin(), captions.end(),
              [](const Caption &a, const Caption &b) { return a.startTime() < b.startTime(); });

    int index = 0;
    for (const auto &cap : captions) {
        out << formatTimeVTT(cap.startTime()) << " --> " << formatTimeVTT(cap.endTime()) << "\n";

        // VTT supports styling
        const CaptionStyle &style = cap.style();
        if (style.allCaps)
            out << cap.text().toUpper() << "\n\n";
        else
            out << cap.text() << "\n\n";

        index++;
        m_progress = static_cast<qreal>(index) / captions.size();
        emit progressChanged(m_progress);
    }
}

void Exporter::exportASS(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + path);
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // ASS header
    out << "[Script Info]\n";
    out << "Title: CaptionStudio Export\n";
    out << "ScriptType: v4.00+\n";
    out << "WrapStyle: 0\n";
    out << "ScaledBorderAndShadow: yes\n";
    out << "PlayResX: " << m_project->resolution().width() << "\n";
    out << "PlayResY: " << m_project->resolution().height() << "\n\n";

    // Styles
    out << "[V4+ Styles]\n";
    out << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, "
           "OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, "
           "ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, "
           "Alignment, MarginL, MarginR, MarginV, Encoding\n";

    out << "Style: Default,Arial,32,&H00FFFFFF,&H000000FF,&H00000000,&H80000000,"
           "0,0,0,0,100,100,0,0,1,2,2,2,10,10,10,1\n\n";

    // Events
    out << "[Events]\n";
    out << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n";

    auto captions = m_project->timeline()->captions();
    std::sort(captions.begin(), captions.end(),
              [](const Caption &a, const Caption &b) { return a.startTime() < b.startTime(); });

    int index = 0;
    for (const auto &cap : captions) {
        QString text = cap.text();
        text.replace("\n", "\\N"); // ASS line break

        out << "Dialogue: 0,"
            << formatTimeASS(cap.startTime()) << ","
            << formatTimeASS(cap.endTime()) << ","
            << "Default,,0,0,0,,"
            << text << "\n";

        index++;
        m_progress = static_cast<qreal>(index) / captions.size();
        emit progressChanged(m_progress);
    }
}

QString Exporter::formatTimeSRT(qint64 ms) const
{
    int hours = static_cast<int>(ms / 3600000);
    int minutes = static_cast<int>((ms % 3600000) / 60000);
    int seconds = static_cast<int>((ms % 60000) / 1000);
    int millis = static_cast<int>(ms % 1000);
    return QString("%1:%2:%3,%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(millis, 3, 10, QChar('0'));
}

QString Exporter::formatTimeVTT(qint64 ms) const
{
    int hours = static_cast<int>(ms / 3600000);
    int minutes = static_cast<int>((ms % 3600000) / 60000);
    int seconds = static_cast<int>((ms % 60000) / 1000);
    int millis = static_cast<int>(ms % 1000);
    return QString("%1:%2:%3.%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(millis, 3, 10, QChar('0'));
}

QString Exporter::formatTimeASS(qint64 ms) const
{
    int hours = static_cast<int>(ms / 3600000);
    int minutes = static_cast<int>((ms % 3600000) / 60000);
    int seconds = static_cast<int>((ms % 60000) / 1000);
    int centis = static_cast<int>((ms % 1000) / 10);
    return QString("%1:%2:%3.%4")
        .arg(hours)
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(centis, 2, 10, QChar('0'));
}
