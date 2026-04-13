#include "captioneditor.h"
#include <QFontDatabase>
#include <QScrollArea>

// ColorButton
ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QPushButton(parent)
    , m_color(color)
{
    setFixedSize(32, 22);
    setCursor(Qt::PointingHandCursor);
    updateStyle();

    connect(this, &QPushButton::clicked, [this]() {
        QColor newColor = QColorDialog::getColor(m_color, this, "Choose Color",
                                                  QColorDialog::ShowAlphaChannel);
        if (newColor.isValid()) {
            setColor(newColor);
            emit colorChanged(newColor);
        }
    });
}

void ColorButton::setColor(const QColor &color)
{
    m_color = color;
    updateStyle();
}

void ColorButton::updateStyle()
{
    setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 1px solid #555; border-radius: 3px; }"
        "QPushButton:hover { border-color: #888; }")
        .arg(m_color.name()));
}

// CaptionEditor
CaptionEditor::CaptionEditor(QWidget *parent)
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

    // Title
    auto *titleLabel = new QLabel("CAPTION EDITOR");
    titleLabel->setObjectName("sectionLabel");
    layout->addWidget(titleLabel);

    layout->addWidget(createTextGroup());
    layout->addWidget(createFontGroup());
    layout->addWidget(createColorsGroup());
    layout->addWidget(createEffectsGroup());
    layout->addWidget(createPositionGroup());
    layout->addWidget(createAnimationGroup());

    // Save as Preset button
    m_savePresetBtn = new QPushButton("Save as Preset");
    m_savePresetBtn->setObjectName("accentButton");
    m_savePresetBtn->setFixedHeight(28);
    layout->addWidget(m_savePresetBtn);

    layout->addStretch();

    scrollArea->setWidget(content);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);

    connect(m_savePresetBtn, &QPushButton::clicked, [this]() {
        if (m_caption)
            emit presetSaveRequested(m_caption->style(), "Custom Preset");
    });

    clearCaption();
}

QGroupBox *CaptionEditor::createTextGroup()
{
    auto *group = new QGroupBox("Text");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    m_textEdit = new QTextEdit;
    m_textEdit->setMaximumHeight(80);
    m_textEdit->setPlaceholderText("Enter caption text...");
    layout->addWidget(m_textEdit);

    auto *timingLayout = new QHBoxLayout;
    timingLayout->addWidget(new QLabel("Start:"));
    m_startTime = new QSpinBox;
    m_startTime->setRange(0, 999999);
    m_startTime->setSuffix(" ms");
    timingLayout->addWidget(m_startTime);

    timingLayout->addWidget(new QLabel("Duration:"));
    m_duration = new QSpinBox;
    m_duration->setRange(100, 999999);
    m_duration->setSuffix(" ms");
    timingLayout->addWidget(m_duration);

    layout->addLayout(timingLayout);

    connect(m_textEdit, &QTextEdit::textChanged, this, &CaptionEditor::onTextChanged);
    connect(m_startTime, QOverload<int>::of(&QSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_duration, QOverload<int>::of(&QSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);

    return group;
}

QGroupBox *CaptionEditor::createFontGroup()
{
    auto *group = new QGroupBox("Font");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    m_fontFamily = new QComboBox;
    QFontDatabase fontDb;
    m_fontFamily->addItems(fontDb.families());
    m_fontFamily->setCurrentText("Arial");
    m_fontFamily->setMaximumWidth(160);

    m_fontSize = new QSpinBox;
    m_fontSize->setRange(8, 200);
    m_fontSize->setValue(32);
    m_fontSize->setSuffix(" pt");

    row1->addWidget(m_fontFamily, 1);
    row1->addWidget(m_fontSize);

    auto *row2 = new QHBoxLayout;
    m_boldBtn = new QPushButton("B");
    m_boldBtn->setFixedSize(28, 24);
    m_boldBtn->setCheckable(true);
    m_boldBtn->setChecked(true);
    m_boldBtn->setStyleSheet("QPushButton { font-weight: bold; }");

    m_italicBtn = new QPushButton("I");
    m_italicBtn->setFixedSize(28, 24);
    m_italicBtn->setCheckable(true);
    m_italicBtn->setStyleSheet("QPushButton { font-style: italic; }");

    m_underlineBtn = new QPushButton("U");
    m_underlineBtn->setFixedSize(28, 24);
    m_underlineBtn->setCheckable(true);
    m_underlineBtn->setStyleSheet("QPushButton { text-decoration: underline; }");

    m_allCapsBtn = new QPushButton("AA");
    m_allCapsBtn->setFixedSize(28, 24);
    m_allCapsBtn->setCheckable(true);
    m_allCapsBtn->setToolTip("All Caps");

    row2->addWidget(m_boldBtn);
    row2->addWidget(m_italicBtn);
    row2->addWidget(m_underlineBtn);
    row2->addWidget(m_allCapsBtn);
    row2->addStretch();

    auto *row3 = new QHBoxLayout;
    row3->addWidget(new QLabel("Spacing:"));
    m_letterSpacing = new QDoubleSpinBox;
    m_letterSpacing->setRange(-10.0, 50.0);
    m_letterSpacing->setSingleStep(0.5);
    row3->addWidget(m_letterSpacing);

    row3->addWidget(new QLabel("Line:"));
    m_lineSpacing = new QDoubleSpinBox;
    m_lineSpacing->setRange(0.5, 5.0);
    m_lineSpacing->setSingleStep(0.1);
    m_lineSpacing->setValue(1.2);
    row3->addWidget(m_lineSpacing);

    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addLayout(row3);

    connect(m_fontFamily, &QComboBox::currentTextChanged, this, &CaptionEditor::onFontChanged);
    connect(m_fontSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &CaptionEditor::onFontChanged);
    connect(m_boldBtn, &QPushButton::toggled, this, &CaptionEditor::onFontChanged);
    connect(m_italicBtn, &QPushButton::toggled, this, &CaptionEditor::onStyleChanged);
    connect(m_underlineBtn, &QPushButton::toggled, this, &CaptionEditor::onStyleChanged);
    connect(m_allCapsBtn, &QPushButton::toggled, this, &CaptionEditor::onStyleChanged);
    connect(m_letterSpacing, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_lineSpacing, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);

    return group;
}

QGroupBox *CaptionEditor::createColorsGroup()
{
    auto *group = new QGroupBox("Colors");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("Text:"));
    m_textColorBtn = new ColorButton(Qt::white);
    row1->addWidget(m_textColorBtn);
    row1->addSpacing(8);
    row1->addWidget(new QLabel("Outline:"));
    m_outlineColorBtn = new ColorButton(Qt::black);
    row1->addWidget(m_outlineColorBtn);
    row1->addStretch();

    auto *row2 = new QHBoxLayout;
    m_showBgCheck = new QCheckBox("Background:");
    m_showBgCheck->setChecked(true);
    row2->addWidget(m_showBgCheck);
    m_bgColorBtn = new ColorButton(QColor(0, 0, 0, 180));
    row2->addWidget(m_bgColorBtn);
    row2->addSpacing(8);
    row2->addWidget(new QLabel("Shadow:"));
    m_shadowColorBtn = new ColorButton(QColor(0, 0, 0, 128));
    row2->addWidget(m_shadowColorBtn);
    row2->addStretch();

    layout->addLayout(row1);
    layout->addLayout(row2);

    connect(m_textColorBtn, &ColorButton::colorChanged, this, &CaptionEditor::onStyleChanged);
    connect(m_bgColorBtn, &ColorButton::colorChanged, this, &CaptionEditor::onStyleChanged);
    connect(m_outlineColorBtn, &ColorButton::colorChanged, this, &CaptionEditor::onStyleChanged);
    connect(m_shadowColorBtn, &ColorButton::colorChanged, this, &CaptionEditor::onStyleChanged);
    connect(m_showBgCheck, &QCheckBox::toggled, this, &CaptionEditor::onStyleChanged);

    return group;
}

QGroupBox *CaptionEditor::createEffectsGroup()
{
    auto *group = new QGroupBox("Effects");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("Outline:"));
    m_outlineWidth = new QDoubleSpinBox;
    m_outlineWidth->setRange(0, 20);
    m_outlineWidth->setSingleStep(0.5);
    m_outlineWidth->setValue(2.0);
    row1->addWidget(m_outlineWidth);

    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("Shadow X:"));
    m_shadowOffsetX = new QDoubleSpinBox;
    m_shadowOffsetX->setRange(-20, 20);
    m_shadowOffsetX->setSingleStep(0.5);
    row2->addWidget(m_shadowOffsetX);

    row2->addWidget(new QLabel("Y:"));
    m_shadowOffsetY = new QDoubleSpinBox;
    m_shadowOffsetY->setRange(-20, 20);
    m_shadowOffsetY->setSingleStep(0.5);
    row2->addWidget(m_shadowOffsetY);

    row2->addWidget(new QLabel("Blur:"));
    m_shadowBlur = new QDoubleSpinBox;
    m_shadowBlur->setRange(0, 30);
    m_shadowBlur->setSingleStep(0.5);
    row2->addWidget(m_shadowBlur);

    auto *row3 = new QHBoxLayout;
    row3->addWidget(new QLabel("BG Pad:"));
    m_bgPadding = new QDoubleSpinBox;
    m_bgPadding->setRange(0, 50);
    m_bgPadding->setSingleStep(1.0);
    row3->addWidget(m_bgPadding);

    row3->addWidget(new QLabel("Radius:"));
    m_bgRadius = new QDoubleSpinBox;
    m_bgRadius->setRange(0, 30);
    m_bgRadius->setSingleStep(1.0);
    row3->addWidget(m_bgRadius);

    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addLayout(row3);

    connect(m_outlineWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_shadowOffsetX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_shadowOffsetY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_shadowBlur, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_bgPadding, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);
    connect(m_bgRadius, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onStyleChanged);

    return group;
}

QGroupBox *CaptionEditor::createPositionGroup()
{
    auto *group = new QGroupBox("Position");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("H Align:"));
    m_alignH = new QComboBox;
    m_alignH->addItems({"Left", "Center", "Right"});
    m_alignH->setCurrentIndex(1);
    row1->addWidget(m_alignH);

    row1->addWidget(new QLabel("V Align:"));
    m_alignV = new QComboBox;
    m_alignV->addItems({"Top", "Center", "Bottom"});
    m_alignV->setCurrentIndex(2);
    row1->addWidget(m_alignV);

    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("X:"));
    m_posX = new QDoubleSpinBox;
    m_posX->setRange(0.0, 1.0);
    m_posX->setSingleStep(0.05);
    m_posX->setValue(0.5);
    m_posX->setDecimals(2);
    row2->addWidget(m_posX);

    row2->addWidget(new QLabel("Y:"));
    m_posY = new QDoubleSpinBox;
    m_posY->setRange(0.0, 1.0);
    m_posY->setSingleStep(0.05);
    m_posY->setValue(0.85);
    m_posY->setDecimals(2);
    row2->addWidget(m_posY);

    layout->addLayout(row1);
    layout->addLayout(row2);

    connect(m_alignH, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CaptionEditor::onPositionChanged);
    connect(m_alignV, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CaptionEditor::onPositionChanged);
    connect(m_posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onPositionChanged);
    connect(m_posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onPositionChanged);

    return group;
}

QGroupBox *CaptionEditor::createAnimationGroup()
{
    auto *group = new QGroupBox("Animation");

    auto *layout = new QVBoxLayout(group);
    layout->setSpacing(4);

    auto *row1 = new QHBoxLayout;
    row1->addWidget(new QLabel("In:"));
    m_animIn = new QComboBox;
    m_animIn->addItems({"None", "Fade In", "Fade Out", "Slide Up", "Slide Down",
                         "TypeWriter", "Word by Word", "Pop"});
    row1->addWidget(m_animIn);

    row1->addWidget(new QLabel("Out:"));
    m_animOut = new QComboBox;
    m_animOut->addItems({"None", "Fade In", "Fade Out", "Slide Up", "Slide Down",
                          "TypeWriter", "Word by Word", "Pop"});
    row1->addWidget(m_animOut);

    auto *row2 = new QHBoxLayout;
    row2->addWidget(new QLabel("Duration:"));
    m_animDuration = new QDoubleSpinBox;
    m_animDuration->setRange(0, 5000);
    m_animDuration->setSuffix(" ms");
    m_animDuration->setValue(300);
    row2->addWidget(m_animDuration);
    row2->addStretch();

    layout->addLayout(row1);
    layout->addLayout(row2);

    connect(m_animIn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CaptionEditor::onAnimationChanged);
    connect(m_animOut, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CaptionEditor::onAnimationChanged);
    connect(m_animDuration, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CaptionEditor::onAnimationChanged);

    return group;
}

void CaptionEditor::setCaption(Caption *caption)
{
    m_caption = caption;
    setEnabled(true);
    updateFromCaption();
}

void CaptionEditor::clearCaption()
{
    m_caption = nullptr;
    setEnabled(false);
}

void CaptionEditor::applyBrandKit(const BrandKit &kit)
{
    if (!m_caption)
        return;

    CaptionStyle style = m_caption->style();
    if (!kit.primaryFontFamily().isEmpty()) {
        style.font.setFamily(kit.primaryFontFamily());
    }
    style.textColor = kit.primaryColor();
    m_caption->setStyle(style);
    updateFromCaption();
    emit captionChanged();
}

void CaptionEditor::updateFromCaption()
{
    if (!m_caption)
        return;

    blockSignalsForAll(true);

    const CaptionStyle &s = m_caption->style();

    m_textEdit->setPlainText(m_caption->text());
    m_startTime->setValue(static_cast<int>(m_caption->startTime()));
    m_duration->setValue(static_cast<int>(m_caption->duration()));

    m_fontFamily->setCurrentText(s.font.family());
    m_fontSize->setValue(s.font.pointSize());
    m_boldBtn->setChecked(s.font.bold());
    m_italicBtn->setChecked(s.italic);
    m_underlineBtn->setChecked(s.underline);
    m_allCapsBtn->setChecked(s.allCaps);
    m_letterSpacing->setValue(s.letterSpacing);
    m_lineSpacing->setValue(s.lineSpacing);

    m_textColorBtn->setColor(s.textColor);
    m_bgColorBtn->setColor(s.backgroundColor);
    m_outlineColorBtn->setColor(s.outlineColor);
    m_shadowColorBtn->setColor(s.shadowColor);
    m_showBgCheck->setChecked(s.showBackground);

    m_outlineWidth->setValue(s.outlineWidth);
    m_shadowOffsetX->setValue(s.shadowOffsetX);
    m_shadowOffsetY->setValue(s.shadowOffsetY);
    m_shadowBlur->setValue(s.shadowBlur);
    m_bgPadding->setValue(s.backgroundPadding);
    m_bgRadius->setValue(s.backgroundRadius);

    m_posX->setValue(m_caption->position().x());
    m_posY->setValue(m_caption->position().y());

    m_animIn->setCurrentIndex(static_cast<int>(m_caption->animationIn()));
    m_animOut->setCurrentIndex(static_cast<int>(m_caption->animationOut()));
    m_animDuration->setValue(m_caption->animationDuration());

    blockSignalsForAll(false);
}

void CaptionEditor::blockSignalsForAll(bool block)
{
    m_textEdit->blockSignals(block);
    m_fontFamily->blockSignals(block);
    m_fontSize->blockSignals(block);
    m_boldBtn->blockSignals(block);
    m_italicBtn->blockSignals(block);
    m_underlineBtn->blockSignals(block);
    m_allCapsBtn->blockSignals(block);
    m_letterSpacing->blockSignals(block);
    m_lineSpacing->blockSignals(block);
    m_textColorBtn->blockSignals(block);
    m_bgColorBtn->blockSignals(block);
    m_outlineColorBtn->blockSignals(block);
    m_shadowColorBtn->blockSignals(block);
    m_showBgCheck->blockSignals(block);
    m_outlineWidth->blockSignals(block);
    m_shadowOffsetX->blockSignals(block);
    m_shadowOffsetY->blockSignals(block);
    m_shadowBlur->blockSignals(block);
    m_bgPadding->blockSignals(block);
    m_bgRadius->blockSignals(block);
    m_posX->blockSignals(block);
    m_posY->blockSignals(block);
    m_alignH->blockSignals(block);
    m_alignV->blockSignals(block);
    m_animIn->blockSignals(block);
    m_animOut->blockSignals(block);
    m_animDuration->blockSignals(block);
    m_startTime->blockSignals(block);
    m_duration->blockSignals(block);
}

void CaptionEditor::onTextChanged()
{
    if (!m_caption) return;
    m_caption->setText(m_textEdit->toPlainText());
    emit captionChanged();
}

void CaptionEditor::onFontChanged()
{
    if (!m_caption) return;
    CaptionStyle style = m_caption->style();
    style.font = QFont(m_fontFamily->currentText(), m_fontSize->value(),
                       m_boldBtn->isChecked() ? QFont::Bold : QFont::Normal);
    m_caption->setStyle(style);
    emit captionChanged();
}

void CaptionEditor::onStyleChanged()
{
    if (!m_caption) return;

    CaptionStyle style = m_caption->style();
    style.font = QFont(m_fontFamily->currentText(), m_fontSize->value(),
                       m_boldBtn->isChecked() ? QFont::Bold : QFont::Normal);
    style.italic = m_italicBtn->isChecked();
    style.underline = m_underlineBtn->isChecked();
    style.allCaps = m_allCapsBtn->isChecked();
    style.letterSpacing = m_letterSpacing->value();
    style.lineSpacing = m_lineSpacing->value();
    style.textColor = m_textColorBtn->color();
    style.backgroundColor = m_bgColorBtn->color();
    style.outlineColor = m_outlineColorBtn->color();
    style.shadowColor = m_shadowColorBtn->color();
    style.showBackground = m_showBgCheck->isChecked();
    style.outlineWidth = m_outlineWidth->value();
    style.shadowOffsetX = m_shadowOffsetX->value();
    style.shadowOffsetY = m_shadowOffsetY->value();
    style.shadowBlur = m_shadowBlur->value();
    style.backgroundPadding = m_bgPadding->value();
    style.backgroundRadius = m_bgRadius->value();
    m_caption->setStyle(style);

    m_caption->setStartTime(m_startTime->value());
    m_caption->setDuration(m_duration->value());

    emit captionChanged();
}

void CaptionEditor::onPositionChanged()
{
    if (!m_caption) return;
    m_caption->setPosition(QPointF(m_posX->value(), m_posY->value()));

    // Compute alignment
    int align = 0;
    switch (m_alignH->currentIndex()) {
    case 0: align |= Qt::AlignLeft; m_posX->setValue(0.15); break;
    case 1: align |= Qt::AlignHCenter; m_posX->setValue(0.5); break;
    case 2: align |= Qt::AlignRight; m_posX->setValue(0.85); break;
    }
    switch (m_alignV->currentIndex()) {
    case 0: align |= Qt::AlignTop; m_posY->setValue(0.15); break;
    case 1: align |= Qt::AlignVCenter; m_posY->setValue(0.5); break;
    case 2: align |= Qt::AlignBottom; m_posY->setValue(0.85); break;
    }

    CaptionStyle style = m_caption->style();
    style.alignment = align;
    m_caption->setStyle(style);

    emit captionChanged();
}

void CaptionEditor::onAnimationChanged()
{
    if (!m_caption) return;
    m_caption->setAnimationIn(static_cast<CaptionAnimation>(m_animIn->currentIndex()));
    m_caption->setAnimationOut(static_cast<CaptionAnimation>(m_animOut->currentIndex()));
    m_caption->setAnimationDuration(m_animDuration->value());
    emit captionChanged();
}
