#include "presetspanel.h"
#include "theme.h"
#include <QScrollArea>
#include <QPainter>
#include <QPainterPath>
#include <QInputDialog>

// PresetPreviewWidget
PresetPreviewWidget::PresetPreviewWidget(const CaptionPreset &preset, QWidget *parent)
    : QWidget(parent)
    , m_preset(preset)
{
    setFixedHeight(60);
    setCursor(Qt::PointingHandCursor);
    setToolTip(preset.name());
}

void PresetPreviewWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // Background
    QColor bgColor = m_hovered ? QColor(42, 42, 42) : QColor(30, 30, 30);
    QPainterPath path;
    path.addRoundedRect(rect(), 4, 4);
    p.fillPath(path, bgColor);
    p.setPen(QPen(QColor(55, 55, 55), 1));
    p.drawPath(path);

    const CaptionStyle &style = m_preset.style();
    QString text = m_preset.previewText();
    if (style.allCaps) text = text.toUpper();

    // Scale font for preview
    QFont font = style.font;
    font.setPointSize(std::min(font.pointSize(), 16));
    if (style.italic) font.setItalic(true);
    p.setFont(font);

    // Draw preview text
    QRect textArea = rect().adjusted(10, 5, -10, -18);

    // Background box
    if (style.showBackground) {
        QFontMetrics fm(font);
        QRect textBounds = fm.boundingRect(textArea, Qt::AlignCenter, text);
        QPainterPath bgPath;
        bgPath.addRoundedRect(textBounds.adjusted(-4, -2, 4, 2),
                              style.backgroundRadius / 2, style.backgroundRadius / 2);
        p.fillPath(bgPath, style.backgroundColor);
    }

    // Outline
    if (style.outlineWidth > 0) {
        p.setPen(QPen(style.outlineColor, 1));
        p.drawText(textArea.translated(1, 1), Qt::AlignCenter, text);
    }

    // Text
    p.setPen(style.textColor);
    p.drawText(textArea, Qt::AlignCenter, text);

    // Preset name
    p.setPen(QColor(120, 120, 120));
    QFont nameFont("Segoe UI", 8);
    p.setFont(nameFont);
    p.drawText(rect().adjusted(10, 0, -10, -4), Qt::AlignBottom | Qt::AlignLeft, m_preset.name());

    // Category badge
    if (!m_preset.category().isEmpty()) {
        p.setPen(QColor(80, 80, 80));
        p.drawText(rect().adjusted(10, 0, -10, -4), Qt::AlignBottom | Qt::AlignRight,
                   m_preset.category());
    }
}

void PresetPreviewWidget::mousePressEvent(QMouseEvent *)
{
    emit clicked(m_preset);
}

void PresetPreviewWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked(m_preset);
}

// PresetsPanel
PresetsPanel::PresetsPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    auto *titleLabel = new QLabel("CAPTION PRESETS");
    titleLabel->setObjectName("sectionLabel");
    mainLayout->addWidget(titleLabel);

    // Category filter
    auto *filterLayout = new QHBoxLayout;
    m_categoryFilter = new QComboBox;
    m_categoryFilter->addItem("All Categories");
    filterLayout->addWidget(m_categoryFilter, 1);

    m_deleteBtn = new QPushButton("Delete");
    m_deleteBtn->setFixedHeight(22);
    m_deleteBtn->setEnabled(false);
    filterLayout->addWidget(m_deleteBtn);

    mainLayout->addLayout(filterLayout);

    // Presets scroll area
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    auto *scrollContent = new QWidget;
    m_presetsLayout = new QVBoxLayout(scrollContent);
    m_presetsLayout->setContentsMargins(0, 0, 0, 0);
    m_presetsLayout->setSpacing(4);
    m_presetsLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);

    // Count
    m_countLabel = new QLabel("0 presets");
    m_countLabel->setStyleSheet("color: #696969; font-size: 10px;");
    mainLayout->addWidget(m_countLabel);

    // Connections
    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PresetsPanel::onCategoryChanged);
    connect(m_deleteBtn, &QPushButton::clicked, this, &PresetsPanel::onDeletePreset);

    refreshPresets();
}

void PresetsPanel::addCustomPreset(const CaptionStyle &style, const QString &name)
{
    bool ok;
    QString presetName = QInputDialog::getText(this, "Save Preset", "Preset name:",
                                                QLineEdit::Normal, name, &ok);
    if (!ok || presetName.isEmpty()) return;

    CaptionPreset preset(presetName, style);
    preset.setCategory("Custom");
    PresetManager::instance().addPreset(preset);
    refreshPresets();
}

void PresetsPanel::onCategoryChanged(int)
{
    refreshPresets();
}

void PresetsPanel::onDeletePreset()
{
    if (m_selectedPreset.isBuiltIn()) return;
    PresetManager::instance().removePreset(m_selectedPreset.id());
    m_deleteBtn->setEnabled(false);
    refreshPresets();
}

void PresetsPanel::refreshPresets()
{
    // Clear existing
    QLayoutItem *item;
    while ((item = m_presetsLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            delete item->widget();
        delete item;
    }

    // Update categories
    m_categoryFilter->blockSignals(true);
    QString currentCat = m_categoryFilter->currentText();
    m_categoryFilter->clear();
    m_categoryFilter->addItem("All Categories");
    for (const auto &cat : PresetManager::instance().categories())
        m_categoryFilter->addItem(cat);
    int catIdx = m_categoryFilter->findText(currentCat);
    m_categoryFilter->setCurrentIndex(catIdx >= 0 ? catIdx : 0);
    m_categoryFilter->blockSignals(false);

    // Filter and display
    QString filter = m_categoryFilter->currentIndex() > 0 ? m_categoryFilter->currentText() : "";
    auto presets = filter.isEmpty() ? PresetManager::instance().presets()
                                    : PresetManager::instance().presetsInCategory(filter);

    int count = 0;
    for (const auto &preset : presets) {
        auto *previewWidget = new PresetPreviewWidget(preset);
        connect(previewWidget, &PresetPreviewWidget::clicked, [this](const CaptionPreset &p) {
            m_selectedPreset = p;
            m_deleteBtn->setEnabled(!p.isBuiltIn());
            emit presetSelected(p);
        });
        connect(previewWidget, &PresetPreviewWidget::doubleClicked, [this](const CaptionPreset &p) {
            emit presetApplied(p);
        });
        m_presetsLayout->addWidget(previewWidget);
        count++;
    }

    m_presetsLayout->addStretch();
    m_countLabel->setText(QString("%1 preset(s)").arg(count));
}
