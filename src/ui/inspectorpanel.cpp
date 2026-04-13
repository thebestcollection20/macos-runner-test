#include "inspectorpanel.h"
#include <QScrollArea>

InspectorPanel::InspectorPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    auto *content = new QWidget;
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel("INSPECTOR");
    titleLabel->setObjectName("sectionLabel");
    layout->addWidget(titleLabel);

    // Clip info
    auto *infoGroup = new QGroupBox("Clip Info");
    auto *infoLayout = new QVBoxLayout(infoGroup);
    m_nameLabel = new QLabel("No clip selected");
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    m_typeLabel = new QLabel("");
    m_typeLabel->setStyleSheet("color: #969696; font-size: 10px;");
    m_sourceLabel = new QLabel("");
    m_sourceLabel->setStyleSheet("color: #696969; font-size: 9px;");
    m_sourceLabel->setWordWrap(true);
    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_typeLabel);
    infoLayout->addWidget(m_sourceLabel);
    layout->addWidget(infoGroup);

    layout->addWidget(createTransformGroup());
    layout->addWidget(createMediaGroup());
    layout->addStretch();

    scrollArea->setWidget(content);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);

    clearClip();
}

QGroupBox *InspectorPanel::createTransformGroup()
{
    auto *group = new QGroupBox("Timing");
    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("Start:"));
    m_startTime = new QSpinBox;
    m_startTime->setRange(0, 999999);
    m_startTime->setSuffix(" ms");
    row1->addWidget(m_startTime);

    row1->addWidget(new QLabel("Duration:"));
    m_duration = new QSpinBox;
    m_duration->setRange(100, 999999);
    m_duration->setSuffix(" ms");
    row1->addWidget(m_duration);

    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("Trim In:"));
    m_trimIn = new QSpinBox;
    m_trimIn->setRange(0, 999999);
    m_trimIn->setSuffix(" ms");
    row2->addWidget(m_trimIn);

    row2->addWidget(new QLabel("Trim Out:"));
    m_trimOut = new QSpinBox;
    m_trimOut->setRange(0, 999999);
    m_trimOut->setSuffix(" ms");
    row2->addWidget(m_trimOut);

    layout->addLayout(row1);
    layout->addLayout(row2);

    connect(m_startTime, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    connect(m_duration, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    connect(m_trimIn, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    connect(m_trimOut, QOverload<int>::of(&QSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);

    return group;
}

QGroupBox *InspectorPanel::createMediaGroup()
{
    auto *group = new QGroupBox("Properties");
    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("Opacity:"));
    m_opacity = new QDoubleSpinBox;
    m_opacity->setRange(0.0, 1.0);
    m_opacity->setSingleStep(0.05);
    m_opacity->setValue(1.0);
    m_opacity->setDecimals(2);
    row1->addWidget(m_opacity);

    row1->addWidget(new QLabel("Volume:"));
    m_volume = new QDoubleSpinBox;
    m_volume->setRange(0.0, 2.0);
    m_volume->setSingleStep(0.05);
    m_volume->setValue(1.0);
    m_volume->setDecimals(2);
    row1->addWidget(m_volume);

    layout->addLayout(row1);

    connect(m_opacity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);
    connect(m_volume, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InspectorPanel::onPropertyChanged);

    return group;
}

void InspectorPanel::setClip(Clip *clip)
{
    m_clip = clip;
    setEnabled(true);
    updateFromClip();
}

void InspectorPanel::clearClip()
{
    m_clip = nullptr;
    setEnabled(false);
    m_nameLabel->setText("No clip selected");
    m_typeLabel->setText("");
    m_sourceLabel->setText("");
}

void InspectorPanel::updateFromClip()
{
    if (!m_clip) return;

    m_startTime->blockSignals(true);
    m_duration->blockSignals(true);
    m_trimIn->blockSignals(true);
    m_trimOut->blockSignals(true);
    m_opacity->blockSignals(true);
    m_volume->blockSignals(true);

    m_nameLabel->setText(m_clip->name());

    QString typeStr;
    switch (m_clip->type()) {
    case ClipType::Video: typeStr = "Video Clip"; break;
    case ClipType::Audio: typeStr = "Audio Clip"; break;
    case ClipType::Image: typeStr = "Image"; break;
    case ClipType::Caption: typeStr = "Caption"; break;
    case ClipType::Transition: typeStr = "Transition"; break;
    }
    m_typeLabel->setText(typeStr);
    m_sourceLabel->setText(m_clip->sourcePath());

    m_startTime->setValue(static_cast<int>(m_clip->startTime()));
    m_duration->setValue(static_cast<int>(m_clip->duration()));
    m_trimIn->setValue(static_cast<int>(m_clip->trimIn()));
    m_trimOut->setValue(static_cast<int>(m_clip->trimOut()));
    m_opacity->setValue(m_clip->opacity());
    m_volume->setValue(m_clip->volume());

    m_startTime->blockSignals(false);
    m_duration->blockSignals(false);
    m_trimIn->blockSignals(false);
    m_trimOut->blockSignals(false);
    m_opacity->blockSignals(false);
    m_volume->blockSignals(false);
}

void InspectorPanel::onPropertyChanged()
{
    if (!m_clip) return;

    m_clip->setStartTime(m_startTime->value());
    m_clip->setDuration(m_duration->value());
    m_clip->setTrimIn(m_trimIn->value());
    m_clip->setTrimOut(m_trimOut->value());
    m_clip->setOpacity(m_opacity->value());
    m_clip->setVolume(m_volume->value());

    emit clipChanged();
}
