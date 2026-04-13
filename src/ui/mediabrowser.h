#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include "core/project.h"

class MediaBrowser : public QWidget {
    Q_OBJECT

public:
    explicit MediaBrowser(QWidget *parent = nullptr);

    void setProject(Project *project);

signals:
    void mediaSelected(const QString &path);
    void mediaDoubleClicked(const QString &path);
    void addToTimeline(const QString &path);

public slots:
    void importMedia();
    void removeSelected();
    void refreshList();

private:
    void updateList();
    QString fileIcon(const QString &path) const;
    QString fileSize(const QString &path) const;

    Project *m_project = nullptr;
    QListWidget *m_listWidget;
    QPushButton *m_importBtn;
    QPushButton *m_removeBtn;
    QPushButton *m_addToTimelineBtn;
    QLabel *m_infoLabel;
};
