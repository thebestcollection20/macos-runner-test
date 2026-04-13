#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QColorDialog>
#include "core/caption.h"
#include "core/brandkit.h"

class ColorButton : public QPushButton {
    Q_OBJECT

public:
    explicit ColorButton(const QColor &color = Qt::white, QWidget *parent = nullptr);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

signals:
    void colorChanged(const QColor &color);

private:
    void updateStyle();
    QColor m_color;
};

class CaptionEditor : public QWidget {
    Q_OBJECT

public:
    explicit CaptionEditor(QWidget *parent = nullptr);

    void setCaption(Caption *caption);
    void clearCaption();
    void applyBrandKit(const BrandKit &kit);

signals:
    void captionChanged();
    void presetSaveRequested(const CaptionStyle &style, const QString &name);

private slots:
    void onTextChanged();
    void onFontChanged();
    void onStyleChanged();
    void onPositionChanged();
    void onAnimationChanged();

private:
    void updateFromCaption();
    void blockSignalsForAll(bool block);
    QGroupBox *createTextGroup();
    QGroupBox *createFontGroup();
    QGroupBox *createColorsGroup();
    QGroupBox *createEffectsGroup();
    QGroupBox *createPositionGroup();
    QGroupBox *createAnimationGroup();

    Caption *m_caption = nullptr;

    // Text
    QTextEdit *m_textEdit;

    // Font
    QComboBox *m_fontFamily;
    QSpinBox *m_fontSize;
    QPushButton *m_boldBtn;
    QPushButton *m_italicBtn;
    QPushButton *m_underlineBtn;
    QPushButton *m_allCapsBtn;
    QDoubleSpinBox *m_letterSpacing;
    QDoubleSpinBox *m_lineSpacing;

    // Colors
    ColorButton *m_textColorBtn;
    ColorButton *m_bgColorBtn;
    ColorButton *m_outlineColorBtn;
    ColorButton *m_shadowColorBtn;
    QCheckBox *m_showBgCheck;

    // Effects
    QDoubleSpinBox *m_outlineWidth;
    QDoubleSpinBox *m_shadowOffsetX;
    QDoubleSpinBox *m_shadowOffsetY;
    QDoubleSpinBox *m_shadowBlur;
    QDoubleSpinBox *m_bgPadding;
    QDoubleSpinBox *m_bgRadius;

    // Position
    QComboBox *m_alignH;
    QComboBox *m_alignV;
    QDoubleSpinBox *m_posX;
    QDoubleSpinBox *m_posY;

    // Animation
    QComboBox *m_animIn;
    QComboBox *m_animOut;
    QDoubleSpinBox *m_animDuration;

    // Timing
    QSpinBox *m_startTime;
    QSpinBox *m_duration;

    // Save preset
    QPushButton *m_savePresetBtn;
};
