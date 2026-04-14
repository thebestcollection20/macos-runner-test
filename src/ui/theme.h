#pragma once

#include <QApplication>
#include <QPalette>
#include <QString>
#include <QColor>

namespace DaVinciTheme {

// Color constants matching DaVinci Resolve's dark UI
namespace Colors {
    const QColor Background(24, 24, 24);
    const QColor Surface(32, 32, 32);
    const QColor SurfaceLight(42, 42, 42);
    const QColor SurfaceHover(52, 52, 52);
    const QColor Panel(28, 28, 28);
    const QColor PanelHeader(20, 20, 20);
    const QColor Border(55, 55, 55);
    const QColor BorderLight(70, 70, 70);

    const QColor TextPrimary(210, 210, 210);
    const QColor TextSecondary(150, 150, 150);
    const QColor TextDisabled(80, 80, 80);

    const QColor Accent(72, 140, 230);       // Blue accent
    const QColor AccentHover(90, 155, 240);
    const QColor AccentDark(50, 110, 190);

    const QColor TimelineRuler(45, 45, 45);
    const QColor TimelineTrack(35, 35, 35);
    const QColor TimelineTrackAlt(38, 38, 38);
    const QColor Playhead(230, 60, 60);

    const QColor ClipVideo(65, 105, 155);
    const QColor ClipAudio(60, 140, 80);
    const QColor ClipCaption(180, 130, 60);
    const QColor ClipImage(130, 80, 160);
    const QColor ClipTransition(120, 120, 120);
    const QColor ClipSelected(72, 140, 230);

    const QColor Success(60, 180, 80);
    const QColor Warning(220, 170, 40);
    const QColor Error(220, 60, 60);
}

void apply(QApplication *app);
QString styleSheet();

} // namespace DaVinciTheme
