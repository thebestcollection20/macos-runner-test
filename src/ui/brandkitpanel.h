#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include "core/brandkit.h"

class BrandKitPanel : public QWidget {
    Q_OBJECT

public:
    explicit BrandKitPanel(QWidget *parent = nullptr);

    void refreshKitList();

signals:
    void brandKitSelected(const BrandKit &kit);
    void applyBrandToCaption();

private slots:
    void onKitSelected(int index);
    void onNewKit();
    void onDeleteKit();
    void onAddColor();
    void onAddFont();
    void onAddLogo();
    void onSaveKit();

private:
    void updateKitDisplay();
    QWidget *createColorSwatch(const BrandColor &bc, int index);
    QWidget *createFontItem(const BrandFont &bf, int index);

    QComboBox *m_kitSelector;
    QPushButton *m_newKitBtn;
    QPushButton *m_deleteKitBtn;
    QLineEdit *m_kitNameEdit;

    // Colors section
    QWidget *m_colorsContainer;
    QVBoxLayout *m_colorsLayout;
    QPushButton *m_addColorBtn;

    // Fonts section
    QWidget *m_fontsContainer;
    QVBoxLayout *m_fontsLayout;
    QPushButton *m_addFontBtn;

    // Logos section
    QListWidget *m_logosList;
    QPushButton *m_addLogoBtn;

    // Apply
    QPushButton *m_applyBtn;
    QPushButton *m_saveBtn;
};
