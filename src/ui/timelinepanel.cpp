#include "timelinepanel.h"
#include "theme.h"
#include <QScrollBar>
#include <QtGlobal>
#include <QUrl>
#include <QFileInfo>
#include <QInputDialog>

// Qt5/Qt6 mouse event compatibility
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define MOUSE_POS(event) (event)->position().toPoint()
#define MOUSE_X(event)   static_cast<int>((event)->position().x())
#else
#define MOUSE_POS(event) (event)->pos()
#define MOUSE_X(event)   (event)->x()
#endif

// TimelineRuler
TimelineRuler::TimelineRuler(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(28);
    setStyleSheet("background-color: #1c1c1c;");
}

void TimelineRuler::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor(28, 28, 28));

    // Time markings
    qreal msPerPixel = 1.0 / m_pixelsPerMs;
    qreal majorInterval = 1000; // 1 second
    if (msPerPixel > 5) majorInterval = 5000;
    if (msPerPixel > 20) majorInterval = 10000;
    if (msPerPixel > 50) majorInterval = 30000;
    if (msPerPixel > 100) majorInterval = 60000;

    qreal minorInterval = majorInterval / 5;

    p.setPen(QColor(80, 80, 80));
    QFont rulerFont("Courier New", 8);
    p.setFont(rulerFont);

    qreal startMs = m_scrollOffset / m_pixelsPerMs;
    qreal endMs = (m_scrollOffset + width()) / m_pixelsPerMs;

    qint64 firstMajor = static_cast<qint64>((startMs / majorInterval)) * static_cast<qint64>(majorInterval);

    for (qreal ms = firstMajor; ms <= endMs; ms += minorInterval) {
        int x = static_cast<int>(ms * m_pixelsPerMs - m_scrollOffset);

        bool isMajor = (static_cast<qint64>(ms) % static_cast<qint64>(majorInterval)) == 0;

        if (isMajor) {
            p.setPen(QColor(100, 100, 100));
            p.drawLine(x, height() - 14, x, height());

            int sec = static_cast<int>(ms / 1000);
            int min = sec / 60;
            sec %= 60;
            QString label = QString("%1:%2").arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
            p.setPen(QColor(150, 150, 150));
            p.drawText(x + 3, 12, label);
        } else {
            p.setPen(QColor(60, 60, 60));
            p.drawLine(x, height() - 6, x, height());
        }
    }

    // Playhead
    if (m_timeline) {
        int playheadX = static_cast<int>(m_timeline->currentTime() * m_pixelsPerMs - m_scrollOffset);
        p.setPen(QPen(DaVinciTheme::Colors::Playhead, 2));
        p.drawLine(playheadX, 0, playheadX, height());

        // Playhead triangle
        QPolygon triangle;
        triangle << QPoint(playheadX - 5, 0)
                 << QPoint(playheadX + 5, 0)
                 << QPoint(playheadX, 8);
        p.setBrush(DaVinciTheme::Colors::Playhead);
        p.setPen(Qt::NoPen);
        p.drawPolygon(triangle);
    }

    // Bottom border
    p.setPen(QColor(55, 55, 55));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void TimelineRuler::mousePressEvent(QMouseEvent *event)
{
    qint64 time = static_cast<qint64>((MOUSE_X(event) + m_scrollOffset) / m_pixelsPerMs);
    emit seekRequested(time);
}

// TimelineTrackHeader
TimelineTrackHeader::TimelineTrackHeader(int trackIndex, const Track &track, QWidget *parent)
    : QWidget(parent)
    , m_trackIndex(trackIndex)
{
    setFixedHeight(40);
    setStyleSheet("background-color: #1a1a1a; border-bottom: 1px solid #2a2a2a;");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 2, 4, 2);
    layout->setSpacing(2);

    m_nameLabel = new QLabel(track.name);
    m_nameLabel->setStyleSheet("color: #b0b0b0; font-size: 10px;");

    m_visibleBtn = new QPushButton("V");
    m_visibleBtn->setFixedSize(20, 20);
    m_visibleBtn->setCheckable(true);
    m_visibleBtn->setChecked(track.visible);
    m_visibleBtn->setToolTip("Toggle Visibility");

    m_lockBtn = new QPushButton("L");
    m_lockBtn->setFixedSize(20, 20);
    m_lockBtn->setCheckable(true);
    m_lockBtn->setChecked(track.locked);
    m_lockBtn->setToolTip("Toggle Lock");

    m_muteBtn = new QPushButton("M");
    m_muteBtn->setFixedSize(20, 20);
    m_muteBtn->setCheckable(true);
    m_muteBtn->setChecked(track.muted);
    m_muteBtn->setToolTip("Toggle Mute");

    layout->addWidget(m_nameLabel, 1);
    layout->addWidget(m_visibleBtn);
    layout->addWidget(m_lockBtn);
    layout->addWidget(m_muteBtn);

    connect(m_visibleBtn, &QPushButton::toggled, [this](bool checked) {
        emit visibilityToggled(m_trackIndex, checked);
    });
    connect(m_lockBtn, &QPushButton::toggled, [this](bool checked) {
        emit lockToggled(m_trackIndex, checked);
    });
    connect(m_muteBtn, &QPushButton::toggled, [this](bool checked) {
        emit muteToggled(m_trackIndex, checked);
    });
}

void TimelineTrackHeader::setTrack(const Track &track)
{
    m_nameLabel->setText(track.name);
    m_visibleBtn->setChecked(track.visible);
    m_lockBtn->setChecked(track.locked);
    m_muteBtn->setChecked(track.muted);
}

// TimelineCanvas
TimelineCanvas::TimelineCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumHeight(120);
    setAcceptDrops(true);
}

void TimelineCanvas::setTimeline(Timeline *timeline)
{
    m_timeline = timeline;
    if (m_timeline) {
        connect(m_timeline, &Timeline::timeChanged, [this]() { update(); });
        connect(m_timeline, &Timeline::clipAdded, [this]() { update(); });
        connect(m_timeline, &Timeline::clipRemoved, [this]() { update(); });
        connect(m_timeline, &Timeline::captionAdded, [this]() { update(); });
        connect(m_timeline, &Timeline::captionRemoved, [this]() { update(); });
    }
    update();
}

void TimelineCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (!m_timeline) {
        p.fillRect(rect(), QColor(24, 24, 24));
        return;
    }

    // Draw tracks background
    for (int i = 0; i < m_timeline->trackCount(); ++i) {
        int y = trackY(i);
        QColor trackBg = (i % 2 == 0) ? QColor(35, 35, 35) : QColor(38, 38, 38);
        p.fillRect(0, y, width(), m_trackHeight, trackBg);

        // Track separator
        p.setPen(QColor(28, 28, 28));
        p.drawLine(0, y + m_trackHeight, width(), y + m_trackHeight);
    }

    // Draw clips
    for (const auto &clip : m_timeline->clips()) {
        QRect r = clipRect(clip);
        if (r.right() < 0 || r.left() > width())
            continue;

        QColor color = clipColor(clip.type(), clip.isSelected());

        // Clip body
        p.fillRect(r, color);

        // Clip border
        p.setPen(QPen(color.lighter(130), 1));
        p.drawRect(r);

        // Clip name
        p.setPen(Qt::white);
        QFont clipFont("Segoe UI", 8);
        p.setFont(clipFont);
        p.drawText(r.adjusted(4, 2, -4, -2), Qt::AlignLeft | Qt::AlignVCenter, clip.name());

        // Trim handles (always visible for better UX)
        QColor clipHandleColor = clip.isSelected() ? color.lighter(180) : color.lighter(140);
        p.fillRect(r.left(), r.top(), 5, r.height(), clipHandleColor);
        p.fillRect(r.right() - 5, r.top(), 5, r.height(), clipHandleColor);
    }

    // Draw captions on caption tracks
    for (const auto &cap : m_timeline->captions()) {
        QRect r = captionRect(cap);
        if (r.right() < 0 || r.left() > width())
            continue;

        QColor color = cap.isSelected() ? DaVinciTheme::Colors::ClipSelected
                                        : DaVinciTheme::Colors::ClipCaption;

        // Caption body
        p.fillRect(r, color);
        p.setPen(QPen(color.lighter(130), 1));
        p.drawRect(r);

        // Caption text preview
        p.setPen(Qt::white);
        QFont capFont("Segoe UI", 8);
        p.setFont(capFont);
        p.drawText(r.adjusted(8, 2, -8, -2), Qt::AlignLeft | Qt::AlignVCenter,
                   cap.text().left(30));

        // Trim handles for captions
        QColor capHandleColor = cap.isSelected() ? color.lighter(180) : color.lighter(140);
        p.fillRect(r.left(), r.top(), 5, r.height(), capHandleColor);
        p.fillRect(r.right() - 5, r.top(), 5, r.height(), capHandleColor);
    }

    // Playhead
    int playheadX = static_cast<int>(m_timeline->currentTime() * m_pixelsPerMs - m_scrollOffset);
    p.setPen(QPen(DaVinciTheme::Colors::Playhead, 2));
    p.drawLine(playheadX, 0, playheadX, height());
}

void TimelineCanvas::mousePressEvent(QMouseEvent *event)
{
    if (!m_timeline)
        return;

    QPoint pos = MOUSE_POS(event);
    m_dragStart = pos;

    // Check captions first (edge trim takes priority over move)
    for (auto &cap : m_timeline->captions()) {
        QRect r = captionRect(cap);
        if (r.contains(pos)) {
            m_timeline->deselectAllClips();
            m_timeline->deselectAllCaptions();
            Caption *selected = m_timeline->captionById(cap.id());
            if (selected) {
                selected->setSelected(true);
                m_dragItemId = cap.id();
                m_originalStart = cap.startTime();
                m_originalDuration = cap.duration();

                if (isNearLeftEdge(r, pos))
                    m_dragMode = DragTrimLeftCaption;
                else if (isNearRightEdge(r, pos))
                    m_dragMode = DragTrimRightCaption;
                else
                    m_dragMode = DragMoveCaption;

                emit captionSelected(cap.id());
            }
            update();
            return;
        }
    }

    // Check clips
    for (auto &clip : m_timeline->clips()) {
        QRect r = clipRect(clip);
        if (r.contains(pos)) {
            m_timeline->deselectAllClips();
            m_timeline->deselectAllCaptions();
            Clip *selected = m_timeline->clipById(clip.id());
            if (selected) {
                selected->setSelected(true);
                m_dragItemId = clip.id();
                m_originalStart = clip.startTime();
                m_originalDuration = clip.duration();

                if (isNearLeftEdge(r, pos))
                    m_dragMode = DragTrimLeftClip;
                else if (isNearRightEdge(r, pos))
                    m_dragMode = DragTrimRightClip;
                else
                    m_dragMode = DragMoveClip;

                emit clipSelected(clip.id());
            }
            update();
            return;
        }
    }

    // Click on empty space - deselect
    m_timeline->deselectAllClips();
    m_timeline->deselectAllCaptions();
    m_dragMode = DragNone;
    update();
}

void TimelineCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_timeline) return;

    QPoint pos = MOUSE_POS(event);

    if (m_dragMode == DragNone) {
        updateCursorForPos(pos);
        return;
    }

    int dx = pos.x() - m_dragStart.x();
    qint64 dt = static_cast<qint64>(dx / m_pixelsPerMs);

    switch (m_dragMode) {
    case DragMoveClip: {
        Clip *clip = m_timeline->clipById(m_dragItemId);
        if (clip) {
            qint64 newStart = std::max(qint64(0), m_originalStart + dt);
            clip->setStartTime(newStart);
            int newTrack = trackAtY(pos.y());
            if (newTrack >= 0 && newTrack < m_timeline->trackCount())
                clip->setTrackIndex(newTrack);
        }
        break;
    }
    case DragMoveCaption: {
        Caption *cap = m_timeline->captionById(m_dragItemId);
        if (cap) {
            qint64 newStart = std::max(qint64(0), m_originalStart + dt);
            cap->setStartTime(newStart);
            int newTrack = trackAtY(pos.y());
            if (newTrack >= 0 && newTrack < m_timeline->trackCount())
                cap->setTrackIndex(newTrack);
        }
        break;
    }
    case DragTrimLeftClip: {
        Clip *clip = m_timeline->clipById(m_dragItemId);
        if (clip) {
            qint64 newStart = std::max(qint64(0), m_originalStart + dt);
            qint64 endTime = m_originalStart + m_originalDuration;
            if (newStart < endTime - 100) {
                clip->setStartTime(newStart);
                clip->setDuration(endTime - newStart);
            }
        }
        break;
    }
    case DragTrimRightClip: {
        Clip *clip = m_timeline->clipById(m_dragItemId);
        if (clip) {
            qint64 newDur = std::max(qint64(100), m_originalDuration + dt);
            clip->setDuration(newDur);
        }
        break;
    }
    case DragTrimLeftCaption: {
        Caption *cap = m_timeline->captionById(m_dragItemId);
        if (cap) {
            qint64 newStart = std::max(qint64(0), m_originalStart + dt);
            qint64 endTime = m_originalStart + m_originalDuration;
            if (newStart < endTime - 100) {
                cap->setStartTime(newStart);
                cap->setDuration(endTime - newStart);
            }
        }
        break;
    }
    case DragTrimRightCaption: {
        Caption *cap = m_timeline->captionById(m_dragItemId);
        if (cap) {
            qint64 newDur = std::max(qint64(100), m_originalDuration + dt);
            cap->setDuration(newDur);
        }
        break;
    }
    default:
        break;
    }
    update();
}

void TimelineCanvas::mouseReleaseEvent(QMouseEvent *)
{
    m_dragMode = DragNone;
    setCursor(Qt::ArrowCursor);
    if (m_timeline)
        emit m_timeline->durationChanged(m_timeline->duration());
}

void TimelineCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!m_timeline) return;

    QPoint pos = MOUSE_POS(event);
    int trackIdx = trackAtY(pos.y());
    qint64 time = static_cast<qint64>((pos.x() + m_scrollOffset) / m_pixelsPerMs);

    // Double-click on empty area of a caption track adds a caption
    if (trackIdx >= 0 && trackIdx < m_timeline->trackCount()) {
        Track t = m_timeline->track(trackIdx);
        if (t.type == Track::Caption) {
            emit addCaptionAtTime(time, trackIdx);
            return;
        }
    }
}

void TimelineCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() || event->mimeData()->hasText())
        event->acceptProposedAction();
}

void TimelineCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void TimelineCanvas::dropEvent(QDropEvent *event)
{
    if (!m_timeline) return;

    QPoint pos = MOUSE_POS(event);
    int trackIdx = trackAtY(pos.y());
    qint64 time = static_cast<qint64>((pos.x() + m_scrollOffset) / m_pixelsPerMs);

    if (trackIdx < 0) trackIdx = 0;
    if (trackIdx >= m_timeline->trackCount()) trackIdx = 0;

    if (event->mimeData()->hasUrls()) {
        for (const auto &url : event->mimeData()->urls()) {
            QString path = url.toLocalFile();
            if (!path.isEmpty())
                emit dropMediaFile(path, time, trackIdx);
        }
        event->acceptProposedAction();
    } else if (event->mimeData()->hasText()) {
        QString path = event->mimeData()->text();
        if (QFileInfo::exists(path))
            emit dropMediaFile(path, time, trackIdx);
        event->acceptProposedAction();
    }
}

QColor TimelineCanvas::clipColor(ClipType type, bool selected) const
{
    if (selected)
        return DaVinciTheme::Colors::ClipSelected;

    switch (type) {
    case ClipType::Video:      return DaVinciTheme::Colors::ClipVideo;
    case ClipType::Audio:      return DaVinciTheme::Colors::ClipAudio;
    case ClipType::Image:      return DaVinciTheme::Colors::ClipImage;
    case ClipType::Caption:    return DaVinciTheme::Colors::ClipCaption;
    case ClipType::Transition: return DaVinciTheme::Colors::ClipTransition;
    }
    return DaVinciTheme::Colors::ClipVideo;
}

QRect TimelineCanvas::clipRect(const Clip &clip) const
{
    int x = static_cast<int>(clip.startTime() * m_pixelsPerMs - m_scrollOffset);
    int w = static_cast<int>(clip.duration() * m_pixelsPerMs);
    int y = trackY(clip.trackIndex());
    return QRect(x, y + 2, std::max(w, 4), m_trackHeight - 4);
}

QRect TimelineCanvas::captionRect(const Caption &cap) const
{
    int x = static_cast<int>(cap.startTime() * m_pixelsPerMs - m_scrollOffset);
    int w = static_cast<int>(cap.duration() * m_pixelsPerMs);
    int y = trackY(cap.trackIndex());
    return QRect(x, y + 2, std::max(w, 4), m_trackHeight - 4);
}

int TimelineCanvas::trackY(int trackIndex) const
{
    return trackIndex * m_trackHeight;
}

int TimelineCanvas::trackAtY(int y) const
{
    if (y < 0) return -1;
    return y / m_trackHeight;
}

bool TimelineCanvas::isNearLeftEdge(const QRect &r, const QPoint &pos) const
{
    return pos.x() >= r.left() && pos.x() <= r.left() + 6;
}

bool TimelineCanvas::isNearRightEdge(const QRect &r, const QPoint &pos) const
{
    return pos.x() >= r.right() - 6 && pos.x() <= r.right();
}

void TimelineCanvas::updateCursorForPos(const QPoint &pos)
{
    if (!m_timeline) return;

    for (const auto &cap : m_timeline->captions()) {
        QRect r = captionRect(cap);
        if (r.contains(pos)) {
            if (isNearLeftEdge(r, pos) || isNearRightEdge(r, pos))
                setCursor(Qt::SizeHorCursor);
            else
                setCursor(Qt::OpenHandCursor);
            return;
        }
    }

    for (const auto &clip : m_timeline->clips()) {
        QRect r = clipRect(clip);
        if (r.contains(pos)) {
            if (isNearLeftEdge(r, pos) || isNearRightEdge(r, pos))
                setCursor(Qt::SizeHorCursor);
            else
                setCursor(Qt::OpenHandCursor);
            return;
        }
    }

    setCursor(Qt::ArrowCursor);
}

// TimelinePanel
TimelinePanel::TimelinePanel(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar
    auto *toolbar = new QWidget;
    toolbar->setFixedHeight(32);
    toolbar->setStyleSheet("background-color: #1a1a1a; border-bottom: 1px solid #373737;");

    auto *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(6, 2, 6, 2);
    toolbarLayout->setSpacing(4);

    m_addCaptionBtn = new QPushButton("+ Caption");
    m_addCaptionBtn->setObjectName("accentButton");
    m_addCaptionBtn->setFixedHeight(24);
    m_addCaptionBtn->setToolTip("Add Caption");

    m_addTrackBtn = new QPushButton("+ Track");
    m_addTrackBtn->setFixedHeight(24);

    m_splitBtn = new QPushButton("Split");
    m_splitBtn->setFixedHeight(24);
    m_splitBtn->setToolTip("Split at Playhead (S)");

    m_deleteBtn = new QPushButton("Delete");
    m_deleteBtn->setFixedHeight(24);
    m_deleteBtn->setToolTip("Delete Selected (Del)");

    m_snapBtn = new QPushButton("Snap");
    m_snapBtn->setFixedHeight(24);
    m_snapBtn->setCheckable(true);
    m_snapBtn->setChecked(true);
    m_snapBtn->setToolTip("Toggle Snapping (N)");

    m_zoomOutBtn = new QPushButton("-");
    m_zoomOutBtn->setFixedSize(24, 24);
    m_zoomLabel = new QLabel("100%");
    m_zoomLabel->setStyleSheet("color: #969696; font-size: 10px; min-width: 36px;");
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    m_zoomInBtn = new QPushButton("+");
    m_zoomInBtn->setFixedSize(24, 24);

    toolbarLayout->addWidget(m_addCaptionBtn);
    toolbarLayout->addWidget(m_addTrackBtn);
    toolbarLayout->addSpacing(8);
    toolbarLayout->addWidget(m_splitBtn);
    toolbarLayout->addWidget(m_deleteBtn);
    toolbarLayout->addSpacing(8);
    toolbarLayout->addWidget(m_snapBtn);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_zoomOutBtn);
    toolbarLayout->addWidget(m_zoomLabel);
    toolbarLayout->addWidget(m_zoomInBtn);

    mainLayout->addWidget(toolbar);

    // Timeline area
    auto *timelineArea = new QWidget;
    auto *timelineLayout = new QHBoxLayout(timelineArea);
    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);

    // Track headers
    auto *headersContainer = new QWidget;
    headersContainer->setFixedWidth(140);
    headersContainer->setStyleSheet("background-color: #1a1a1a; border-right: 1px solid #373737;");

    auto *headersOuterLayout = new QVBoxLayout(headersContainer);
    headersOuterLayout->setContentsMargins(0, 0, 0, 0);
    headersOuterLayout->setSpacing(0);

    // Spacer for ruler
    auto *rulerSpacer = new QWidget;
    rulerSpacer->setFixedHeight(28);
    rulerSpacer->setStyleSheet("background-color: #1a1a1a;");
    headersOuterLayout->addWidget(rulerSpacer);

    m_headersWidget = new QWidget;
    m_headersLayout = new QVBoxLayout(m_headersWidget);
    m_headersLayout->setContentsMargins(0, 0, 0, 0);
    m_headersLayout->setSpacing(0);
    headersOuterLayout->addWidget(m_headersWidget);
    headersOuterLayout->addStretch();

    // Ruler + canvas
    auto *rightSide = new QWidget;
    auto *rightLayout = new QVBoxLayout(rightSide);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    m_ruler = new TimelineRuler;
    m_canvas = new TimelineCanvas;

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(m_canvas);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #181818; }");

    rightLayout->addWidget(m_ruler);
    rightLayout->addWidget(m_scrollArea);

    timelineLayout->addWidget(headersContainer);
    timelineLayout->addWidget(rightSide, 1);

    mainLayout->addWidget(timelineArea, 1);

    // Connections
    connect(m_addCaptionBtn, &QPushButton::clicked, this, &TimelinePanel::addCaptionRequested);
    connect(m_splitBtn, &QPushButton::clicked, this, &TimelinePanel::splitRequested);
    connect(m_deleteBtn, &QPushButton::clicked, this, &TimelinePanel::deleteRequested);
    connect(m_addTrackBtn, &QPushButton::clicked, this, &TimelinePanel::addTrackRequested);
    connect(m_zoomInBtn, &QPushButton::clicked, this, &TimelinePanel::zoomIn);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &TimelinePanel::zoomOut);
    connect(m_ruler, &TimelineRuler::seekRequested, [this](qint64 time) {
        if (m_timeline) m_timeline->setCurrentTime(time);
    });
    connect(m_canvas, &TimelineCanvas::clipSelected, this, &TimelinePanel::clipSelected);
    connect(m_canvas, &TimelineCanvas::captionSelected, this, &TimelinePanel::captionSelected);
    connect(m_canvas, &TimelineCanvas::addCaptionAtTime, [this](qint64 time, int) {
        if (m_timeline) m_timeline->setCurrentTime(time);
        emit addCaptionRequested();
    });
    connect(m_canvas, &TimelineCanvas::dropMediaFile, this, &TimelinePanel::dropMediaFile);

    // Snap button: connect once in constructor, lambda reads m_timeline at invocation time
    connect(m_snapBtn, &QPushButton::toggled, [this](bool checked) {
        if (m_timeline) m_timeline->setSnapEnabled(checked);
    });

    connect(m_scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &TimelinePanel::onScroll);
}

void TimelinePanel::setTimeline(Timeline *timeline)
{
    m_timeline = timeline;
    m_ruler->setTimeline(timeline);
    m_canvas->setTimeline(timeline);
    updateHeaders();

    if (m_timeline) {
        int canvasWidth = static_cast<int>(m_timeline->duration() * m_pixelsPerMs) + 200;
        int canvasHeight = m_timeline->trackCount() * 40;
        m_canvas->setFixedSize(canvasWidth, canvasHeight);

        connect(m_timeline, &Timeline::durationChanged, [this]() {
            int w = static_cast<int>(m_timeline->duration() * m_pixelsPerMs) + 200;
            m_canvas->setFixedWidth(w);
        });

        connect(m_timeline, &Timeline::trackAdded, [this]() {
            updateHeaders();
            int h = m_timeline->trackCount() * 40;
            m_canvas->setFixedHeight(h);
        });

        connect(m_timeline, &Timeline::trackRemoved, [this]() {
            updateHeaders();
            int h = m_timeline->trackCount() * 40;
            m_canvas->setFixedHeight(h);
        });

    }
}

void TimelinePanel::zoomIn()
{
    m_pixelsPerMs = std::min(1.0, m_pixelsPerMs * 1.3);
    m_ruler->setPixelsPerMs(m_pixelsPerMs);
    m_canvas->setPixelsPerMs(m_pixelsPerMs);
    m_zoomLabel->setText(QString("%1%").arg(static_cast<int>(m_pixelsPerMs * 1000)));

    if (m_timeline) {
        int w = static_cast<int>(m_timeline->duration() * m_pixelsPerMs) + 200;
        m_canvas->setFixedWidth(w);
    }
}

void TimelinePanel::zoomOut()
{
    m_pixelsPerMs = std::max(0.01, m_pixelsPerMs / 1.3);
    m_ruler->setPixelsPerMs(m_pixelsPerMs);
    m_canvas->setPixelsPerMs(m_pixelsPerMs);
    m_zoomLabel->setText(QString("%1%").arg(static_cast<int>(m_pixelsPerMs * 1000)));

    if (m_timeline) {
        int w = static_cast<int>(m_timeline->duration() * m_pixelsPerMs) + 200;
        m_canvas->setFixedWidth(w);
    }
}

void TimelinePanel::fitToWindow()
{
    if (!m_timeline || m_scrollArea->width() <= 0)
        return;

    m_pixelsPerMs = static_cast<qreal>(m_scrollArea->width() - 20) / m_timeline->duration();
    m_ruler->setPixelsPerMs(m_pixelsPerMs);
    m_canvas->setPixelsPerMs(m_pixelsPerMs);
    m_zoomLabel->setText(QString("%1%").arg(static_cast<int>(m_pixelsPerMs * 1000)));
    m_canvas->setFixedWidth(m_scrollArea->width() - 2);
}

void TimelinePanel::updateHeaders()
{
    // Clear existing headers
    QLayoutItem *item;
    while ((item = m_headersLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (!m_timeline)
        return;

    for (int i = 0; i < m_timeline->trackCount(); ++i) {
        auto *header = new TimelineTrackHeader(i, m_timeline->track(i));
        connect(header, &TimelineTrackHeader::visibilityToggled,
                [this](int idx, bool vis) { m_timeline->setTrackVisible(idx, vis); });
        connect(header, &TimelineTrackHeader::lockToggled,
                [this](int idx, bool locked) { m_timeline->setTrackLocked(idx, locked); });
        connect(header, &TimelineTrackHeader::muteToggled,
                [this](int idx, bool muted) { m_timeline->setTrackMuted(idx, muted); });
        m_headersLayout->addWidget(header);
    }
}

void TimelinePanel::onScroll(int value)
{
    m_ruler->setScrollOffset(value);
    m_canvas->setScrollOffset(value);
}
