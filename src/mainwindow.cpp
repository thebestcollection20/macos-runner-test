#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_project(new Project(this))
    , m_exporter(new Exporter(this))
{
    setWindowTitle("CaptionStudio");
    setMinimumSize(1200, 700);
    resize(1400, 850);

    // Create panels
    m_previewPanel = new PreviewPanel;
    m_timelinePanel = new TimelinePanel;
    m_mediaBrowser = new MediaBrowser;
    m_captionEditor = new CaptionEditor;
    m_inspectorPanel = new InspectorPanel;
    m_brandKitPanel = new BrandKitPanel;
    m_presetsPanel = new PresetsPanel;

    setupMenuBar();
    setupToolBar();
    setupDockWidgets();
    setupStatusBar();
    setupConnections();

    // Initialize with project
    m_previewPanel->setTimeline(m_project->timeline());
    m_timelinePanel->setTimeline(m_project->timeline());
    m_mediaBrowser->setProject(m_project);
    m_exporter->setProject(m_project);

    addSampleContent();
    updateTitle();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
    // File menu
    auto *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New Project", this, &MainWindow::newProject, QKeySequence::New);
    fileMenu->addAction("&Open Project...", this, &MainWindow::openProject, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("&Save", this, &MainWindow::saveProject, QKeySequence::Save);
    fileMenu->addAction("Save &As...", this, &MainWindow::saveProjectAs, QKeySequence("Ctrl+Shift+S"));
    fileMenu->addSeparator();
    fileMenu->addAction("Import &Media...", m_mediaBrowser, &MediaBrowser::importMedia, QKeySequence("Ctrl+I"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Export Project...", this, &MainWindow::exportProject, QKeySequence("Ctrl+E"));
    fileMenu->addAction("Export &Captions...", this, &MainWindow::exportCaptions, QKeySequence("Ctrl+Shift+E"));
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

    // Edit menu
    auto *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("&Undo", this, &MainWindow::undo, QKeySequence::Undo);
    editMenu->addAction("&Redo", this, &MainWindow::redo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("&Delete", this, &MainWindow::deleteSelected, QKeySequence::Delete);
    editMenu->addAction("Select &All", this, &MainWindow::selectAll, QKeySequence::SelectAll);

    // Caption menu
    auto *captionMenu = menuBar()->addMenu("&Caption");
    captionMenu->addAction("&Add Caption", this, &MainWindow::addCaption, QKeySequence("Ctrl+T"));
    captionMenu->addAction("&Duplicate Caption", this, &MainWindow::duplicateCaption, QKeySequence("Ctrl+D"));
    captionMenu->addSeparator();
    captionMenu->addAction("Apply &Brand Kit", this, &MainWindow::onApplyBrandToCaption);

    // View menu
    auto *viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Reset Layout", this, &MainWindow::resetLayout);
    viewMenu->addSeparator();
    viewMenu->addAction("Zoom &In", m_timelinePanel, &TimelinePanel::zoomIn, QKeySequence("Ctrl+="));
    viewMenu->addAction("Zoom &Out", m_timelinePanel, &TimelinePanel::zoomOut, QKeySequence("Ctrl+-"));
    viewMenu->addAction("&Fit to Window", m_timelinePanel, &TimelinePanel::fitToWindow, QKeySequence("Ctrl+0"));

    // Playback menu
    auto *playbackMenu = menuBar()->addMenu("&Playback");
    playbackMenu->addAction("&Play/Pause", m_previewPanel, &PreviewPanel::togglePlay, QKeySequence("Space"));
    playbackMenu->addAction("&Stop", m_previewPanel, &PreviewPanel::stop, QKeySequence("Escape"));
    playbackMenu->addAction("Step &Forward", m_previewPanel, &PreviewPanel::stepForward, QKeySequence("Right"));
    playbackMenu->addAction("Step &Backward", m_previewPanel, &PreviewPanel::stepBackward, QKeySequence("Left"));

    // Help menu
    auto *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About CaptionStudio", [this]() {
        QMessageBox::about(this, "About CaptionStudio",
            "<h2>CaptionStudio v1.0</h2>"
            "<p>A video editing application focused on caption styling, "
            "presets, and brand identity management.</p>"
            "<p>Built with Qt 5 and C++17</p>");
    });
}

void MainWindow::setupToolBar()
{
    auto *toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20, 20));

    toolbar->addAction("New", this, &MainWindow::newProject);
    toolbar->addAction("Open", this, &MainWindow::openProject);
    toolbar->addAction("Save", this, &MainWindow::saveProject);
    toolbar->addSeparator();
    toolbar->addAction("Import", m_mediaBrowser, &MediaBrowser::importMedia);
    toolbar->addSeparator();
    toolbar->addAction("Add Caption", this, &MainWindow::addCaption);
    toolbar->addSeparator();
    toolbar->addAction("Export", this, &MainWindow::exportProject);
}

void MainWindow::setupDockWidgets()
{
    setDockNestingEnabled(true);

    // Central widget: Preview + Timeline vertical split
    auto *centralSplitter = new QSplitter(Qt::Vertical);
    centralSplitter->addWidget(m_previewPanel);
    centralSplitter->addWidget(m_timelinePanel);
    centralSplitter->setStretchFactor(0, 2);
    centralSplitter->setStretchFactor(1, 1);
    setCentralWidget(centralSplitter);

    // Left: Media Browser
    m_mediaDock = new QDockWidget("Media Pool", this);
    m_mediaDock->setWidget(m_mediaBrowser);
    m_mediaDock->setMinimumWidth(200);
    addDockWidget(Qt::LeftDockWidgetArea, m_mediaDock);

    // Left: Presets (tabbed with Media)
    m_presetsDock = new QDockWidget("Presets", this);
    m_presetsDock->setWidget(m_presetsPanel);
    tabifyDockWidget(m_mediaDock, m_presetsDock);

    // Right: Caption Editor
    m_captionDock = new QDockWidget("Caption Editor", this);
    m_captionDock->setWidget(m_captionEditor);
    m_captionDock->setMinimumWidth(260);
    addDockWidget(Qt::RightDockWidgetArea, m_captionDock);

    // Right: Inspector (tabbed with Caption Editor)
    m_inspectorDock = new QDockWidget("Inspector", this);
    m_inspectorDock->setWidget(m_inspectorPanel);
    tabifyDockWidget(m_captionDock, m_inspectorDock);

    // Right: Brand Kit (tabbed)
    m_brandKitDock = new QDockWidget("Brand Kit", this);
    m_brandKitDock->setWidget(m_brandKitPanel);
    tabifyDockWidget(m_inspectorDock, m_brandKitDock);

    // Show Caption Editor tab by default
    m_captionDock->raise();
}

void MainWindow::setupStatusBar()
{
    m_resolutionLabel = new QLabel("1920x1080");
    m_resolutionLabel->setStyleSheet("padding: 0 8px;");
    m_fpsLabel = new QLabel("30 fps");
    m_fpsLabel->setStyleSheet("padding: 0 8px;");
    m_zoomLabel = new QLabel("100%");
    m_zoomLabel->setStyleSheet("padding: 0 8px;");

    statusBar()->addPermanentWidget(m_resolutionLabel);
    statusBar()->addPermanentWidget(m_fpsLabel);
    statusBar()->addPermanentWidget(m_zoomLabel);
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // Timeline -> Preview sync
    connect(m_project->timeline(), &Timeline::timeChanged, [this](qint64) {
        m_previewPanel->update();
    });

    // Timeline clip/caption selection
    connect(m_timelinePanel, &TimelinePanel::clipSelected, this, &MainWindow::onClipSelected);
    connect(m_timelinePanel, &TimelinePanel::captionSelected, this, &MainWindow::onCaptionSelected);
    connect(m_timelinePanel, &TimelinePanel::addCaptionRequested, this, &MainWindow::addCaption);

    // Media browser
    connect(m_mediaBrowser, &MediaBrowser::addToTimeline, this, &MainWindow::onAddToTimeline);

    // Caption editor changes -> update preview
    connect(m_captionEditor, &CaptionEditor::captionChanged, [this]() {
        m_previewPanel->update();
        m_timelinePanel->update();
        m_project->setModified(true);
        updateTitle();
    });

    // Preset events
    connect(m_presetsPanel, &PresetsPanel::presetApplied, this, &MainWindow::onPresetApplied);
    connect(m_captionEditor, &CaptionEditor::presetSaveRequested, this, &MainWindow::onPresetSaveRequested);

    // Brand kit
    connect(m_brandKitPanel, &BrandKitPanel::applyBrandToCaption, this, &MainWindow::onApplyBrandToCaption);

    // Inspector changes
    connect(m_inspectorPanel, &InspectorPanel::clipChanged, [this]() {
        m_timelinePanel->update();
        m_previewPanel->update();
        m_project->setModified(true);
        updateTitle();
    });

    // Exporter
    connect(m_exporter, &Exporter::exportCompleted, [this](const QString &path) {
        statusBar()->showMessage("Export completed: " + path, 5000);
    });
    connect(m_exporter, &Exporter::exportFailed, [this](const QString &error) {
        QMessageBox::critical(this, "Export Failed", error);
    });
    connect(m_exporter, &Exporter::progressChanged, [this](qreal progress) {
        statusBar()->showMessage(QString("Exporting... %1%").arg(static_cast<int>(progress * 100)));
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_project->isModified()) {
        auto result = QMessageBox::question(this, "Unsaved Changes",
            "You have unsaved changes. Save before closing?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Save) {
            if (saveProject())
                event->accept();
            else
                event->ignore();
        } else if (result == QMessageBox::Cancel) {
            event->ignore();
        } else {
            event->accept();
        }
    } else {
        event->accept();
    }
}

void MainWindow::newProject()
{
    if (m_project->isModified()) {
        auto result = QMessageBox::question(this, "Unsaved Changes",
            "Save current project first?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Save)
            saveProject();
        else if (result == QMessageBox::Cancel)
            return;
    }

    delete m_project;
    m_project = new Project(this);
    m_previewPanel->setTimeline(m_project->timeline());
    m_timelinePanel->setTimeline(m_project->timeline());
    m_mediaBrowser->setProject(m_project);
    m_exporter->setProject(m_project);
    m_captionEditor->clearCaption();
    m_inspectorPanel->clearClip();
    m_selectedCaption = nullptr;
    updateTitle();
    statusBar()->showMessage("New project created", 3000);
}

void MainWindow::openProject()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Project",
        QString(), "CaptionStudio Projects (*.csp);;All Files (*)");

    if (path.isEmpty()) return;

    Project *proj = Project::load(path, this);
    if (!proj) {
        QMessageBox::critical(this, "Error", "Failed to open project.");
        return;
    }

    delete m_project;
    m_project = proj;
    m_previewPanel->setTimeline(m_project->timeline());
    m_timelinePanel->setTimeline(m_project->timeline());
    m_mediaBrowser->setProject(m_project);
    m_exporter->setProject(m_project);
    m_captionEditor->clearCaption();
    m_inspectorPanel->clearClip();
    m_selectedCaption = nullptr;
    updateTitle();
    statusBar()->showMessage("Project opened: " + path, 3000);
}

bool MainWindow::saveProject()
{
    if (m_project->filePath().isEmpty()) {
        return saveProjectAs();
    }

    if (m_project->save()) {
        updateTitle();
        statusBar()->showMessage("Project saved", 3000);
        return true;
    } else {
        QMessageBox::critical(this, "Error", "Failed to save project.");
        return false;
    }
}

bool MainWindow::saveProjectAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save Project As",
        m_project->name() + ".csp",
        "CaptionStudio Projects (*.csp);;All Files (*)");

    if (path.isEmpty()) return false;

    if (m_project->saveAs(path)) {
        updateTitle();
        statusBar()->showMessage("Project saved: " + path, 3000);
        return true;
    } else {
        QMessageBox::critical(this, "Error", "Failed to save project.");
        return false;
    }
}

void MainWindow::undo()
{
    statusBar()->showMessage("Undo (not implemented yet)", 2000);
}

void MainWindow::redo()
{
    statusBar()->showMessage("Redo (not implemented yet)", 2000);
}

void MainWindow::deleteSelected()
{
    if (m_selectedCaption) {
        m_project->timeline()->removeCaption(m_selectedCaption->id());
        m_captionEditor->clearCaption();
        m_selectedCaption = nullptr;
        m_previewPanel->update();
        statusBar()->showMessage("Caption deleted", 2000);
    }
}

void MainWindow::selectAll()
{
    statusBar()->showMessage("Select All", 2000);
}

void MainWindow::addCaption()
{
    qint64 time = m_project->timeline()->currentTime();
    Caption cap("Enter your caption here", time, 3000);
    cap.setTrackIndex(1); // Caption track

    // Apply brand kit defaults
    BrandKit kit = BrandKitManager::instance().activeKit();
    if (!kit.primaryFontFamily().isEmpty()) {
        CaptionStyle style = cap.style();
        style.font.setFamily(kit.primaryFontFamily());
        cap.setStyle(style);
    }

    m_project->timeline()->addCaption(cap);

    // Select the new caption
    Caption *newCap = m_project->timeline()->captionById(cap.id());
    if (newCap) {
        m_selectedCaption = newCap;
        m_captionEditor->setCaption(newCap);
        m_captionDock->raise();
    }

    m_project->setModified(true);
    updateTitle();
    statusBar()->showMessage("Caption added", 2000);
}

void MainWindow::duplicateCaption()
{
    if (!m_selectedCaption) {
        statusBar()->showMessage("No caption selected to duplicate", 2000);
        return;
    }

    Caption dup(m_selectedCaption->text(),
                m_selectedCaption->endTime(),
                m_selectedCaption->duration());
    dup.setStyle(m_selectedCaption->style());
    dup.setPosition(m_selectedCaption->position());
    dup.setTrackIndex(m_selectedCaption->trackIndex());
    dup.setAnimationIn(m_selectedCaption->animationIn());
    dup.setAnimationOut(m_selectedCaption->animationOut());
    dup.setAnimationDuration(m_selectedCaption->animationDuration());

    m_project->timeline()->addCaption(dup);

    Caption *newCap = m_project->timeline()->captionById(dup.id());
    if (newCap) {
        m_selectedCaption = newCap;
        m_captionEditor->setCaption(newCap);
    }

    m_project->setModified(true);
    updateTitle();
    statusBar()->showMessage("Caption duplicated", 2000);
}

void MainWindow::exportProject()
{
    QString path = QFileDialog::getSaveFileName(this, "Export Project",
        m_project->name(),
        "MP4 Video (*.mp4);;AVI Video (*.avi);;All Files (*)");

    if (path.isEmpty()) return;

    ExportSettings settings;
    settings.outputPath = path;
    settings.resolution = m_project->resolution();
    settings.frameRate = m_project->frameRate();
    m_exporter->setSettings(settings);
    m_exporter->exportProject();
}

void MainWindow::exportCaptions()
{
    QStringList formats = {"SRT Subtitles (*.srt)", "WebVTT (*.vtt)", "Advanced SubStation Alpha (*.ass)"};
    QString selectedFilter;
    QString path = QFileDialog::getSaveFileName(this, "Export Captions",
        m_project->name(), formats.join(";;"), &selectedFilter);

    if (path.isEmpty()) return;

    ExportSettings settings;
    settings.outputPath = path;
    settings.exportCaptionsOnly = true;

    if (selectedFilter.contains("srt"))
        settings.captionFormat = "SRT";
    else if (selectedFilter.contains("vtt"))
        settings.captionFormat = "VTT";
    else if (selectedFilter.contains("ass"))
        settings.captionFormat = "ASS";

    m_exporter->setSettings(settings);
    m_exporter->exportCaptions();
}

void MainWindow::resetLayout()
{
    // Re-add all dock widgets
    removeDockWidget(m_mediaDock);
    removeDockWidget(m_presetsDock);
    removeDockWidget(m_captionDock);
    removeDockWidget(m_inspectorDock);
    removeDockWidget(m_brandKitDock);

    addDockWidget(Qt::LeftDockWidgetArea, m_mediaDock);
    tabifyDockWidget(m_mediaDock, m_presetsDock);
    addDockWidget(Qt::RightDockWidgetArea, m_captionDock);
    tabifyDockWidget(m_captionDock, m_inspectorDock);
    tabifyDockWidget(m_inspectorDock, m_brandKitDock);

    m_mediaDock->show();
    m_presetsDock->show();
    m_captionDock->show();
    m_inspectorDock->show();
    m_brandKitDock->show();
    m_captionDock->raise();
    m_mediaDock->raise();

    statusBar()->showMessage("Layout reset", 2000);
}

void MainWindow::onClipSelected(const QUuid &id)
{
    Clip *clip = m_project->timeline()->clipById(id);
    if (clip) {
        m_inspectorPanel->setClip(clip);
        m_inspectorDock->raise();
        m_selectedCaption = nullptr;
    }
}

void MainWindow::onCaptionSelected(const QUuid &id)
{
    Caption *cap = m_project->timeline()->captionById(id);
    if (cap) {
        m_selectedCaption = cap;
        m_captionEditor->setCaption(cap);
        m_captionDock->raise();
    }
}

void MainWindow::onAddToTimeline(const QString &path)
{
    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();

    ClipType type = ClipType::Video;
    int trackIndex = 0;

    if (ext == "mp3" || ext == "wav" || ext == "aac" || ext == "flac") {
        type = ClipType::Audio;
        trackIndex = 2; // Audio track
    } else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "bmp" || ext == "svg") {
        type = ClipType::Image;
    }

    Clip clip(type, path);
    clip.setStartTime(m_project->timeline()->currentTime());
    clip.setTrackIndex(trackIndex);
    m_project->timeline()->addClip(clip);

    m_project->setModified(true);
    updateTitle();
    statusBar()->showMessage("Added to timeline: " + fi.fileName(), 3000);
}

void MainWindow::onPresetApplied(const CaptionPreset &preset)
{
    if (!m_selectedCaption) {
        statusBar()->showMessage("Select a caption first to apply preset", 2000);
        return;
    }

    m_selectedCaption->setStyle(preset.style());
    m_selectedCaption->setAnimationIn(preset.animationIn());
    m_selectedCaption->setAnimationOut(preset.animationOut());
    m_captionEditor->setCaption(m_selectedCaption);
    m_previewPanel->update();
    m_project->setModified(true);
    updateTitle();
    statusBar()->showMessage("Preset applied: " + preset.name(), 2000);
}

void MainWindow::onPresetSaveRequested(const CaptionStyle &style, const QString &name)
{
    m_presetsPanel->addCustomPreset(style, name);
    m_presetsDock->raise();
    statusBar()->showMessage("Preset saved", 2000);
}

void MainWindow::onApplyBrandToCaption()
{
    if (!m_selectedCaption) {
        statusBar()->showMessage("Select a caption first to apply brand", 2000);
        return;
    }

    BrandKit kit = BrandKitManager::instance().activeKit();
    m_captionEditor->applyBrandKit(kit);
    m_previewPanel->update();
    statusBar()->showMessage("Brand kit applied", 2000);
}

void MainWindow::updateTitle()
{
    QString title = "CaptionStudio - " + m_project->name();
    if (m_project->isModified())
        title += " *";
    setWindowTitle(title);
}

void MainWindow::addSampleContent()
{
    // Add sample captions to showcase the app
    Caption cap1("Welcome to CaptionStudio!", 0, 4000);
    cap1.setTrackIndex(1);
    CaptionStyle style1;
    style1.font = QFont("Arial", 40, QFont::Bold);
    style1.textColor = QColor(255, 255, 255);
    style1.outlineColor = QColor(0, 0, 0);
    style1.outlineWidth = 3.0;
    style1.showBackground = false;
    cap1.setStyle(style1);
    cap1.setPosition(QPointF(0.5, 0.4));

    Caption cap2("Create stunning captions with presets", 4500, 3500);
    cap2.setTrackIndex(1);
    CaptionStyle style2;
    style2.font = QFont("Arial", 32, QFont::Bold);
    style2.textColor = QColor(0, 255, 200);
    style2.outlineWidth = 2.0;
    style2.shadowColor = QColor(0, 255, 200, 80);
    style2.shadowBlur = 8;
    style2.shadowOffsetX = 0;
    style2.shadowOffsetY = 0;
    style2.showBackground = false;
    cap2.setStyle(style2);

    Caption cap3("Brand your content with your own style", 9000, 3000);
    cap3.setTrackIndex(1);
    CaptionStyle style3;
    style3.font = QFont("Georgia", 28, QFont::Normal, true);
    style3.textColor = QColor(240, 220, 180);
    style3.showBackground = true;
    style3.backgroundColor = QColor(20, 20, 20, 200);
    style3.backgroundPadding = 10;
    style3.backgroundRadius = 6;
    style3.outlineWidth = 0;
    style3.italic = true;
    cap3.setStyle(style3);

    m_project->timeline()->addCaption(cap1);
    m_project->timeline()->addCaption(cap2);
    m_project->timeline()->addCaption(cap3);

    // Add a sample video clip placeholder
    Clip videoClip(ClipType::Video);
    videoClip.setName("Sample Video");
    videoClip.setStartTime(0);
    videoClip.setDuration(15000);
    videoClip.setTrackIndex(0);
    m_project->timeline()->addClip(videoClip);
}
