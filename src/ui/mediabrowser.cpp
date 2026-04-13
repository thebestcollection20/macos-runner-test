#include "mediabrowser.h"
#include <QFileDialog>
#include <QMessageBox>

MediaBrowser::MediaBrowser(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // Header
    auto *headerLayout = new QHBoxLayout;
    auto *title = new QLabel("MEDIA POOL");
    title->setObjectName("sectionLabel");

    m_importBtn = new QPushButton("Import");
    m_importBtn->setFixedHeight(22);
    m_importBtn->setObjectName("accentButton");

    m_removeBtn = new QPushButton("Remove");
    m_removeBtn->setFixedHeight(22);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(m_importBtn);
    headerLayout->addWidget(m_removeBtn);

    layout->addLayout(headerLayout);

    // List
    m_listWidget = new QListWidget;
    m_listWidget->setIconSize(QSize(32, 32));
    m_listWidget->setSpacing(2);
    layout->addWidget(m_listWidget, 1);

    // Add to timeline
    m_addToTimelineBtn = new QPushButton("Add to Timeline");
    m_addToTimelineBtn->setFixedHeight(26);
    layout->addWidget(m_addToTimelineBtn);

    // Info
    m_infoLabel = new QLabel("No media imported");
    m_infoLabel->setStyleSheet("color: #696969; font-size: 10px;");
    layout->addWidget(m_infoLabel);

    // Connections
    connect(m_importBtn, &QPushButton::clicked, this, &MediaBrowser::importMedia);
    connect(m_removeBtn, &QPushButton::clicked, this, &MediaBrowser::removeSelected);
    connect(m_addToTimelineBtn, &QPushButton::clicked, [this]() {
        auto *item = m_listWidget->currentItem();
        if (item)
            emit addToTimeline(item->data(Qt::UserRole).toString());
    });
    connect(m_listWidget, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        emit mediaSelected(item->data(Qt::UserRole).toString());
        m_infoLabel->setText(fileSize(item->data(Qt::UserRole).toString()));
    });
    connect(m_listWidget, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item) {
        emit mediaDoubleClicked(item->data(Qt::UserRole).toString());
    });
}

void MediaBrowser::setProject(Project *project)
{
    m_project = project;
    updateList();

    if (m_project) {
        connect(m_project, &Project::mediaFileAdded, [this]() { updateList(); });
        connect(m_project, &Project::mediaFileRemoved, [this]() { updateList(); });
    }
}

void MediaBrowser::importMedia()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this, "Import Media",
        QString(),
        "Media Files (*.mp4 *.avi *.mov *.mkv *.webm *.mp3 *.wav *.aac *.png *.jpg *.jpeg *.gif *.bmp *.svg);;All Files (*)");

    if (m_project) {
        for (const auto &file : files)
            m_project->addMediaFile(file);
    }
}

void MediaBrowser::removeSelected()
{
    auto *item = m_listWidget->currentItem();
    if (item && m_project) {
        m_project->removeMediaFile(item->data(Qt::UserRole).toString());
    }
}

void MediaBrowser::refreshList()
{
    updateList();
}

void MediaBrowser::updateList()
{
    m_listWidget->clear();
    if (!m_project)
        return;

    for (const auto &path : m_project->mediaFiles()) {
        QFileInfo fi(path);
        auto *item = new QListWidgetItem(fileIcon(path) + " " + fi.fileName());
        item->setData(Qt::UserRole, path);
        item->setToolTip(path);
        m_listWidget->addItem(item);
    }

    int count = m_project->mediaFiles().size();
    m_infoLabel->setText(count > 0 ? QString("%1 file(s)").arg(count) : "No media imported");
}

QString MediaBrowser::fileIcon(const QString &path) const
{
    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();
    if (ext == "mp4" || ext == "avi" || ext == "mov" || ext == "mkv" || ext == "webm")
        return "[VID]";
    if (ext == "mp3" || ext == "wav" || ext == "aac" || ext == "flac")
        return "[AUD]";
    if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "bmp" || ext == "svg")
        return "[IMG]";
    return "[FILE]";
}

QString MediaBrowser::fileSize(const QString &path) const
{
    QFileInfo fi(path);
    qint64 size = fi.size();
    if (size < 1024)
        return QString("%1 B").arg(size);
    if (size < 1024 * 1024)
        return QString("%1 KB").arg(size / 1024);
    return QString("%1 MB").arg(size / (1024 * 1024));
}
