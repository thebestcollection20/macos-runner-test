#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "core/timeline.h"

class TimelineRuler : public QWidget {
    Q_OBJECT

public:
    explicit TimelineRuler(QWidget *parent = nullptr);

    void setTimeline(Timeline *timeline) { m_timeline = timeline; update(); }
    void setPixelsPerMs(qreal ppm) { m_pixelsPerMs = ppm; update(); }
    void setScrollOffset(int offset) { m_scrollOffset = offset; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void seekRequested(qint64 time);

private:
    Timeline *m_timeline = nullptr;
    qreal m_pixelsPerMs = 0.1;
    int m_scrollOffset = 0;
};

class TimelineTrackHeader : public QWidget {
    Q_OBJECT

public:
    TimelineTrackHeader(int trackIndex, const Track &track, QWidget *parent = nullptr);

    void setTrack(const Track &track);

signals:
    void visibilityToggled(int index, bool visible);
    void lockToggled(int index, bool locked);
    void muteToggled(int index, bool muted);

private:
    int m_trackIndex;
    QLabel *m_nameLabel;
    QPushButton *m_visibleBtn;
    QPushButton *m_lockBtn;
    QPushButton *m_muteBtn;
};

class TimelineCanvas : public QWidget {
    Q_OBJECT

public:
    explicit TimelineCanvas(QWidget *parent = nullptr);

    void setTimeline(Timeline *timeline);
    void setPixelsPerMs(qreal ppm) { m_pixelsPerMs = ppm; update(); }
    void setScrollOffset(int offset) { m_scrollOffset = offset; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void clipSelected(const QUuid &id);
    void captionSelected(const QUuid &id);
    void addCaptionAtTime(qint64 time, int trackIndex);
    void dropMediaFile(const QString &path, qint64 time, int trackIndex);

private:
    enum DragMode { DragNone, DragMoveClip, DragMoveCaption, DragTrimLeftClip, DragTrimRightClip, DragTrimLeftCaption, DragTrimRightCaption };

    QColor clipColor(ClipType type, bool selected) const;
    QRect clipRect(const Clip &clip) const;
    QRect captionRect(const Caption &cap) const;
    int trackY(int trackIndex) const;
    int trackAtY(int y) const;
    bool isNearLeftEdge(const QRect &r, const QPoint &pos) const;
    bool isNearRightEdge(const QRect &r, const QPoint &pos) const;
    void updateCursorForPos(const QPoint &pos);

    Timeline *m_timeline = nullptr;
    qreal m_pixelsPerMs = 0.1;
    int m_scrollOffset = 0;
    int m_trackHeight = 40;
    QUuid m_dragItemId;
    QPoint m_dragStart;
    DragMode m_dragMode = DragNone;
    qint64 m_originalStart = 0;
    qint64 m_originalDuration = 0;
};

class TimelinePanel : public QWidget {
    Q_OBJECT

public:
    explicit TimelinePanel(QWidget *parent = nullptr);

    void setTimeline(Timeline *timeline);

signals:
    void clipSelected(const QUuid &id);
    void captionSelected(const QUuid &id);
    void addCaptionRequested();
    void splitRequested();
    void deleteRequested();
    void addTrackRequested();
    void dropMediaFile(const QString &path, qint64 time, int trackIndex);

public slots:
    void zoomIn();
    void zoomOut();
    void fitToWindow();

private:
    void updateHeaders();
    void onScroll(int value);

    Timeline *m_timeline = nullptr;
    TimelineRuler *m_ruler;
    TimelineCanvas *m_canvas;
    QScrollArea *m_scrollArea;
    QWidget *m_headersWidget;
    QVBoxLayout *m_headersLayout;
    QPushButton *m_addCaptionBtn;
    QPushButton *m_addTrackBtn;
    QPushButton *m_splitBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_snapBtn;
    QPushButton *m_zoomInBtn;
    QPushButton *m_zoomOutBtn;
    QLabel *m_zoomLabel;
    qreal m_pixelsPerMs = 0.1;
};
