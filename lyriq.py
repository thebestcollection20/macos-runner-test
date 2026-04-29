#!/usr/bin/env python3
"""
LYRIQ - Multilingual Lyric Studio
A PySide6 desktop application for managing, editing, syncing, styling and
exporting multilingual lyrics. Built with a vibrant "Neon-Glass" dark UI.
"""

import sys
import random
import math
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QGridLayout, QLabel, QPushButton, QLineEdit, QTextEdit, QScrollArea,
    QStackedWidget, QFrame, QSlider, QCheckBox, QSizePolicy, QFileDialog,
    QProgressBar, QSpacerItem, QGraphicsDropShadowEffect, QMessageBox,
)
from PySide6.QtCore import (
    Qt, QSize, QTimer, QPropertyAnimation, QEasingCurve, Signal, QRect,
    QPoint, Property, QRectF,
)
from PySide6.QtGui import (
    QFont, QColor, QPainter, QLinearGradient, QPen, QBrush, QIcon,
    QPainterPath, QFontDatabase, QPixmap, QRadialGradient, QPalette,
    QConicalGradient,
)

# ---------------------------------------------------------------------------
# Design-system constants
# ---------------------------------------------------------------------------
COLORS = {
    "background":                "#0b1326",
    "surface":                   "#0b1326",
    "surface_dim":               "#0b1326",
    "surface_bright":            "#31394d",
    "surface_container_lowest":  "#060e20",
    "surface_container_low":     "#131b2e",
    "surface_container":         "#171f33",
    "surface_container_high":    "#222a3d",
    "surface_container_highest": "#2d3449",
    "on_surface":                "#dae2fd",
    "on_surface_variant":        "#c7c4d7",
    "outline":                   "#908fa0",
    "outline_variant":           "#464554",
    "primary":                   "#79d9a9",
    "on_primary":                "#003823",
    "primary_container":         "#3fa276",
    "secondary":                 "#cebdff",
    "on_secondary":              "#380094",
    "secondary_container":       "#5401d4",
    "tertiary":                  "#80d0ff",
    "on_tertiary":               "#00344b",
    "tertiary_container":        "#3f99c8",
    "error":                     "#ffb4ab",
    "error_container":           "#93000a",
    "indigo400":                 "#818cf8",
    "indigo500":                 "#6366f1",
    "indigo600":                 "#4f46e5",
    "purple400":                 "#c084fc",
    "purple500":                 "#a855f7",
    "purple600":                 "#9333ea",
    "slate400":                  "#94a3b8",
    "slate500":                  "#64748b",
    "slate600":                  "#475569",
    "slate700":                  "#334155",
    "slate900":                  "#0f172a",
    "white05":                   "rgba(255,255,255,0.05)",
    "white10":                   "rgba(255,255,255,0.10)",
    "green400":                  "#4ade80",
    "green500":                  "#22c55e",
}

C = COLORS  # shorthand

# ---------------------------------------------------------------------------
# Shared stylesheet helpers
# ---------------------------------------------------------------------------

GLOBAL_QSS = f"""
* {{
    font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
}}
QWidget {{
    color: {C['on_surface']};
}}
QScrollArea {{
    border: none;
    background: transparent;
}}
QScrollBar:vertical {{
    width: 6px;
    background: rgba(255,255,255,0.03);
    border-radius: 3px;
}}
QScrollBar::handle:vertical {{
    background: rgba(99,102,241,0.35);
    min-height: 30px;
    border-radius: 3px;
}}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {{
    height: 0px;
}}
QScrollBar:horizontal {{
    height: 6px;
    background: rgba(255,255,255,0.03);
    border-radius: 3px;
}}
QScrollBar::handle:horizontal {{
    background: rgba(99,102,241,0.35);
    min-height: 30px;
    border-radius: 3px;
}}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {{
    width: 0px;
}}
QLineEdit {{
    background: {C['surface_container']};
    border: none;
    border-bottom: 2px solid {C['outline_variant']};
    border-radius: 6px 6px 0 0;
    padding: 10px 14px;
    color: {C['on_surface']};
    font-size: 14px;
}}
QLineEdit:focus {{
    border-bottom: 2px solid {C['indigo500']};
}}
QTextEdit {{
    background: transparent;
    border: none;
    color: {C['on_surface']};
    font-size: 16px;
    selection-background-color: rgba(99,102,241,0.3);
}}
QCheckBox {{
    spacing: 8px;
    color: {C['on_surface']};
}}
QCheckBox::indicator {{
    width: 20px; height: 20px;
    border-radius: 4px;
    border: 2px solid {C['outline_variant']};
    background: {C['surface_container_highest']};
}}
QCheckBox::indicator:checked {{
    background: {C['indigo500']};
    border-color: {C['indigo500']};
}}
QSlider::groove:horizontal {{
    height: 4px;
    background: {C['slate700']};
    border-radius: 2px;
}}
QSlider::handle:horizontal {{
    background: {C['indigo500']};
    width: 14px; height: 14px;
    margin: -5px 0;
    border-radius: 7px;
}}
QSlider::sub-page:horizontal {{
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 {C['indigo500']}, stop:1 {C['purple500']});
    border-radius: 2px;
}}
"""


def glass_frame_style(border_color="rgba(255,255,255,0.08)", bg="rgba(23,31,51,0.6)", radius=16):
    return (
        f"background: {bg}; "
        f"border: 1px solid {border_color}; "
        f"border-radius: {radius}px;"
    )


def gradient_button_style(from_c=None, to_c=None, radius=24, pad="12px 28px"):
    from_c = from_c or C['indigo600']
    to_c = to_c or C['purple600']
    return (
        f"QPushButton {{ "
        f"  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 {from_c}, stop:1 {to_c}); "
        f"  color: white; border: none; border-radius: {radius}px; padding: {pad}; "
        f"  font-weight: 700; font-size: 12px; letter-spacing: 1.5px; "
        f"}} "
        f"QPushButton:hover {{ "
        f"  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 {to_c}, stop:1 {from_c}); "
        f"}} "
        f"QPushButton:pressed {{ padding-top: 14px; }}"
    )


def pill_button_style(bg=None, color=None, border=None):
    bg = bg or "rgba(255,255,255,0.05)"
    color = color or C['on_surface_variant']
    border = border or "rgba(255,255,255,0.05)"
    return (
        f"QPushButton {{ background: {bg}; color: {color}; border: 1px solid {border}; "
        f"border-radius: 14px; padding: 6px 16px; font-size: 10px; "
        f"font-weight: 700; letter-spacing: 1.5px; text-transform: uppercase; }}"
        f"QPushButton:hover {{ background: rgba(255,255,255,0.10); }}"
    )


# ---------------------------------------------------------------------------
# Custom painted widgets
# ---------------------------------------------------------------------------

class GradientLabel(QLabel):
    """Label with gradient text (LYRIQ branding)."""
    def __init__(self, text="LYRIQ", parent=None):
        super().__init__(text, parent)
        self._text = text
        font = QFont("Spline Sans", 20, QFont.Black)
        font.setItalic(True)
        font.setLetterSpacing(QFont.PercentageSpacing, 95)
        self.setFont(font)
        self.setMinimumHeight(32)

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        grad = QLinearGradient(0, 0, self.width(), 0)
        grad.setColorAt(0, QColor(C['indigo500']))
        grad.setColorAt(1, QColor(C['purple400']))
        pen = QPen(QBrush(grad), 1)
        p.setPen(pen)
        p.setFont(self.font())
        flags = int(self.alignment()) | Qt.AlignVCenter
        if self.wordWrap():
            flags |= Qt.TextWordWrap
        p.drawText(self.rect(), flags, self._text)
        p.end()


class WaveformWidget(QWidget):
    """Decorative waveform visualisation."""
    def __init__(self, bar_count=60, played_ratio=0.35, parent=None):
        super().__init__(parent)
        self.bar_count = bar_count
        self.played_ratio = played_ratio
        self.heights = [random.uniform(0.15, 1.0) for _ in range(bar_count)]
        self.setMinimumHeight(80)
        self.setMaximumHeight(120)

    def set_played_ratio(self, ratio):
        self.played_ratio = ratio
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w = self.width()
        h = self.height()
        gap = 2
        bar_w = max(2, (w - (self.bar_count - 1) * gap) / self.bar_count)
        played_x = w * self.played_ratio

        for i, bh in enumerate(self.heights):
            x = i * (bar_w + gap)
            bar_h = bh * (h - 10)
            y = (h - bar_h) / 2
            if x < played_x:
                color = QColor(C['indigo500'])
                if bh > 0.8:
                    color = QColor("#818cf8")
            else:
                color = QColor(C['slate700'])
            p.setPen(Qt.NoPen)
            p.setBrush(color)
            p.drawRoundedRect(QRectF(x, y, bar_w, bar_h), bar_w / 2, bar_w / 2)

        # playhead
        px = played_x
        p.setPen(QPen(QColor("white"), 2))
        p.drawLine(int(px), 0, int(px), h)
        p.setBrush(QColor("white"))
        p.drawEllipse(QPoint(int(px), 6), 5, 5)
        p.end()


class GlassPanel(QFrame):
    """A glassmorphism-styled panel."""
    def __init__(self, parent=None, radius=16, border_color="rgba(255,255,255,0.08)",
                 bg="rgba(23,31,51,0.6)"):
        super().__init__(parent)
        self.setStyleSheet(glass_frame_style(border_color, bg, radius))

    def add_glow(self, color=None, blur=20):
        color = color or QColor(99, 102, 241, 50)
        shadow = QGraphicsDropShadowEffect(self)
        shadow.setBlurRadius(blur)
        shadow.setColor(color)
        shadow.setOffset(0, 0)
        self.setGraphicsEffect(shadow)


class CircleAvatar(QWidget):
    """A circular coloured avatar placeholder."""
    def __init__(self, initials="LP", size=40, bg_color=None, parent=None):
        super().__init__(parent)
        self.initials = initials
        self.bg_color = QColor(bg_color) if bg_color else QColor(C['indigo500'])
        self.setFixedSize(size, size)

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.setBrush(self.bg_color)
        p.setPen(QPen(QColor(C['indigo500']), 2))
        p.drawEllipse(2, 2, self.width() - 4, self.height() - 4)
        p.setPen(QColor("white"))
        p.setFont(QFont("Segoe UI", 11, QFont.Bold))
        p.drawText(self.rect(), Qt.AlignCenter, self.initials)
        p.end()


class ColorSwatch(QPushButton):
    """Clickable colour circle for styling preview."""
    def __init__(self, colors_grad=None, solid=None, size=36, parent=None):
        super().__init__(parent)
        self.colors_grad = colors_grad
        self.solid = solid
        self.setFixedSize(size, size)
        self.setCursor(Qt.PointingHandCursor)
        self.setStyleSheet("border: none; background: transparent;")
        self._selected = False

    def set_selected(self, s):
        self._selected = s
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        r = min(self.width(), self.height()) / 2 - 2
        cx, cy = self.width() / 2, self.height() / 2
        if self.solid:
            p.setBrush(QColor(self.solid))
        elif self.colors_grad:
            grad = QLinearGradient(0, 0, self.width(), self.height())
            grad.setColorAt(0, QColor(self.colors_grad[0]))
            grad.setColorAt(1, QColor(self.colors_grad[1]))
            p.setBrush(QBrush(grad))
        if self._selected:
            p.setPen(QPen(QColor("white"), 2))
        else:
            p.setPen(QPen(QColor("rgba(255,255,255,0.2)"), 1))
        p.drawEllipse(QRectF(cx - r, cy - r, r * 2, r * 2))
        p.end()


class ProgressBarCustom(QWidget):
    """A slim gradient progress bar."""
    def __init__(self, value=0, max_val=100, parent=None):
        super().__init__(parent)
        self._value = value
        self._max = max_val
        self.setFixedHeight(6)

    def set_value(self, v):
        self._value = v
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = self.width(), self.height()
        # track
        p.setBrush(QColor("rgba(255,255,255,0.05)"))
        p.setPen(Qt.NoPen)
        p.drawRoundedRect(0, 0, w, h, h / 2, h / 2)
        # fill
        ratio = self._value / self._max if self._max else 0
        fill_w = int(w * ratio)
        if fill_w > 0:
            grad = QLinearGradient(0, 0, fill_w, 0)
            grad.setColorAt(0, QColor(C['indigo500']))
            grad.setColorAt(1, QColor(C['purple400']))
            p.setBrush(QBrush(grad))
            p.drawRoundedRect(0, 0, fill_w, h, h / 2, h / 2)
        p.end()


class StatusDot(QWidget):
    """A small pulsing dot."""
    def __init__(self, color=None, size=8, parent=None):
        super().__init__(parent)
        self._color = QColor(color) if color else QColor(C['primary'])
        self.setFixedSize(size, size)

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        p.setBrush(self._color)
        p.setPen(Qt.NoPen)
        p.drawEllipse(1, 1, self.width() - 2, self.height() - 2)
        p.end()


# ---------------------------------------------------------------------------
# Helper to create styled label
# ---------------------------------------------------------------------------

def make_label(text, size=14, color=None, bold=False, font_family=None, caps=False):
    lbl = QLabel(text)
    c = color or C['on_surface']
    weight = "bold" if bold else "normal"
    ff = font_family or "Segoe UI"
    extra = "text-transform: uppercase; letter-spacing: 1.5px;" if caps else ""
    lbl.setStyleSheet(f"color: {c}; font-size: {size}px; font-weight: {weight}; "
                      f"font-family: '{ff}'; {extra} background: transparent;")
    lbl.setWordWrap(True)
    return lbl


def make_icon_label(icon_char, color=None, size=22):
    """Use unicode or text as icon placeholder."""
    lbl = QLabel(icon_char)
    c = color or C['on_surface']
    lbl.setStyleSheet(f"color: {c}; font-size: {size}px; background: transparent;")
    lbl.setFixedSize(size + 4, size + 4)
    lbl.setAlignment(Qt.AlignCenter)
    return lbl


def h_spacer():
    return QSpacerItem(0, 0, QSizePolicy.Expanding, QSizePolicy.Minimum)


def v_spacer():
    return QSpacerItem(0, 0, QSizePolicy.Minimum, QSizePolicy.Expanding)


# ---------------------------------------------------------------------------
# Top App Bar
# ---------------------------------------------------------------------------

class TopAppBar(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedHeight(64)
        self.setStyleSheet(
            f"background: rgba(15,23,42,0.7); border-bottom: 1px solid rgba(255,255,255,0.1);"
        )
        layout = QHBoxLayout(self)
        layout.setContentsMargins(20, 0, 20, 0)

        avatar = CircleAvatar("LP", 36, C['surface_container_highest'])
        layout.addWidget(avatar)

        logo = GradientLabel("LYRIQ")
        layout.addWidget(logo)
        layout.addStretch()

        for nav_text in ["Library", "Studio", "Sync"]:
            btn = QPushButton(nav_text.upper())
            btn.setStyleSheet(
                f"QPushButton {{ color: {C['slate400']}; background: transparent; "
                f"border: none; font-size: 11px; font-weight: 700; letter-spacing: 2px; "
                f"padding: 6px 12px; }} "
                f"QPushButton:hover {{ color: {C['indigo400']}; }}"
            )
            btn.setCursor(Qt.PointingHandCursor)
            layout.addWidget(btn)

        settings_btn = QPushButton("\u2699")
        settings_btn.setStyleSheet(
            f"QPushButton {{ color: {C['slate400']}; background: transparent; "
            f"border: none; font-size: 18px; padding: 6px; border-radius: 18px; }} "
            f"QPushButton:hover {{ background: rgba(255,255,255,0.05); }}"
        )
        settings_btn.setFixedSize(36, 36)
        settings_btn.setCursor(Qt.PointingHandCursor)
        layout.addWidget(settings_btn)


# ---------------------------------------------------------------------------
# Bottom Navigation Bar
# ---------------------------------------------------------------------------

class BottomNavBar(QWidget):
    tab_changed = Signal(int)

    NAV_ITEMS = [
        ("\U0001F3B5", "Library"),
        ("\u270F", "Studio"),
        ("\u21C4", "Sync"),
        ("\U0001F3A8", "Style"),
        ("\u2B07", "Export"),
    ]

    def __init__(self, parent=None):
        super().__init__(parent)
        self.current_index = 0
        self.setFixedHeight(72)
        self.setStyleSheet(
            "background: rgba(15,23,42,0.85); "
            "border-top: 1px solid rgba(99,102,241,0.2);"
        )
        layout = QHBoxLayout(self)
        layout.setContentsMargins(8, 0, 8, 4)
        layout.setSpacing(0)
        self.buttons = []
        for i, (icon, label) in enumerate(self.NAV_ITEMS):
            btn = QPushButton()
            btn.setFixedHeight(64)
            btn.setCursor(Qt.PointingHandCursor)
            btn_layout = QVBoxLayout(btn)
            btn_layout.setContentsMargins(0, 6, 0, 2)
            btn_layout.setSpacing(2)

            icon_lbl = QLabel(icon)
            icon_lbl.setAlignment(Qt.AlignCenter)
            icon_lbl.setStyleSheet("font-size: 20px; background: transparent;")
            btn_layout.addWidget(icon_lbl)

            text_lbl = QLabel(label.upper())
            text_lbl.setAlignment(Qt.AlignCenter)
            text_lbl.setStyleSheet(
                "font-size: 9px; font-weight: 700; letter-spacing: 2px; background: transparent;"
            )
            btn_layout.addWidget(text_lbl)

            btn.setStyleSheet("QPushButton { border: none; background: transparent; }")
            btn.clicked.connect(lambda checked, idx=i: self._on_click(idx))
            layout.addWidget(btn)
            self.buttons.append((btn, icon_lbl, text_lbl))

        self._update_styles()

    def _on_click(self, idx):
        self.current_index = idx
        self._update_styles()
        self.tab_changed.emit(idx)

    def _update_styles(self):
        for i, (btn, icon_lbl, text_lbl) in enumerate(self.buttons):
            if i == self.current_index:
                icon_lbl.setStyleSheet(
                    f"font-size: 22px; color: {C['indigo400']}; background: transparent;"
                )
                text_lbl.setStyleSheet(
                    f"font-size: 9px; font-weight: 700; letter-spacing: 2px; "
                    f"color: {C['indigo400']}; background: transparent;"
                )
            else:
                icon_lbl.setStyleSheet(
                    f"font-size: 20px; color: {C['slate500']}; background: transparent;"
                )
                text_lbl.setStyleSheet(
                    f"font-size: 9px; font-weight: 700; letter-spacing: 2px; "
                    f"color: {C['slate500']}; background: transparent;"
                )


# ---------------------------------------------------------------------------
# PAGE 1: Lyrics Library
# ---------------------------------------------------------------------------

class ProjectCard(GlassPanel):
    """A single project card for the library grid."""
    def __init__(self, title, language, status, progress, status_color=None, parent=None):
        super().__init__(parent, radius=20)
        self.setCursor(Qt.PointingHandCursor)
        layout = QVBoxLayout(self)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        # top row
        top = QHBoxLayout()
        icon_w = QWidget()
        icon_w.setFixedSize(36, 36)
        icon_w.setStyleSheet(
            f"background: {C['surface_container_high']}; border-radius: 8px;"
        )
        icon_lbl = QLabel("\U0001F3B5")
        icon_lbl.setAlignment(Qt.AlignCenter)
        icon_lbl.setStyleSheet("font-size: 18px; background: transparent;")
        il = QVBoxLayout(icon_w)
        il.setContentsMargins(0, 0, 0, 0)
        il.addWidget(icon_lbl)
        top.addWidget(icon_w)
        top.addStretch()

        status_color = status_color or C['primary']
        badge = QLabel(status.upper())
        badge.setStyleSheet(
            f"background: {status_color}20; color: {status_color}; "
            f"font-size: 9px; font-weight: 700; letter-spacing: 1.5px; "
            f"padding: 3px 8px; border-radius: 4px;"
        )
        top.addWidget(badge)
        layout.addLayout(top)

        # title
        title_lbl = make_label(title, 20, C['on_surface'], bold=True)
        layout.addWidget(title_lbl)

        # language
        lang_lbl = make_label(f"\U0001F310 {language}", 12, C['on_surface_variant'])
        layout.addWidget(lang_lbl)

        layout.addStretch()

        # progress
        prog_row = QHBoxLayout()
        prog_row.addWidget(make_label("Completion", 9, C['on_surface_variant'], bold=True, caps=True))
        prog_row.addStretch()
        prog_row.addWidget(make_label(f"{progress}%", 9, C['primary'], bold=True, caps=True))
        layout.addLayout(prog_row)

        bar = ProgressBarCustom(progress)
        layout.addWidget(bar)


class LyricsLibraryPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")

        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.addWidget(scroll)

        container = QWidget()
        container.setStyleSheet("background: transparent;")
        scroll.setWidget(container)
        layout = QVBoxLayout(container)
        layout.setContentsMargins(32, 24, 32, 32)
        layout.setSpacing(24)

        # header section
        header_row = QHBoxLayout()
        header_left = QVBoxLayout()
        header_left.addWidget(make_label("PROJECT MANAGER", 11, C['primary'], bold=True, caps=True))
        header_left.addWidget(make_label("Your Library", 36, C['on_surface'], bold=True))
        header_left.addWidget(make_label(
            "Manage your lyrical compositions, vocal syncs, and AI-assisted drafts in one place.",
            16, C['on_surface_variant']
        ))
        header_row.addLayout(header_left, 1)

        header_right = QHBoxLayout()
        header_right.setSpacing(10)
        self.search_input = QLineEdit()
        self.search_input.setPlaceholderText("\U0001F50D  Search song titles...")
        self.search_input.setFixedWidth(260)
        header_right.addWidget(self.search_input)

        new_btn = QPushButton("+ NEW PROJECT")
        new_btn.setStyleSheet(gradient_button_style(
            C['primary_container'], C['primary'], 12, "10px 20px"
        ))
        new_btn.setCursor(Qt.PointingHandCursor)
        new_btn.clicked.connect(self._new_project)
        header_right.addWidget(new_btn)
        header_row.addLayout(header_right)
        layout.addLayout(header_row)

        # filters
        filters_row = QHBoxLayout()
        filters_row.setSpacing(8)
        filter_labels = ["All Tracks", "English", "Spanish", "In Progress", "Completed"]
        for i, f in enumerate(filter_labels):
            btn = QPushButton(f.upper())
            if i == 0:
                btn.setStyleSheet(pill_button_style(C['primary'], C['on_primary']))
            else:
                btn.setStyleSheet(pill_button_style())
            btn.setCursor(Qt.PointingHandCursor)
            filters_row.addWidget(btn)
        filters_row.addStretch()
        layout.addLayout(filters_row)

        # project grid
        grid = QGridLayout()
        grid.setSpacing(16)
        projects = [
            ("Midnight Resonance", "English (US)", "Editing Now", 85, C['primary']),
            ("Neon Horizons", "Japanese / English", "Draft", 30, C['secondary']),
            ("Subway Echoes", "Spanish", "Finalizing", 92, C['tertiary']),
            ("Digital Ghost", "English (UK)", "Vocals Logged", 100, C['indigo400']),
            ("Stardust Blues", "French", "Archived", 15, C['error']),
        ]

        # first card spans 2 cols
        card0 = ProjectCard(*projects[0])
        card0.setMinimumHeight(220)
        grid.addWidget(card0, 0, 0, 1, 2)

        for i, proj in enumerate(projects[1:]):
            card = ProjectCard(*proj)
            card.setMinimumHeight(200)
            row = i // 3 + 1
            col = i % 3
            grid.addWidget(card, row, col)

        # new project slot (after last project card)
        slot_idx = len(projects) - 1
        new_slot = QFrame()
        new_slot.setStyleSheet(
            f"border: 2px dashed rgba(255,255,255,0.1); border-radius: 20px; "
            f"background: transparent;"
        )
        new_slot.setCursor(Qt.PointingHandCursor)
        ns_layout = QVBoxLayout(new_slot)
        ns_layout.setAlignment(Qt.AlignCenter)
        ns_layout.addWidget(make_label("\u2795", 28, C['on_surface_variant']))
        ns_layout.addWidget(make_label("Start New Lyrics", 18, C['on_surface_variant'], bold=True))
        ns_layout.addWidget(make_label("Import beat or sync AI", 12, C['outline']))
        new_slot.setMinimumHeight(200)
        grid.addWidget(new_slot, slot_idx // 3 + 1, slot_idx % 3)

        layout.addLayout(grid)
        layout.addStretch()

    def _new_project(self):
        QMessageBox.information(self, "New Project", "Create new project dialog would open here.")


# ---------------------------------------------------------------------------
# PAGE 2: Audio Import & Processing (Studio)
# ---------------------------------------------------------------------------

class AudioImportPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")

        main_layout = QHBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        # sidebar
        sidebar = self._build_sidebar()
        main_layout.addWidget(sidebar)

        # main content
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        content = QWidget()
        content.setStyleSheet("background: transparent;")
        scroll.setWidget(content)
        clayout = QVBoxLayout(content)
        clayout.setContentsMargins(40, 30, 40, 40)
        clayout.setSpacing(28)

        clayout.addWidget(make_label("Audio Import", 36, C['on_surface'], bold=True))
        clayout.addWidget(make_label(
            "Drop your stems or full tracks here. Our AI engine will handle the "
            "transcription and timing mapping instantly.",
            16, C['on_surface_variant']
        ))

        # bento: upload + queue
        bento = QHBoxLayout()
        bento.setSpacing(20)

        # upload area
        upload_panel = GlassPanel(radius=24)
        upload_panel.setMinimumHeight(350)
        ul = QVBoxLayout(upload_panel)
        ul.setAlignment(Qt.AlignCenter)
        ul.setSpacing(16)

        cloud_icon = QLabel("\u2601")
        cloud_icon.setStyleSheet(
            f"font-size: 48px; color: {C['primary']}; background: {C['surface_container_highest']}; "
            f"border-radius: 40px; padding: 16px;"
        )
        cloud_icon.setAlignment(Qt.AlignCenter)
        cloud_icon.setFixedSize(80, 80)
        ul.addWidget(cloud_icon, 0, Qt.AlignCenter)

        ul.addWidget(make_label("Drag & Drop Audio Files", 20, C['on_surface'], bold=True))
        ul.addWidget(make_label("Supports WAV, MP3, and FLAC (Up to 100MB)", 14, C['on_surface_variant']))

        browse_btn = QPushButton("BROWSE FILES")
        browse_btn.setStyleSheet(gradient_button_style(radius=24, pad="14px 36px"))
        browse_btn.setCursor(Qt.PointingHandCursor)
        browse_btn.clicked.connect(self._browse_files)
        ul.addWidget(browse_btn, 0, Qt.AlignCenter)

        bento.addWidget(upload_panel, 2)

        # processing queue
        queue_panel = GlassPanel(radius=24)
        ql = QVBoxLayout(queue_panel)
        ql.setContentsMargins(20, 20, 20, 20)
        ql.setSpacing(12)

        q_header = QHBoxLayout()
        q_header.addWidget(make_label("AI PROCESSING", 11, C['on_surface_variant'], bold=True, caps=True))
        q_header.addStretch()
        q_header.addWidget(StatusDot(C['tertiary']))
        ql.addLayout(q_header)

        # active item
        item1 = GlassPanel(radius=16, bg="rgba(255,255,255,0.05)")
        i1l = QVBoxLayout(item1)
        i1l.setContentsMargins(14, 14, 14, 14)
        i1l.setSpacing(8)
        i1_top = QHBoxLayout()
        i1_left = QVBoxLayout()
        i1_left.addWidget(make_label("midnight_sessions_v2.wav", 14, bold=True))
        i1_left.addWidget(make_label("Transcription in progress...", 11, C['on_surface_variant']))
        i1_top.addLayout(i1_left)
        i1_top.addStretch()
        i1_top.addWidget(make_label("65%", 13, C['primary'], bold=True))
        i1l.addLayout(i1_top)
        bar1 = ProgressBarCustom(65)
        i1l.addWidget(bar1)
        ql.addWidget(item1)

        # queued item
        item2 = GlassPanel(radius=16, bg="rgba(255,255,255,0.05)")
        item2.setStyleSheet(item2.styleSheet() + " opacity: 0.6;")
        i2l = QVBoxLayout(item2)
        i2l.setContentsMargins(14, 14, 14, 14)
        i2_top = QHBoxLayout()
        i2_left = QVBoxLayout()
        i2_left.addWidget(make_label("urban_beats_main.mp3", 14, bold=True))
        i2_left.addWidget(make_label("Waiting in queue", 11, C['on_surface_variant']))
        i2_top.addLayout(i2_left)
        i2_top.addStretch()
        i2_top.addWidget(make_label("\u23F0", 16, C['slate500']))
        i2l.addLayout(i2_top)
        ql.addWidget(item2)

        ql.addStretch()
        view_queue_btn = QPushButton("VIEW ALL QUEUE")
        view_queue_btn.setStyleSheet(
            f"QPushButton {{ border: 1px solid rgba(255,255,255,0.1); "
            f"background: transparent; color: {C['on_surface_variant']}; "
            f"border-radius: 10px; padding: 10px; font-size: 11px; "
            f"font-weight: 700; letter-spacing: 1.5px; }}"
            f"QPushButton:hover {{ background: rgba(255,255,255,0.05); }}"
        )
        view_queue_btn.setCursor(Qt.PointingHandCursor)
        ql.addWidget(view_queue_btn)

        bento.addWidget(queue_panel, 1)
        clayout.addLayout(bento)

        # recently imported
        ri_header = QHBoxLayout()
        ri_header.addWidget(make_label("Recently Imported", 20, C['on_surface'], bold=True))
        ri_header.addStretch()
        view_lib_btn = QPushButton("VIEW LIBRARY")
        view_lib_btn.setStyleSheet(
            f"QPushButton {{ color: {C['primary']}; background: transparent; border: none; "
            f"font-size: 11px; font-weight: 700; letter-spacing: 1.5px; }}"
            f"QPushButton:hover {{ text-decoration: underline; }}"
        )
        view_lib_btn.setCursor(Qt.PointingHandCursor)
        ri_header.addWidget(view_lib_btn)
        clayout.addLayout(ri_header)

        files_grid = QHBoxLayout()
        files_grid.setSpacing(16)
        files_data = [
            ("Summer_Vibes_Stem.wav", "2.4 MB \u2022 2 days ago", "SYNCED", C['green400']),
            ("Neon_Nights_Rough.mp3", "5.1 MB \u2022 5 days ago", "AI VERIFIED", C['indigo400']),
            ("Freestyle_Test_04.wav", "1.8 MB \u2022 1 week ago", "DRAFT", C['slate400']),
        ]
        for fname, meta, status, scolor in files_data:
            card = GlassPanel(radius=16)
            card.setCursor(Qt.PointingHandCursor)
            cl = QVBoxLayout(card)
            cl.setContentsMargins(18, 18, 18, 18)
            cl.setSpacing(10)
            top_row = QHBoxLayout()
            icon_w = QWidget()
            icon_w.setFixedSize(40, 40)
            icon_w.setStyleSheet(
                f"background: {scolor}20; border-radius: 10px;"
            )
            iw_l = QVBoxLayout(icon_w)
            iw_l.setContentsMargins(0, 0, 0, 0)
            iw_l.addWidget(make_label("\U0001F3B6", 18, scolor))
            top_row.addWidget(icon_w)
            name_col = QVBoxLayout()
            name_col.addWidget(make_label(fname, 13, bold=True))
            name_col.addWidget(make_label(meta, 11, C['on_surface_variant']))
            top_row.addLayout(name_col)
            top_row.addStretch()
            cl.addLayout(top_row)

            sep = QFrame()
            sep.setFixedHeight(1)
            sep.setStyleSheet("background: rgba(255,255,255,0.05);")
            cl.addWidget(sep)

            badge = QLabel(status)
            badge.setStyleSheet(
                f"background: {scolor}30; color: {scolor}; font-size: 9px; "
                f"font-weight: 700; padding: 3px 8px; border-radius: 4px;"
            )
            cl.addWidget(badge)
            files_grid.addWidget(card)

        clayout.addLayout(files_grid)
        clayout.addStretch()

        main_layout.addWidget(scroll, 1)

    def _build_sidebar(self):
        sidebar = QWidget()
        sidebar.setFixedWidth(220)
        sidebar.setStyleSheet(
            f"background: rgba(15,23,42,0.9); border-right: 1px solid rgba(255,255,255,0.05);"
        )
        sl = QVBoxLayout(sidebar)
        sl.setContentsMargins(16, 28, 16, 16)
        sl.setSpacing(4)

        sl.addWidget(make_label("Lyricist Pro", 16, C['indigo400'], bold=True))
        sl.addWidget(make_label("PREMIUM ACCOUNT", 9, C['slate500'], bold=True, caps=True))
        sl.addSpacing(28)

        nav_items = [
            ("\U0001F4C2", "Recent Projects", False),
            ("\u2728", "AI Studio", True),
            ("\u23F1", "Timing Sync", False),
            ("\U0001F58C", "Visualizer", False),
            ("\u2B07\uFE0F", "Export Hub", False),
        ]
        for icon, label, active in nav_items:
            btn = QPushButton(f"  {icon}  {label}")
            btn.setFixedHeight(42)
            btn.setCursor(Qt.PointingHandCursor)
            if active:
                btn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: rgba(99,102,241,0.1); "
                    f"color: {C['indigo400']}; border: none; border-left: 3px solid {C['indigo500']}; "
                    f"border-radius: 8px; padding-left: 12px; font-size: 13px; font-weight: 600; }}"
                )
            else:
                btn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: transparent; "
                    f"color: {C['slate400']}; border: none; border-radius: 8px; "
                    f"padding-left: 16px; font-size: 13px; }} "
                    f"QPushButton:hover {{ background: rgba(255,255,255,0.05); }}"
                )
            sl.addWidget(btn)

        sl.addStretch()
        return sidebar

    def _browse_files(self):
        path, _ = QFileDialog.getOpenFileName(
            self, "Import Audio", "",
            "Audio Files (*.wav *.mp3 *.flac *.ogg);;All Files (*)"
        )
        if path:
            QMessageBox.information(self, "File Selected", f"Imported: {path}")


# ---------------------------------------------------------------------------
# PAGE 3: Lyrics Editor
# ---------------------------------------------------------------------------

class LyricCard(GlassPanel):
    """An editable lyric card."""
    def __init__(self, section_name, lines, active=False, parent=None):
        border = f"rgba(99,102,241,0.4)" if active else "rgba(255,255,255,0.05)"
        bg = "rgba(99,102,241,0.05)" if active else "rgba(23,31,51,0.4)"
        super().__init__(parent, radius=20, border_color=border, bg=bg)
        if active:
            self.add_glow(QColor(99, 102, 241, 30), 30)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(24, 20, 24, 20)
        layout.setSpacing(8)

        sec_color = C['primary'] if active else C['slate500']
        suffix = " (ACTIVE)" if active else ""
        layout.addWidget(make_label(f"{section_name}{suffix}", 10, sec_color, bold=True, caps=True))

        for i, line in enumerate(lines):
            row = QHBoxLayout()
            num = make_label(f"{i + 1:02d}", 11, C['slate600'] if not active else f"{C['primary']}90",
                             bold=True, caps=True)
            num.setFixedWidth(28)
            row.addWidget(num)

            editor = QTextEdit()
            editor.setPlainText(line)
            editor.setMaximumHeight(38)
            editor.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
            font = QFont("Segoe UI", 16, QFont.Medium)
            editor.setFont(font)
            if active:
                editor.setStyleSheet(f"color: {C['on_surface']}; background: transparent; border: none;")
            else:
                editor.setStyleSheet(f"color: {C['on_surface_variant']}; background: transparent; border: none;")
            row.addWidget(editor)
            layout.addLayout(row)


class LyricsEditorPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        # waveform section
        wave_section = QWidget()
        wave_section.setStyleSheet(f"background: {C['surface_container_low']};")
        ws_layout = QVBoxLayout(wave_section)
        ws_layout.setContentsMargins(24, 12, 24, 12)

        wave_header = QHBoxLayout()
        wave_header.addWidget(make_label("\U0001F3A7 Studio Session: Punjab_Vibe_V3.wav",
                                         16, C['on_surface'], bold=True))
        wave_header.addStretch()
        live_badge = QLabel("LIVE SYNC")
        live_badge.setStyleSheet(
            f"background: {C['secondary_container']}; color: {C['secondary']}; "
            f"font-size: 9px; font-weight: 700; letter-spacing: 1.5px; "
            f"padding: 4px 10px; border-radius: 10px;"
        )
        wave_header.addWidget(live_badge)
        time_badge = QLabel("03:42 / 04:15")
        time_badge.setStyleSheet(
            f"background: rgba(255,255,255,0.05); color: {C['slate400']}; "
            f"font-size: 9px; font-weight: 700; padding: 4px 10px; border-radius: 10px;"
        )
        wave_header.addWidget(time_badge)
        ws_layout.addLayout(wave_header)

        waveform = WaveformWidget(80, 0.25)
        ws_layout.addWidget(waveform)
        main_layout.addWidget(wave_section)

        # editor area
        editor_area = QHBoxLayout()
        editor_area.setSpacing(0)

        # left sidebar
        sidebar = QWidget()
        sidebar.setFixedWidth(240)
        sidebar.setStyleSheet(
            f"background: {C['surface_container']}; border-right: 1px solid rgba(255,255,255,0.05);"
        )
        sb_layout = QVBoxLayout(sidebar)
        sb_layout.setContentsMargins(18, 18, 18, 18)
        sb_layout.setSpacing(12)

        sb_layout.addWidget(make_label("AI ASSISTANT", 10, C['slate500'], bold=True, caps=True))

        fix_btn = QPushButton("\u2728  Fix Grammar")
        fix_btn.setStyleSheet(gradient_button_style(pad="14px 16px", radius=16))
        fix_btn.setCursor(Qt.PointingHandCursor)
        sb_layout.addWidget(fix_btn)

        trans_btn = QPushButton("\U0001F310  Transcribe to EN")
        trans_btn.setStyleSheet(
            f"QPushButton {{ background: {C['surface_container_highest']}; "
            f"border: 1px solid rgba(99,102,241,0.2); color: {C['indigo400']}; "
            f"border-radius: 16px; padding: 14px 16px; font-size: 13px; font-weight: 600; }}"
            f"QPushButton:hover {{ background: rgba(99,102,241,0.1); }}"
        )
        trans_btn.setCursor(Qt.PointingHandCursor)
        sb_layout.addWidget(trans_btn)

        sb_layout.addSpacing(12)
        sb_layout.addWidget(make_label("DETECTED ISSUES (3)", 10, C['slate500'], bold=True, caps=True))

        # issues
        issues = [
            ("\u26A0", "GRAMMAR", C['error'], 'Line 12: Ambiguous tense in "Main tenu yaad".'),
            ("\u2714", "SPELLING", C['tertiary'], 'Line 15: "Chaliya" might be "Challiya".'),
        ]
        for icon, cat, color, desc in issues:
            issue_panel = GlassPanel(radius=10,
                                     border_color=f"{color}30",
                                     bg=f"{color}15")
            il = QVBoxLayout(issue_panel)
            il.setContentsMargins(12, 10, 12, 10)
            il.setSpacing(4)
            cat_row = QHBoxLayout()
            cat_row.addWidget(make_label(icon, 12, color))
            cat_row.addWidget(make_label(cat, 9, color, bold=True, caps=True))
            cat_row.addStretch()
            il.addLayout(cat_row)
            il.addWidget(make_label(desc, 12, C['on_surface_variant']))
            sb_layout.addWidget(issue_panel)

        sb_layout.addStretch()
        editor_area.addWidget(sidebar)

        # right: lyrics editor
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        scroll.setStyleSheet(f"background: {C['surface_dim']};")
        editor_container = QWidget()
        editor_container.setStyleSheet("background: transparent;")
        scroll.setWidget(editor_container)
        ec_layout = QVBoxLayout(editor_container)
        ec_layout.setContentsMargins(32, 24, 32, 80)
        ec_layout.setSpacing(16)

        verse1 = LyricCard("VERSE 1", [
            "Raatan nu jagda ni main tere layi",
            "Akhiyan ch chehra tera har pal vayi",
        ])
        ec_layout.addWidget(verse1)

        chorus = LyricCard("CHORUS", [
            "Main tenu samjhaavan ki",
            "Na tere bin lagda jee",
        ], active=True)
        ec_layout.addWidget(chorus)

        outro = LyricCard("OUTRO", [
            "Mera dil taun pucho haal",
        ])
        ec_layout.addWidget(outro)

        ec_layout.addStretch()
        editor_area.addWidget(scroll, 1)
        main_layout.addLayout(editor_area, 1)

        # floating dock
        dock = self._build_dock()
        main_layout.addWidget(dock)

    def _build_dock(self):
        dock = QWidget()
        dock.setFixedHeight(60)
        dock.setStyleSheet(
            "background: rgba(15,23,42,0.85); border-top: 1px solid rgba(255,255,255,0.1); "
            "border-radius: 24px 24px 0 0;"
        )
        dl = QHBoxLayout(dock)
        dl.setContentsMargins(24, 0, 24, 0)
        dl.addStretch()

        rew_btn = QPushButton("\u23EA")
        rew_btn.setStyleSheet(f"QPushButton {{ color: {C['slate400']}; border: none; font-size: 18px; background: transparent; }}")
        dl.addWidget(rew_btn)

        play_btn = QPushButton("\u25B6")
        play_btn.setFixedSize(48, 48)
        play_btn.setStyleSheet(
            f"QPushButton {{ background: {C['primary']}; color: {C['on_primary']}; "
            f"border-radius: 24px; font-size: 20px; border: none; }}"
            f"QPushButton:hover {{ background: {C['primary_container']}; }}"
        )
        play_btn.setCursor(Qt.PointingHandCursor)
        dl.addWidget(play_btn)

        fwd_btn = QPushButton("\u23E9")
        fwd_btn.setStyleSheet(f"QPushButton {{ color: {C['slate400']}; border: none; font-size: 18px; background: transparent; }}")
        dl.addWidget(fwd_btn)

        sep = QFrame()
        sep.setFixedSize(1, 24)
        sep.setStyleSheet("background: rgba(255,255,255,0.1);")
        dl.addWidget(sep)

        mic = QLabel("\U0001F3A4")
        mic.setStyleSheet(f"color: {C['slate400']}; font-size: 16px; background: transparent;")
        dl.addWidget(mic)

        vol_slider = QSlider(Qt.Horizontal)
        vol_slider.setFixedWidth(80)
        vol_slider.setValue(75)
        dl.addWidget(vol_slider)

        dl.addStretch()
        return dock


# ---------------------------------------------------------------------------
# PAGE 4: Fine-Tuning Sync
# ---------------------------------------------------------------------------

class FineTuningSyncPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")

        main_layout = QHBoxLayout(self)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        # sidebar
        sidebar = self._build_sidebar()
        main_layout.addWidget(sidebar)

        # main
        right = QWidget()
        right_layout = QVBoxLayout(right)
        right_layout.setContentsMargins(0, 0, 0, 0)
        right_layout.setSpacing(0)

        # sync canvas
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet(f"background: {C['surface_container_lowest']};")
        canvas = QWidget()
        canvas.setStyleSheet("background: transparent;")
        scroll.setWidget(canvas)
        cl = QVBoxLayout(canvas)
        cl.setContentsMargins(32, 24, 32, 40)
        cl.setSpacing(20)

        header = QHBoxLayout()
        h_left = QVBoxLayout()
        h_left.addWidget(make_label("Fine-Tuning", 36, C['on_surface'], bold=True))
        h_left.addWidget(make_label('Syncing: "Neon Dreams" \u2014 Final Master', 16, C['on_surface_variant']))
        header.addLayout(h_left)
        header.addStretch()
        live_badge = QLabel("\u26AB LIVE SYNC")
        live_badge.setStyleSheet(
            f"background: {C['surface_container_high']}; border: 1px solid {C['outline_variant']}; "
            f"color: {C['on_surface']}; font-size: 9px; font-weight: 700; "
            f"letter-spacing: 1.5px; padding: 5px 12px; border-radius: 14px;"
        )
        header.addWidget(live_badge)
        cl.addLayout(header)

        # lyric blocks grid
        grid = QGridLayout()
        grid.setSpacing(16)

        # previous line
        prev_card = GlassPanel(radius=12)
        prev_card.setStyleSheet(prev_card.styleSheet() + " opacity: 0.7;")
        pcl = QVBoxLayout(prev_card)
        pcl.setContentsMargins(20, 16, 20, 16)
        pcl.addWidget(make_label("00:12.4", 10, C['outline'], bold=True, caps=True))
        pcl.addWidget(make_label("Walking through the electric rain,", 20, C['on_surface']))
        grid.addWidget(prev_card, 0, 0, 1, 8)

        # current phrase (focused)
        current_card = GlassPanel(radius=12, border_color=f"{C['primary']}60",
                                  bg=f"rgba(121,217,169,0.05)")
        current_card.add_glow(QColor(121, 217, 169, 40), 25)
        ccl = QVBoxLayout(current_card)
        ccl.setContentsMargins(24, 28, 24, 20)
        ccl.setSpacing(12)

        badge_row = QHBoxLayout()
        current_badge = QLabel("CURRENT PHRASE")
        current_badge.setStyleSheet(
            f"background: {C['primary']}; color: {C['on_primary']}; font-size: 9px; "
            f"font-weight: 700; letter-spacing: 1.5px; padding: 4px 14px; border-radius: 12px;"
        )
        badge_row.addWidget(current_badge)
        badge_row.addStretch()
        ccl.addLayout(badge_row)

        time_row = QHBoxLayout()
        time_row.addWidget(make_label("00:15.8", 14, C['primary'], bold=True, caps=True))
        time_row.addStretch()
        edit_btn = QPushButton("\u270F")
        edit_btn.setStyleSheet(
            f"QPushButton {{ color: {C['on_surface']}; background: transparent; "
            f"border: none; font-size: 16px; padding: 4px; border-radius: 14px; }}"
            f"QPushButton:hover {{ background: rgba(255,255,255,0.1); }}"
        )
        time_row.addWidget(edit_btn)
        ccl.addLayout(time_row)

        phrase_lbl = make_label("Washing away all the digital pain.", 28, C['on_surface'], bold=True)
        phrase_lbl.setWordWrap(True)
        ccl.addWidget(phrase_lbl)

        ccl.addWidget(ProgressBarCustom(66))
        grid.addWidget(current_card, 1, 0, 1, 12)

        # upcoming line
        next_card = GlassPanel(radius=12)
        ncl = QVBoxLayout(next_card)
        ncl.setContentsMargins(20, 16, 20, 16)
        ncl.setSpacing(8)
        nc_top = QHBoxLayout()
        nc_top.addWidget(make_label("00:20.2", 10, C['outline'], bold=True, caps=True))
        nc_top.addStretch()
        set_start_btn = QPushButton("SET START")
        set_start_btn.setStyleSheet(
            f"QPushButton {{ color: {C['primary_container']}; background: transparent; "
            f"border: none; font-size: 9px; font-weight: 700; letter-spacing: 1.5px; }}"
            f"QPushButton:hover {{ text-decoration: underline; }}"
        )
        nc_top.addWidget(set_start_btn)
        ncl.addLayout(nc_top)
        ncl.addWidget(make_label("Searching for a ghost in the machine,", 18, C['on_surface']))
        grid.addWidget(next_card, 2, 0, 1, 6)

        # action tiles
        actions_grid = QGridLayout()
        actions_grid.setSpacing(10)
        for i, (icon, label) in enumerate([("\u2728", "AI ALIGN"), ("\u23F0", "RESTORE")]):
            tile = GlassPanel(radius=12, border_color=f"{C['outline']}50")
            tile.setStyleSheet(tile.styleSheet().replace(
                "border: 1px solid", "border: 1px dashed"
            ))
            tl = QVBoxLayout(tile)
            tl.setAlignment(Qt.AlignCenter)
            tl.setSpacing(6)
            color = C['primary'] if i == 0 else C['tertiary']
            tl.addWidget(make_label(icon, 28, color))
            tl.addWidget(make_label(label, 10, C['on_surface'], bold=True, caps=True))
            tile.setMinimumHeight(90)
            actions_grid.addWidget(tile, 0, i)

        action_w = QWidget()
        action_w.setLayout(actions_grid)
        grid.addWidget(action_w, 2, 6, 1, 6)

        cl.addLayout(grid)
        cl.addStretch()

        right_layout.addWidget(scroll, 1)

        # waveform dock
        dock = self._build_waveform_dock()
        right_layout.addWidget(dock)

        main_layout.addWidget(right, 1)

    def _build_sidebar(self):
        sidebar = QWidget()
        sidebar.setFixedWidth(220)
        sidebar.setStyleSheet(
            f"background: rgba(15,23,42,0.9); border-right: 1px solid rgba(255,255,255,0.05);"
        )
        sl = QVBoxLayout(sidebar)
        sl.setContentsMargins(16, 28, 16, 16)
        sl.setSpacing(4)

        sl.addWidget(make_label("Lyricist Pro", 16, C['indigo500'], bold=True))
        sl.addWidget(make_label("PREMIUM ACCOUNT", 9, C['slate400'], bold=True, caps=True))
        sl.addSpacing(28)

        nav_items = [
            ("\U0001F4C2", "Recent Projects", False),
            ("\u2728", "AI Studio", True),
            ("\u23F1", "Timing Sync", False),
            ("\U0001F58C", "Visualizer", False),
            ("\u2B07\uFE0F", "Export Hub", False),
        ]
        for icon, label, active in nav_items:
            btn = QPushButton(f"  {icon}  {label}")
            btn.setFixedHeight(42)
            btn.setCursor(Qt.PointingHandCursor)
            if active:
                btn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: rgba(99,102,241,0.1); "
                    f"color: {C['indigo400']}; border: none; border-left: 3px solid {C['indigo500']}; "
                    f"border-radius: 8px; padding-left: 12px; font-size: 13px; font-weight: 600; }}"
                )
            else:
                btn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: transparent; "
                    f"color: {C['slate400']}; border: none; border-radius: 8px; "
                    f"padding-left: 16px; font-size: 13px; }}"
                    f"QPushButton:hover {{ background: rgba(255,255,255,0.05); }}"
                )
            sl.addWidget(btn)
        sl.addStretch()
        return sidebar

    def _build_waveform_dock(self):
        dock = QWidget()
        dock.setMinimumHeight(160)
        dock.setStyleSheet(
            f"background: rgba(23,31,51,0.7); "
            f"border: 1px solid rgba(255,255,255,0.08); "
            f"border-top-left-radius: 24px; border-top-right-radius: 24px;"
        )
        dl = QVBoxLayout(dock)
        dl.setContentsMargins(20, 16, 20, 12)
        dl.setSpacing(12)

        # timeline markers
        markers = QHBoxLayout()
        for t in ["00:10", "00:15", "00:15.8", "00:20", "00:25"]:
            lbl = make_label(t, 9, C['primary'] if t == "00:15.8" else C['outline'],
                             bold=(t == "00:15.8"), caps=True)
            markers.addWidget(lbl)
        dl.addLayout(markers)

        # waveform
        waveform = WaveformWidget(50, 0.5)
        waveform.setMaximumHeight(60)
        dl.addWidget(waveform)

        # controls
        controls = QHBoxLayout()
        controls.setSpacing(12)

        prev_btn = QPushButton("\u23EE")
        prev_btn.setStyleSheet(
            f"QPushButton {{ color: {C['slate400']}; font-size: 20px; background: transparent; border: none; }}"
        )
        controls.addWidget(prev_btn)

        play_btn = QPushButton("\u23F8")
        play_btn.setFixedSize(48, 48)
        play_btn.setStyleSheet(gradient_button_style(radius=24, pad="0"))
        play_btn.setCursor(Qt.PointingHandCursor)
        controls.addWidget(play_btn)

        next_btn = QPushButton("\u23ED")
        next_btn.setStyleSheet(
            f"QPushButton {{ color: {C['slate400']}; font-size: 20px; background: transparent; border: none; }}"
        )
        controls.addWidget(next_btn)

        controls.addSpacing(20)

        # zoom slider
        zoom_w = QWidget()
        zoom_w.setStyleSheet(
            f"background: {C['surface_container_high']}; border-radius: 14px; "
            f"border: 1px solid rgba(255,255,255,0.05);"
        )
        zl = QHBoxLayout(zoom_w)
        zl.setContentsMargins(10, 4, 10, 4)
        zl.addWidget(make_label("\U0001F50D-", 11, C['slate400']))
        zoom_slider = QSlider(Qt.Horizontal)
        zoom_slider.setFixedWidth(80)
        zoom_slider.setValue(50)
        zl.addWidget(zoom_slider)
        zl.addWidget(make_label("\U0001F50D+", 11, C['slate400']))

        sep = QFrame()
        sep.setFixedSize(1, 20)
        sep.setStyleSheet("background: rgba(255,255,255,0.1);")
        zl.addWidget(sep)

        zl.addWidget(make_label("\U0001F50A", 14, C['slate400']))
        vol_slider = QSlider(Qt.Horizontal)
        vol_slider.setFixedWidth(80)
        vol_slider.setValue(70)
        zl.addWidget(vol_slider)
        controls.addWidget(zoom_w)

        controls.addStretch()

        discard_btn = QPushButton("DISCARD")
        discard_btn.setStyleSheet(
            f"QPushButton {{ background: rgba(255,255,255,0.05); color: {C['on_surface']}; "
            f"border: 1px solid rgba(255,255,255,0.1); border-radius: 10px; padding: 8px 20px; "
            f"font-size: 11px; font-weight: 700; letter-spacing: 1.5px; }}"
            f"QPushButton:hover {{ background: rgba(255,255,255,0.1); }}"
        )
        discard_btn.setCursor(Qt.PointingHandCursor)
        controls.addWidget(discard_btn)

        finalize_btn = QPushButton("FINALIZE SYNC")
        finalize_btn.setStyleSheet(
            f"QPushButton {{ background: {C['primary']}; color: {C['on_primary']}; "
            f"border: none; border-radius: 10px; padding: 8px 24px; "
            f"font-size: 11px; font-weight: 700; letter-spacing: 1.5px; }}"
            f"QPushButton:hover {{ background: {C['primary_container']}; }}"
        )
        finalize_btn.setCursor(Qt.PointingHandCursor)
        controls.addWidget(finalize_btn)

        dl.addLayout(controls)
        return dock


# ---------------------------------------------------------------------------
# PAGE 5: Styling & Preview
# ---------------------------------------------------------------------------

class StylingPreviewPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")

        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        ml = QVBoxLayout(self)
        ml.setContentsMargins(0, 0, 0, 0)
        ml.addWidget(scroll)

        container = QWidget()
        container.setStyleSheet("background: transparent;")
        scroll.setWidget(container)
        layout = QHBoxLayout(container)
        layout.setContentsMargins(24, 24, 24, 24)
        layout.setSpacing(24)

        # left column: controls
        left = QVBoxLayout()
        left.setSpacing(20)

        left.addWidget(make_label("Style Studio", 28, C['on_surface'], bold=True))
        left.addWidget(make_label("Customize your lyrics for the perfect visual rhythm.",
                                  13, C['on_surface_variant']))

        # typography
        typo_panel = GlassPanel(radius=12)
        tl = QVBoxLayout(typo_panel)
        tl.setContentsMargins(18, 18, 18, 18)
        tl.setSpacing(10)
        typo_header = QHBoxLayout()
        typo_header.addWidget(make_label("\u2712", 16, C['primary']))
        typo_header.addWidget(make_label("TYPOGRAPHY", 10, C['primary'], bold=True, caps=True))
        typo_header.addStretch()
        tl.addLayout(typo_header)

        fonts = [("Spline Sans (Default)", True), ("Vietnam Pro", False), ("Cormorant Display", False)]
        for fname, selected in fonts:
            fbtn = QPushButton(fname)
            if selected:
                fbtn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: rgba(99,102,241,0.1); "
                    f"color: {C['indigo400']}; border: none; border-left: 3px solid {C['indigo500']}; "
                    f"border-radius: 6px; padding: 10px 14px; font-size: 13px; }}"
                )
            else:
                fbtn.setStyleSheet(
                    f"QPushButton {{ text-align: left; background: transparent; "
                    f"color: {C['slate400']}; border: none; border-left: 3px solid transparent; "
                    f"border-radius: 6px; padding: 10px 14px; font-size: 13px; }}"
                    f"QPushButton:hover {{ background: rgba(255,255,255,0.05); }}"
                )
            fbtn.setCursor(Qt.PointingHandCursor)
            tl.addWidget(fbtn)
        left.addWidget(typo_panel)

        # color presets
        color_panel = GlassPanel(radius=12)
        cpl = QVBoxLayout(color_panel)
        cpl.setContentsMargins(18, 18, 18, 18)
        cpl.setSpacing(14)

        vib_header = QHBoxLayout()
        vib_header.addWidget(make_label("\U0001F3A8", 16, C['tertiary']))
        vib_header.addWidget(make_label("VIBRANT PRESETS", 10, C['tertiary'], bold=True, caps=True))
        vib_header.addStretch()
        cpl.addLayout(vib_header)

        grad_row = QHBoxLayout()
        grad_row.setSpacing(12)
        grads = [
            ([C['indigo500'], C['purple500']], True),
            (["#fb923c", "#f43f5e"], False),
            (["#34d399", "#22d3ee"], False),
            (["#fde047", "#d97706"], False),
        ]
        for colors, sel in grads:
            sw = ColorSwatch(colors_grad=colors, size=36)
            sw.set_selected(sel)
            grad_row.addWidget(sw)
        grad_row.addStretch()
        cpl.addLayout(grad_row)

        solid_header = QHBoxLayout()
        solid_header.addWidget(make_label("\U0001F58C", 14, C['on_surface_variant']))
        solid_header.addWidget(make_label("SOLID TONES", 10, C['on_surface_variant'], bold=True, caps=True))
        solid_header.addStretch()
        cpl.addLayout(solid_header)

        solid_row = QHBoxLayout()
        solid_row.setSpacing(8)
        solids = ["#ffffff", C['primary_container'], C['secondary_container'],
                  C['tertiary_container'], None]
        for s in solids:
            if s:
                sw = ColorSwatch(solid=s, size=32)
            else:
                sw = QPushButton("+")
                sw.setFixedSize(32, 32)
                sw.setStyleSheet(
                    f"QPushButton {{ background: {C['slate400']}; color: {C['slate900']}; "
                    f"border-radius: 6px; font-size: 16px; font-weight: bold; border: none; }}"
                )
            solid_row.addWidget(sw)
        solid_row.addStretch()
        cpl.addLayout(solid_row)

        left.addWidget(color_panel)

        export_btn = QPushButton("\u2B07  EXPORT SYNCED LYRICS")
        export_btn.setStyleSheet(gradient_button_style(pad="14px 24px", radius=12))
        export_btn.setCursor(Qt.PointingHandCursor)
        left.addWidget(export_btn)
        left.addStretch()

        left_widget = QWidget()
        left_widget.setLayout(left)
        left_widget.setMaximumWidth(340)
        layout.addWidget(left_widget)

        # right column: preview
        right = QVBoxLayout()
        right.setSpacing(16)

        # main preview
        preview_panel = GlassPanel(radius=16, bg="rgba(15,23,42,0.8)")
        preview_panel.setMinimumHeight(300)
        pl = QVBoxLayout(preview_panel)
        pl.setContentsMargins(0, 0, 0, 0)
        pl.setSpacing(0)

        # lyrics overlay area
        lyrics_area = QWidget()
        lyrics_area.setMinimumHeight(260)
        lyrics_area.setStyleSheet(
            f"background: qlineargradient(x1:0,y1:0,x2:1,y2:1, "
            f"stop:0 rgba(30,20,60,0.8), stop:1 rgba(10,15,40,0.9)); "
            f"border-radius: 16px;"
        )
        ll = QVBoxLayout(lyrics_area)
        ll.setAlignment(Qt.AlignCenter)
        ll.setSpacing(16)

        prev_line = make_label("Lost in the frequencies of the night", 18, f"{C['on_surface']}60")
        prev_line.setAlignment(Qt.AlignCenter)
        ll.addWidget(prev_line)

        current_line = GradientLabel("Chasing the echoes until the light")
        current_line.setAlignment(Qt.AlignCenter)
        font = QFont("Segoe UI", 22, QFont.Bold)
        font.setItalic(True)
        current_line.setFont(font)
        current_line.setMinimumHeight(60)
        current_line.setWordWrap(True)
        ll.addWidget(current_line)

        next_line = make_label("Where silence meets the blinding white", 18, f"{C['on_surface']}60")
        next_line.setAlignment(Qt.AlignCenter)
        ll.addWidget(next_line)

        # playback controls overlay
        controls = QHBoxLayout()
        controls.setAlignment(Qt.AlignCenter)
        controls.setSpacing(12)
        for icon in ["\u23EA", "\u25B6", "\u23E9"]:
            btn = QPushButton(icon)
            if icon == "\u25B6":
                btn.setFixedSize(36, 36)
                btn.setStyleSheet(
                    f"QPushButton {{ background: {C['primary']}; color: {C['on_primary']}; "
                    f"border-radius: 18px; font-size: 14px; border: none; }}"
                )
            else:
                btn.setStyleSheet(
                    f"QPushButton {{ color: {C['on_surface_variant']}; background: transparent; "
                    f"border: none; font-size: 16px; }}"
                )
            controls.addWidget(btn)
        ll.addLayout(controls)

        # progress bar
        prog = ProgressBarCustom(66)
        prog.setFixedHeight(3)
        ll.addWidget(prog)

        pl.addWidget(lyrics_area)
        right.addWidget(preview_panel)

        # info cards
        info_row = QHBoxLayout()
        info_row.setSpacing(12)
        info_data = [
            ("\u23F1", "ACTIVE LINE", "02:14.4", "Precision sync active", C['indigo400']),
            ("\u2728", "AI ENGINE", "Smoothing", "Transitions optimized", C['tertiary']),
            ("\U0001F441", "ACCESSIBILITY", "High Contrast", "Passes WCAG 2.1", C['secondary']),
        ]
        for icon, label, value, desc, color in info_data:
            card = GlassPanel(radius=12)
            cl = QVBoxLayout(card)
            cl.setContentsMargins(16, 14, 16, 14)
            cl.setSpacing(6)
            top = QHBoxLayout()
            top.addWidget(make_label(icon, 16, color))
            top.addStretch()
            top.addWidget(make_label(label, 9, C['on_surface_variant'], bold=True, caps=True))
            cl.addLayout(top)
            cl.addWidget(make_label(value, 20, C['on_surface'], bold=True))
            cl.addWidget(make_label(desc, 12, C['on_surface_variant']))
            info_row.addWidget(card)
        right.addLayout(info_row)
        right.addStretch()

        right_widget = QWidget()
        right_widget.setLayout(right)
        layout.addWidget(right_widget, 1)


# ---------------------------------------------------------------------------
# PAGE 6: Export Settings
# ---------------------------------------------------------------------------

class ExportFormatCard(GlassPanel):
    """Export format selection card."""
    def __init__(self, title, description, badge_text, selected=False, parent=None):
        border = f"{C['indigo500']}60" if selected else "rgba(255,255,255,0.1)"
        bg = "rgba(23,31,51,0.6)"
        super().__init__(parent, radius=12, border_color=border, bg=bg)
        if selected:
            self.add_glow(QColor(99, 102, 241, 50), 20)
        self.setCursor(Qt.PointingHandCursor)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(10)

        top_row = QHBoxLayout()
        icon_w = QWidget()
        icon_w.setFixedSize(40, 40)
        icon_bg = C['indigo500'] if selected else f"{C['indigo500']}30"
        icon_color = C['on_primary'] if selected else C['indigo400']
        icon_w.setStyleSheet(f"background: {icon_bg}; border-radius: 8px;")
        iw_l = QVBoxLayout(icon_w)
        iw_l.setContentsMargins(0, 0, 0, 0)
        iw_l.addWidget(make_label("\u2630" if "Line" in title else "\u2261", 18, icon_color))
        top_row.addWidget(icon_w)
        top_row.addStretch()

        badge = QLabel(badge_text.upper())
        badge_color = C['primary'] if selected else C['indigo400']
        badge.setStyleSheet(
            f"background: {badge_color}30; color: {badge_color}; "
            f"font-size: 9px; font-weight: 700; letter-spacing: 1.5px; "
            f"padding: 4px 10px; border-radius: 10px;"
        )
        top_row.addWidget(badge)
        if selected:
            check = QLabel("\u2714")
            check.setStyleSheet(f"color: {C['indigo400']}; font-size: 16px; background: transparent;")
            top_row.addWidget(check)
        layout.addLayout(top_row)

        layout.addWidget(make_label(title, 20, C['on_surface'], bold=True))
        layout.addWidget(make_label(description, 12, C['on_surface_variant']))

        layout.addStretch()
        action_text = "Active Selection" if selected else "Select Format \u2192"
        layout.addWidget(make_label(action_text, 9, C['indigo400'], bold=True, caps=True))


class ToggleSwitch(QWidget):
    """Custom toggle switch widget."""
    toggled = Signal(bool)

    def __init__(self, checked=False, parent=None):
        super().__init__(parent)
        self._checked = checked
        self.setFixedSize(48, 26)
        self.setCursor(Qt.PointingHandCursor)

    def is_checked(self):
        return self._checked

    def mousePressEvent(self, event):
        self._checked = not self._checked
        self.toggled.emit(self._checked)
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.Antialiasing)
        w, h = self.width(), self.height()

        # track
        track_color = QColor(C['indigo500']) if self._checked else QColor(C['surface_container_highest'])
        p.setBrush(track_color)
        p.setPen(Qt.NoPen)
        p.drawRoundedRect(0, 0, w, h, h / 2, h / 2)

        # thumb
        p.setBrush(QColor("white"))
        thumb_x = w - h + 3 if self._checked else 3
        p.drawEllipse(int(thumb_x), 3, h - 6, h - 6)
        p.end()


class ExportSettingsPage(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background: {C['background']};")

        scroll = QScrollArea(self)
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        ml = QVBoxLayout(self)
        ml.setContentsMargins(0, 0, 0, 0)
        ml.addWidget(scroll)

        container = QWidget()
        container.setStyleSheet("background: transparent;")
        scroll.setWidget(container)
        layout = QVBoxLayout(container)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(24)

        inner = QVBoxLayout()
        inner.setContentsMargins(40, 30, 40, 60)
        inner.setSpacing(24)

        inner.addWidget(make_label("Export Hub", 36, C['on_surface'], bold=True))
        inner.addWidget(make_label(
            "Finalize your transcription. Choose the perfect format for your "
            "visualizer or lyric video delivery.",
            16, C['on_surface_variant']
        ))

        # format cards
        cards_row = QHBoxLayout()
        cards_row.setSpacing(16)

        card1 = ExportFormatCard(
            "Line-by-Line",
            "Optimal for standard lyric videos and traditional closed captioning. "
            "High readability for dense lyrics.",
            "SRT Standard",
        )
        cards_row.addWidget(card1)

        card2 = ExportFormatCard(
            "Word-by-Word",
            "Precision timing for karaoke-style visualizers and dynamic kinetic "
            "typography. Frame-perfect sync.",
            "Advanced",
            selected=True,
        )
        cards_row.addWidget(card2)
        inner.addLayout(cards_row)

        # settings
        settings_panel = GlassPanel(radius=12)
        spl = QVBoxLayout(settings_panel)
        spl.setContentsMargins(0, 0, 0, 0)
        spl.setSpacing(0)

        settings_data = [
            ("English Transcription",
             "Enable AI-powered translation for international audiences.", True),
            ("Metadata Injection",
             "Include track title, artist name, and BPM in the file header.", False),
        ]
        for i, (title, desc, checked) in enumerate(settings_data):
            row = QWidget()
            rl = QHBoxLayout(row)
            rl.setContentsMargins(20, 18, 20, 18)
            left = QVBoxLayout()
            left.setSpacing(4)
            left.addWidget(make_label(title, 16, C['on_surface'], bold=True))
            left.addWidget(make_label(desc, 12, C['on_surface_variant']))
            rl.addLayout(left)
            rl.addStretch()
            toggle = ToggleSwitch(checked)
            rl.addWidget(toggle)
            spl.addWidget(row)

            if i < len(settings_data) - 1:
                sep = QFrame()
                sep.setFixedHeight(1)
                sep.setStyleSheet("background: rgba(255,255,255,0.05);")
                spl.addWidget(sep)

        inner.addWidget(settings_panel)

        # generate button
        gen_btn = QPushButton("\u2728  Generate & Export")
        gen_btn.setStyleSheet(
            f"QPushButton {{ "
            f"  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, "
            f"  stop:0 {C['indigo600']}, stop:0.5 {C['purple600']}, stop:1 {C['indigo600']}); "
            f"  color: white; border: none; border-radius: 16px; padding: 20px; "
            f"  font-size: 20px; font-weight: 700; }}"
            f"QPushButton:hover {{ "
            f"  background: qlineargradient(x1:0,y1:0,x2:1,y2:0, "
            f"  stop:0 {C['purple600']}, stop:0.5 {C['indigo600']}, stop:1 {C['purple600']}); }}"
            f"QPushButton:pressed {{ padding-top: 22px; }}"
        )
        gen_btn.setCursor(Qt.PointingHandCursor)
        gen_btn.setMinimumHeight(64)
        gen_btn.clicked.connect(self._export)
        inner.addWidget(gen_btn)

        inner.addWidget(make_label(
            "Estimated processing time: ~14 seconds",
            12, C['on_surface_variant']
        ))

        inner.addStretch()
        layout.addLayout(inner)

    def _export(self):
        QMessageBox.information(self, "Export", "Export generation would start here.")


# ---------------------------------------------------------------------------
# Main Window
# ---------------------------------------------------------------------------

class LyriqMainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("LYRIQ \u2014 Multilingual Lyric Studio")
        self.setMinimumSize(1100, 750)
        self.resize(1280, 820)

        # central widget
        central = QWidget()
        central.setStyleSheet(f"background: {C['background']};")
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        main_layout.setContentsMargins(0, 0, 0, 0)
        main_layout.setSpacing(0)

        # top bar
        self.top_bar = TopAppBar()
        main_layout.addWidget(self.top_bar)

        # pages
        self.stack = QStackedWidget()
        self.library_page = LyricsLibraryPage()
        self.audio_page = AudioImportPage()
        self.editor_page = LyricsEditorPage()
        self.sync_page = FineTuningSyncPage()
        self.style_page = StylingPreviewPage()
        self.export_page = ExportSettingsPage()

        self.stack.addWidget(self.library_page)   # 0 - Library
        self.stack.addWidget(self.audio_page)      # 1 - Studio (Audio Import)
        self.stack.addWidget(self.editor_page)     # 2 - (internal: lyrics editor)
        self.stack.addWidget(self.sync_page)       # 3 - Sync (Fine-Tuning)
        self.stack.addWidget(self.style_page)      # 4 - Style
        self.stack.addWidget(self.export_page)     # 5 - Export

        main_layout.addWidget(self.stack, 1)

        # bottom nav
        self.bottom_nav = BottomNavBar()
        self.bottom_nav.tab_changed.connect(self._on_tab_changed)
        main_layout.addWidget(self.bottom_nav)

    def _on_tab_changed(self, idx):
        # Map bottom nav indices to stack indices:
        # 0->Library(0), 1->Studio/AudioImport(1), 2->Sync(3), 3->Style(4), 4->Export(5)
        mapping = {0: 0, 1: 1, 2: 3, 3: 4, 4: 5}
        self.stack.setCurrentIndex(mapping.get(idx, 0))


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    app = QApplication(sys.argv)
    app.setStyle("Fusion")

    # Dark palette
    palette = QPalette()
    palette.setColor(QPalette.Window, QColor(C['background']))
    palette.setColor(QPalette.WindowText, QColor(C['on_surface']))
    palette.setColor(QPalette.Base, QColor(C['surface_container']))
    palette.setColor(QPalette.AlternateBase, QColor(C['surface_container_high']))
    palette.setColor(QPalette.ToolTipBase, QColor(C['surface_container_highest']))
    palette.setColor(QPalette.ToolTipText, QColor(C['on_surface']))
    palette.setColor(QPalette.Text, QColor(C['on_surface']))
    palette.setColor(QPalette.Button, QColor(C['surface_container_high']))
    palette.setColor(QPalette.ButtonText, QColor(C['on_surface']))
    palette.setColor(QPalette.BrightText, QColor(C['primary']))
    palette.setColor(QPalette.Highlight, QColor(C['indigo500']))
    palette.setColor(QPalette.HighlightedText, QColor("white"))
    app.setPalette(palette)

    app.setStyleSheet(GLOBAL_QSS)

    window = LyriqMainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
