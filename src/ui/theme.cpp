#include "theme.h"
#include <QFont>

namespace DaVinciTheme {

void apply(QApplication *app)
{
    // Set palette
    QPalette palette;
    palette.setColor(QPalette::Window, Colors::Background);
    palette.setColor(QPalette::WindowText, Colors::TextPrimary);
    palette.setColor(QPalette::Base, Colors::Surface);
    palette.setColor(QPalette::AlternateBase, Colors::SurfaceLight);
    palette.setColor(QPalette::ToolTipBase, Colors::SurfaceLight);
    palette.setColor(QPalette::ToolTipText, Colors::TextPrimary);
    palette.setColor(QPalette::Text, Colors::TextPrimary);
    palette.setColor(QPalette::Button, Colors::Surface);
    palette.setColor(QPalette::ButtonText, Colors::TextPrimary);
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Link, Colors::Accent);
    palette.setColor(QPalette::Highlight, Colors::Accent);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Colors::TextDisabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Colors::TextDisabled);
    palette.setColor(QPalette::Mid, Colors::Border);
    palette.setColor(QPalette::Dark, Colors::PanelHeader);
    palette.setColor(QPalette::Shadow, QColor(0, 0, 0));

    app->setPalette(palette);
    app->setStyleSheet(styleSheet());

    QFont defaultFont("Segoe UI", 9);
    defaultFont.setStyleStrategy(QFont::PreferAntialias);
    app->setFont(defaultFont);
}

QString styleSheet()
{
    return QStringLiteral(R"(
        * {
            outline: none;
        }

        QMainWindow {
            background-color: #181818;
        }

        QDockWidget {
            background-color: #1c1c1c;
            border: 1px solid #373737;
            titlebar-close-icon: none;
            titlebar-normal-icon: none;
        }

        QDockWidget::title {
            background-color: #141414;
            padding: 6px 8px;
            font-weight: bold;
            font-size: 11px;
            color: #d2d2d2;
            border-bottom: 1px solid #373737;
        }

        QMenuBar {
            background-color: #141414;
            color: #d2d2d2;
            border-bottom: 1px solid #373737;
            padding: 2px;
        }

        QMenuBar::item {
            padding: 4px 10px;
            background: transparent;
        }

        QMenuBar::item:selected {
            background-color: #2a2a2a;
            border-radius: 3px;
        }

        QMenu {
            background-color: #202020;
            border: 1px solid #373737;
            color: #d2d2d2;
            padding: 4px;
        }

        QMenu::item {
            padding: 6px 24px 6px 12px;
            border-radius: 3px;
        }

        QMenu::item:selected {
            background-color: #488ce6;
            color: white;
        }

        QMenu::separator {
            height: 1px;
            background: #373737;
            margin: 4px 8px;
        }

        QPushButton {
            background-color: #2a2a2a;
            color: #d2d2d2;
            border: 1px solid #373737;
            padding: 5px 14px;
            border-radius: 3px;
            min-height: 22px;
            font-size: 11px;
        }

        QPushButton:hover {
            background-color: #343434;
            border-color: #464646;
        }

        QPushButton:pressed {
            background-color: #202020;
        }

        QPushButton:disabled {
            color: #505050;
            background-color: #1e1e1e;
        }

        QPushButton#accentButton {
            background-color: #488ce6;
            border-color: #5a9bf0;
            color: white;
        }

        QPushButton#accentButton:hover {
            background-color: #5a9bf0;
        }

        QToolBar {
            background-color: #1c1c1c;
            border: none;
            padding: 2px;
            spacing: 2px;
        }

        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 3px;
            padding: 4px;
            color: #d2d2d2;
        }

        QToolButton:hover {
            background-color: #343434;
            border-color: #464646;
        }

        QToolButton:pressed, QToolButton:checked {
            background-color: #488ce6;
            color: white;
        }

        QTabWidget::pane {
            border: 1px solid #373737;
            background-color: #1c1c1c;
        }

        QTabBar::tab {
            background-color: #202020;
            color: #969696;
            padding: 6px 16px;
            border: 1px solid #373737;
            border-bottom: none;
            min-width: 60px;
        }

        QTabBar::tab:selected {
            background-color: #1c1c1c;
            color: #d2d2d2;
            border-bottom: 2px solid #488ce6;
        }

        QTabBar::tab:hover:!selected {
            background-color: #2a2a2a;
            color: #d2d2d2;
        }

        QScrollBar:vertical {
            background: #1c1c1c;
            width: 10px;
            border: none;
        }

        QScrollBar::handle:vertical {
            background: #3a3a3a;
            min-height: 30px;
            border-radius: 4px;
            margin: 2px;
        }

        QScrollBar::handle:vertical:hover {
            background: #4a4a4a;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar:horizontal {
            background: #1c1c1c;
            height: 10px;
            border: none;
        }

        QScrollBar::handle:horizontal {
            background: #3a3a3a;
            min-width: 30px;
            border-radius: 4px;
            margin: 2px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #4a4a4a;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }

        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #202020;
            color: #d2d2d2;
            border: 1px solid #373737;
            border-radius: 3px;
            padding: 4px 8px;
            selection-background-color: #488ce6;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border-color: #488ce6;
        }

        QComboBox {
            background-color: #2a2a2a;
            color: #d2d2d2;
            border: 1px solid #373737;
            border-radius: 3px;
            padding: 4px 8px;
            min-height: 22px;
        }

        QComboBox::drop-down {
            border: none;
            width: 20px;
        }

        QComboBox QAbstractItemView {
            background-color: #202020;
            border: 1px solid #373737;
            color: #d2d2d2;
            selection-background-color: #488ce6;
        }

        QSpinBox, QDoubleSpinBox {
            background-color: #202020;
            color: #d2d2d2;
            border: 1px solid #373737;
            border-radius: 3px;
            padding: 2px 6px;
        }

        QSlider::groove:horizontal {
            background: #2a2a2a;
            height: 4px;
            border-radius: 2px;
        }

        QSlider::handle:horizontal {
            background: #488ce6;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }

        QSlider::handle:horizontal:hover {
            background: #5a9bf0;
        }

        QGroupBox {
            border: 1px solid #373737;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 12px;
            color: #d2d2d2;
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
            color: #969696;
            font-size: 10px;
            text-transform: uppercase;
        }

        QCheckBox {
            color: #d2d2d2;
            spacing: 6px;
        }

        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #373737;
            border-radius: 3px;
            background: #202020;
        }

        QCheckBox::indicator:checked {
            background: #488ce6;
            border-color: #488ce6;
        }

        QLabel {
            color: #d2d2d2;
        }

        QLabel#sectionLabel {
            color: #969696;
            font-size: 10px;
            font-weight: bold;
            text-transform: uppercase;
            padding: 4px 0;
        }

        QSplitter::handle {
            background-color: #373737;
        }

        QSplitter::handle:horizontal {
            width: 2px;
        }

        QSplitter::handle:vertical {
            height: 2px;
        }

        QStatusBar {
            background-color: #141414;
            color: #969696;
            border-top: 1px solid #373737;
            font-size: 10px;
        }

        QListWidget {
            background-color: #202020;
            border: 1px solid #373737;
            color: #d2d2d2;
            border-radius: 3px;
        }

        QListWidget::item {
            padding: 4px 8px;
            border-radius: 3px;
        }

        QListWidget::item:selected {
            background-color: #488ce6;
            color: white;
        }

        QListWidget::item:hover:!selected {
            background-color: #2a2a2a;
        }

        QTreeWidget {
            background-color: #202020;
            border: 1px solid #373737;
            color: #d2d2d2;
        }

        QTreeWidget::item {
            padding: 3px;
        }

        QTreeWidget::item:selected {
            background-color: #488ce6;
            color: white;
        }

        QProgressBar {
            background-color: #2a2a2a;
            border: 1px solid #373737;
            border-radius: 3px;
            text-align: center;
            color: #d2d2d2;
            height: 16px;
        }

        QProgressBar::chunk {
            background-color: #488ce6;
            border-radius: 2px;
        }

        QHeaderView::section {
            background-color: #202020;
            color: #969696;
            border: 1px solid #373737;
            padding: 4px 8px;
            font-size: 10px;
        }

        QColorDialog, QFontDialog, QFileDialog {
            background-color: #1c1c1c;
        }
    )");
}

} // namespace DaVinciTheme
