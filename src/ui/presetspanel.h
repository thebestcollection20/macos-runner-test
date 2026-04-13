#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include "core/preset.h"

class PresetPreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit PresetPreviewWidget(const CaptionPreset &preset, QWidget *parent = nullptr);

    CaptionPreset preset() const { return m_preset; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void clicked(const CaptionPreset &preset);
    void doubleClicked(const CaptionPreset &preset);

private:
    CaptionPreset m_preset;
    bool m_hovered = false;
};

class PresetsPanel : public QWidget {
    Q_OBJECT

public:
    explicit PresetsPanel(QWidget *parent = nullptr);

    void addCustomPreset(const CaptionStyle &style, const QString &name);

signals:
    void presetSelected(const CaptionPreset &preset);
    void presetApplied(const CaptionPreset &preset);

private slots:
    void onCategoryChanged(int index);
    void onDeletePreset();

private:
    void refreshPresets();

    QComboBox *m_categoryFilter;
    QWidget *m_presetsContainer;
    QVBoxLayout *m_presetsLayout;
    QPushButton *m_deleteBtn;
    QLabel *m_countLabel;
    CaptionPreset m_selectedPreset;
};
