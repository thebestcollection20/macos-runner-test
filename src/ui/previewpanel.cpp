#include "previewpanel.h"
#include "theme.h"
#include <QPainterPath>

// VideoCanvas
VideoCanvas::VideoCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: #0a0a0a;");
}

void VideoCanvas::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // Calculate letterboxed area
    QRect area = rect();
    qreal aspectRatio = static_cast<qreal>(m_resolution.width()) / m_resolution.height();
    qreal widgetRatio = static_cast<qreal>(area.width()) / area.height();

    QRect videoArea;
    if (widgetRatio > aspectRatio) {
        int w = static_cast<int>(area.height() * aspectRatio);
        videoArea = QRect((area.width() - w) / 2, 0, w, area.height());
    } else {
        int h = static_cast<int>(area.width() / aspectRatio);
        videoArea = QRect(0, (area.height() - h) / 2, area.width(), h);
    }

    drawBackground(p, videoArea);

    if (m_timeline) {
        drawCaptions(p, videoArea);
    }
}

void VideoCanvas::drawBackground(QPainter &p, const QRect &area)
{
    // Dark video area
    p.fillRect(rect(), QColor(10, 10, 10));
    p.fillRect(area, QColor(25, 25, 30));

    // Grid pattern
    p.setPen(QPen(QColor(35, 35, 40), 1));
    int gridSize = area.width() / 20;
    if (gridSize > 5) {
        for (int x = area.left(); x <= area.right(); x += gridSize)
            p.drawLine(x, area.top(), x, area.bottom());
        for (int y = area.top(); y <= area.bottom(); y += gridSize)
            p.drawLine(area.left(), y, area.right(), y);
    }
}

void VideoCanvas::drawCaptions(QPainter &p, const QRect &area)
{
    auto captions = m_timeline->captionsAtTime(m_currentTime);
    for (const auto &cap : captions) {
        drawCaption(p, cap, area);
    }
}

void VideoCanvas::drawCaption(QPainter &p, const Caption &cap, const QRect &area)
{
    const CaptionStyle &style = cap.style();

    // Set font
    QFont font = style.font;
    qreal scale = static_cast<qreal>(area.width()) / m_resolution.width();
    font.setPointSizeF(font.pointSizeF() * scale);
    if (style.italic) font.setItalic(true);
    if (style.underline) font.setUnderline(true);
    p.setFont(font);

    QString text = style.allCaps ? cap.text().toUpper() : cap.text();

    // Calculate position
    QFontMetrics fm(font);
    QRect textBounds = fm.boundingRect(QRect(0, 0, area.width() * 0.9, 0),
                                        Qt::TextWordWrap | (style.alignment & Qt::AlignHorizontal_Mask),
                                        text);

    int x = area.left() + static_cast<int>(cap.position().x() * area.width()) - textBounds.width() / 2;
    int y = area.top() + static_cast<int>(cap.position().y() * area.height()) - textBounds.height() / 2;

    QRect drawRect(x, y, textBounds.width(), textBounds.height());

    // Draw background
    if (style.showBackground) {
        int pad = static_cast<int>(style.backgroundPadding * scale);
        QRect bgRect = drawRect.adjusted(-pad, -pad, pad, pad);
        QPainterPath path;
        path.addRoundedRect(bgRect, style.backgroundRadius * scale, style.backgroundRadius * scale);
        p.fillPath(path, style.backgroundColor);
    }

    // Draw shadow
    if (style.shadowColor.alpha() > 0) {
        p.setPen(style.shadowColor);
        QRect shadowRect = drawRect.translated(
            static_cast<int>(style.shadowOffsetX * scale),
            static_cast<int>(style.shadowOffsetY * scale));
        p.drawText(shadowRect, Qt::TextWordWrap | (style.alignment & Qt::AlignHorizontal_Mask), text);
    }

    // Draw outline
    if (style.outlineWidth > 0) {
        QPen outlinePen(style.outlineColor, style.outlineWidth * scale);
        p.setPen(outlinePen);
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                QRect oRect = drawRect.translated(
                    static_cast<int>(dx * style.outlineWidth * scale),
                    static_cast<int>(dy * style.outlineWidth * scale));
                p.drawText(oRect, Qt::TextWordWrap | (style.alignment & Qt::AlignHorizontal_Mask), text);
            }
        }
    }

    // Draw text
    p.setPen(style.textColor);
    p.drawText(drawRect, Qt::TextWordWrap | (style.alignment & Qt::AlignHorizontal_Mask), text);
}

// PreviewPanel
PreviewPanel::PreviewPanel(QWidget *parent)
    : QWidget(parent)
    , m_canvas(new VideoCanvas(this))
    , m_playTimer(new QTimer(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Canvas
    layout->addWidget(m_canvas, 1);

    // Transport bar
    auto *transportWidget = new QWidget;
    transportWidget->setStyleSheet("background-color: #141414; border-top: 1px solid #373737;");
    auto *transportLayout = new QVBoxLayout(transportWidget);
    transportLayout->setContentsMargins(8, 4, 8, 4);
    transportLayout->setSpacing(4);

    // Seek slider
    m_seekSlider = new QSlider(Qt::Horizontal);
    m_seekSlider->setRange(0, 30000);
    transportLayout->addWidget(m_seekSlider);

    // Controls
    auto *controlsLayout = new QHBoxLayout;
    controlsLayout->setSpacing(4);

    m_timeLabel = new QLabel("00:00:00:00");
    m_timeLabel->setStyleSheet("color: #488ce6; font-family: 'Courier New'; font-size: 12px; font-weight: bold;");

    m_prevFrameBtn = new QPushButton("|<");
    m_prevFrameBtn->setFixedSize(28, 24);
    m_prevFrameBtn->setToolTip("Previous Frame");

    m_stopBtn = new QPushButton("[]");
    m_stopBtn->setFixedSize(28, 24);
    m_stopBtn->setToolTip("Stop");

    m_playBtn = new QPushButton(">");
    m_playBtn->setFixedSize(36, 24);
    m_playBtn->setToolTip("Play/Pause");
    m_playBtn->setObjectName("accentButton");

    m_nextFrameBtn = new QPushButton(">|");
    m_nextFrameBtn->setFixedSize(28, 24);
    m_nextFrameBtn->setToolTip("Next Frame");

    m_loopBtn = new QPushButton("O");
    m_loopBtn->setFixedSize(28, 24);
    m_loopBtn->setToolTip("Loop");
    m_loopBtn->setCheckable(true);

    m_durationLabel = new QLabel("00:00:30:00");
    m_durationLabel->setStyleSheet("color: #969696; font-family: 'Courier New'; font-size: 12px;");

    controlsLayout->addWidget(m_timeLabel);
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_prevFrameBtn);
    controlsLayout->addWidget(m_stopBtn);
    controlsLayout->addWidget(m_playBtn);
    controlsLayout->addWidget(m_nextFrameBtn);
    controlsLayout->addWidget(m_loopBtn);
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_durationLabel);

    transportLayout->addLayout(controlsLayout);
    layout->addWidget(transportWidget);

    // Connect signals
    connect(m_playBtn, &QPushButton::clicked, this, &PreviewPanel::togglePlay);
    connect(m_stopBtn, &QPushButton::clicked, this, &PreviewPanel::stop);
    connect(m_prevFrameBtn, &QPushButton::clicked, this, &PreviewPanel::stepBackward);
    connect(m_nextFrameBtn, &QPushButton::clicked, this, &PreviewPanel::stepForward);
    connect(m_loopBtn, &QPushButton::toggled, [this](bool checked) { m_looping = checked; });
    connect(m_seekSlider, &QSlider::sliderMoved, this, &PreviewPanel::onSliderMoved);
    connect(m_playTimer, &QTimer::timeout, this, &PreviewPanel::onTimerTick);
}

void PreviewPanel::setTimeline(Timeline *timeline)
{
    m_timeline = timeline;
    m_canvas->setTimeline(timeline);

    if (m_timeline) {
        connect(m_timeline, &Timeline::timeChanged, [this](qint64 time) {
            m_canvas->setCurrentTime(time);
            m_seekSlider->setValue(static_cast<int>(time));
            updateTimeDisplay();
        });
        connect(m_timeline, &Timeline::durationChanged, [this](qint64 dur) {
            m_seekSlider->setRange(0, static_cast<int>(dur));
            m_durationLabel->setText(formatTime(dur));
        });
        m_seekSlider->setRange(0, static_cast<int>(m_timeline->duration()));
        m_durationLabel->setText(formatTime(m_timeline->duration()));
    }
}

void PreviewPanel::play()
{
    m_playing = true;
    m_playBtn->setText("||");
    m_playTimer->start(m_frameInterval);
    emit playStateChanged(true);
}

void PreviewPanel::pause()
{
    m_playing = false;
    m_playBtn->setText(">");
    m_playTimer->stop();
    emit playStateChanged(false);
}

void PreviewPanel::stop()
{
    pause();
    if (m_timeline)
        m_timeline->setCurrentTime(0);
}

void PreviewPanel::togglePlay()
{
    if (m_playing)
        pause();
    else
        play();
}

void PreviewPanel::seekTo(qint64 time)
{
    if (m_timeline)
        m_timeline->setCurrentTime(time);
}

void PreviewPanel::stepForward()
{
    if (m_timeline)
        m_timeline->setCurrentTime(m_timeline->currentTime() + m_frameInterval);
}

void PreviewPanel::stepBackward()
{
    if (m_timeline)
        m_timeline->setCurrentTime(m_timeline->currentTime() - m_frameInterval);
}

void PreviewPanel::onTimerTick()
{
    if (!m_timeline)
        return;

    qint64 newTime = m_timeline->currentTime() + m_frameInterval;
    if (newTime >= m_timeline->duration()) {
        if (m_looping) {
            newTime = 0;
        } else {
            pause();
            return;
        }
    }
    m_timeline->setCurrentTime(newTime);
}

void PreviewPanel::onSliderMoved(int value)
{
    if (m_timeline)
        m_timeline->setCurrentTime(value);
}

void PreviewPanel::updateTimeDisplay()
{
    if (m_timeline)
        m_timeLabel->setText(formatTime(m_timeline->currentTime()));
}

QString PreviewPanel::formatTime(qint64 ms) const
{
    int totalSec = static_cast<int>(ms / 1000);
    int hours = totalSec / 3600;
    int minutes = (totalSec % 3600) / 60;
    int seconds = totalSec % 60;
    int frames = static_cast<int>((ms % 1000) / m_frameInterval);
    return QString("%1:%2:%3:%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(frames, 2, 10, QChar('0'));
}
