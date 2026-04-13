#pragma once

#include <QMainWindow>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QCloseEvent>

#include "core/project.h"
#include "ui/previewpanel.h"
#include "ui/timelinepanel.h"
#include "ui/mediabrowser.h"
#include "ui/captioneditor.h"
#include "ui/inspectorpanel.h"
#include "ui/brandkitpanel.h"
#include "ui/presetspanel.h"
#include "export/exporter.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();

    // Edit
    void undo();
    void redo();
    void deleteSelected();
    void selectAll();

    // Captions
    void addCaption();
    void duplicateCaption();

    // Export
    void exportProject();
    void exportCaptions();

    // View
    void resetLayout();

    // Timeline events
    void onClipSelected(const QUuid &id);
    void onCaptionSelected(const QUuid &id);
    void onAddToTimeline(const QString &path);

    // Preset events
    void onPresetApplied(const CaptionPreset &preset);
    void onPresetSaveRequested(const CaptionStyle &style, const QString &name);

    // Brand kit events
    void onApplyBrandToCaption();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void setupConnections();
    void updateTitle();
    void addSampleContent();

    Project *m_project;
    Exporter *m_exporter;

    // Panels
    PreviewPanel *m_previewPanel;
    TimelinePanel *m_timelinePanel;
    MediaBrowser *m_mediaBrowser;
    CaptionEditor *m_captionEditor;
    InspectorPanel *m_inspectorPanel;
    BrandKitPanel *m_brandKitPanel;
    PresetsPanel *m_presetsPanel;

    // Dock widgets
    QDockWidget *m_mediaDock;
    QDockWidget *m_captionDock;
    QDockWidget *m_inspectorDock;
    QDockWidget *m_brandKitDock;
    QDockWidget *m_presetsDock;

    // Status bar labels
    QLabel *m_resolutionLabel;
    QLabel *m_fpsLabel;
    QLabel *m_zoomLabel;

    // Currently selected caption for editing
    Caption *m_selectedCaption = nullptr;
};
