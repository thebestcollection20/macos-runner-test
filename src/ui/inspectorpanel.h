#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QGroupBox>
#include "core/clip.h"

class InspectorPanel : public QWidget {
    Q_OBJECT

public:
    explicit InspectorPanel(QWidget *parent = nullptr);

    void setClip(Clip *clip);
    void clearClip();

signals:
    void clipChanged();

private slots:
    void onPropertyChanged();

private:
    void updateFromClip();
    QGroupBox *createTransformGroup();
    QGroupBox *createMediaGroup();

    Clip *m_clip = nullptr;

    // Transform
    QSpinBox *m_startTime;
    QSpinBox *m_duration;
    QSpinBox *m_trimIn;
    QSpinBox *m_trimOut;

    // Media
    QDoubleSpinBox *m_opacity;
    QDoubleSpinBox *m_volume;
    QLabel *m_nameLabel;
    QLabel *m_typeLabel;
    QLabel *m_sourceLabel;
};
