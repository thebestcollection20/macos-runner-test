#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QPainter>
#include "core/timeline.h"
#include "core/caption.h"

class VideoCanvas : public QWidget {
    Q_OBJECT

public:
    explicit VideoCanvas(QWidget *parent = nullptr);

    void setTimeline(Timeline *timeline) { m_timeline = timeline; update(); }
    void setCurrentTime(qint64 time) { m_currentTime = time; update(); }
    void setResolution(const QSize &res) { m_resolution = res; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBackground(QPainter &p, const QRect &area);
    void drawCaptions(QPainter &p, const QRect &area);
    void drawCaption(QPainter &p, const Caption &cap, const QRect &area);
    void drawSafeZones(QPainter &p, const QRect &area);

    Timeline *m_timeline = nullptr;
    qint64 m_currentTime = 0;
    QSize m_resolution = QSize(1920, 1080);
};

class PreviewPanel : public QWidget {
    Q_OBJECT

public:
    explicit PreviewPanel(QWidget *parent = nullptr);

    void setTimeline(Timeline *timeline);

public slots:
    void play();
    void pause();
    void stop();
    void togglePlay();
    void seekTo(qint64 time);
    void stepForward();
    void stepBackward();

signals:
    void timeChanged(qint64 time);
    void playStateChanged(bool playing);

private slots:
    void onTimerTick();
    void onSliderMoved(int value);

private:
    void updateTimeDisplay();
    QString formatTime(qint64 ms) const;

    VideoCanvas *m_canvas;
    QSlider *m_seekSlider;
    QLabel *m_timeLabel;
    QLabel *m_durationLabel;
    QPushButton *m_playBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_prevFrameBtn;
    QPushButton *m_nextFrameBtn;
    QPushButton *m_loopBtn;

    Timeline *m_timeline = nullptr;
    QTimer *m_playTimer;
    bool m_playing = false;
    bool m_looping = false;
    qint64 m_frameInterval = 33; // ~30fps
};
