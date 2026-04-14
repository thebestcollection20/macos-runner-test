#include "brandkitpanel.h"
#include "captioneditor.h"
#include <QScrollArea>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>

BrandKitPanel::BrandKitPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");

    auto *content = new QWidget;
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel("BRAND KIT");
    titleLabel->setObjectName("sectionLabel");
    layout->addWidget(titleLabel);

    // Kit selector
    auto *selectorGroup = new QGroupBox("Brand");
    auto *selectorLayout = new QVBoxLayout(selectorGroup);

    auto *selectorRow = new QHBoxLayout;
    m_kitSelector = new QComboBox;
    selectorRow->addWidget(m_kitSelector, 1);

    m_newKitBtn = new QPushButton("+");
    m_newKitBtn->setFixedSize(24, 24);
    m_newKitBtn->setToolTip("New Brand Kit");
    selectorRow->addWidget(m_newKitBtn);

    m_deleteKitBtn = new QPushButton("x");
    m_deleteKitBtn->setFixedSize(24, 24);
    m_deleteKitBtn->setToolTip("Delete Brand Kit");
    selectorRow->addWidget(m_deleteKitBtn);

    selectorLayout->addLayout(selectorRow);

    m_kitNameEdit = new QLineEdit;
    m_kitNameEdit->setPlaceholderText("Brand name...");
    selectorLayout->addWidget(m_kitNameEdit);

    layout->addWidget(selectorGroup);

    // Colors
    auto *colorsGroup = new QGroupBox("Brand Colors");
    auto *colorsOuterLayout = new QVBoxLayout(colorsGroup);

    m_colorsContainer = new QWidget;
    m_colorsLayout = new QVBoxLayout(m_colorsContainer);
    m_colorsLayout->setContentsMargins(0, 0, 0, 0);
    m_colorsLayout->setSpacing(4);
    colorsOuterLayout->addWidget(m_colorsContainer);

    m_addColorBtn = new QPushButton("+ Add Color");
    m_addColorBtn->setFixedHeight(24);
    colorsOuterLayout->addWidget(m_addColorBtn);

    layout->addWidget(colorsGroup);

    // Fonts
    auto *fontsGroup = new QGroupBox("Brand Fonts");
    auto *fontsOuterLayout = new QVBoxLayout(fontsGroup);

    m_fontsContainer = new QWidget;
    m_fontsLayout = new QVBoxLayout(m_fontsContainer);
    m_fontsLayout->setContentsMargins(0, 0, 0, 0);
    m_fontsLayout->setSpacing(4);
    fontsOuterLayout->addWidget(m_fontsContainer);

    m_addFontBtn = new QPushButton("+ Add Font");
    m_addFontBtn->setFixedHeight(24);
    fontsOuterLayout->addWidget(m_addFontBtn);

    layout->addWidget(fontsGroup);

    // Logos
    auto *logosGroup = new QGroupBox("Logos & Watermarks");
    auto *logosLayout = new QVBoxLayout(logosGroup);

    m_logosList = new QListWidget;
    m_logosList->setMaximumHeight(80);
    logosLayout->addWidget(m_logosList);

    m_addLogoBtn = new QPushButton("+ Add Logo");
    m_addLogoBtn->setFixedHeight(24);
    logosLayout->addWidget(m_addLogoBtn);

    layout->addWidget(logosGroup);

    // Action buttons
    m_applyBtn = new QPushButton("Apply to Caption");
    m_applyBtn->setObjectName("accentButton");
    m_applyBtn->setFixedHeight(28);
    layout->addWidget(m_applyBtn);

    m_saveBtn = new QPushButton("Save Brand Kit");
    m_saveBtn->setFixedHeight(28);
    layout->addWidget(m_saveBtn);

    layout->addStretch();

    scrollArea->setWidget(content);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);

    // Connections
    connect(m_kitSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BrandKitPanel::onKitSelected);
    connect(m_newKitBtn, &QPushButton::clicked, this, &BrandKitPanel::onNewKit);
    connect(m_deleteKitBtn, &QPushButton::clicked, this, &BrandKitPanel::onDeleteKit);
    connect(m_addColorBtn, &QPushButton::clicked, this, &BrandKitPanel::onAddColor);
    connect(m_addFontBtn, &QPushButton::clicked, this, &BrandKitPanel::onAddFont);
    connect(m_addLogoBtn, &QPushButton::clicked, this, &BrandKitPanel::onAddLogo);
    connect(m_saveBtn, &QPushButton::clicked, this, &BrandKitPanel::onSaveKit);
    connect(m_applyBtn, &QPushButton::clicked, this, &BrandKitPanel::applyBrandToCaption);

    refreshKitList();
}

void BrandKitPanel::refreshKitList()
{
    m_kitSelector->blockSignals(true);
    m_kitSelector->clear();

    auto &mgr = BrandKitManager::instance();
    for (const auto &kit : mgr.kits()) {
        m_kitSelector->addItem(kit.name(), kit.id().toString());
    }

    // Select active kit
    BrandKit active = mgr.activeKit();
    for (int i = 0; i < m_kitSelector->count(); ++i) {
        if (m_kitSelector->itemData(i).toString() == active.id().toString()) {
            m_kitSelector->setCurrentIndex(i);
            break;
        }
    }

    m_kitSelector->blockSignals(false);
    updateKitDisplay();
}

void BrandKitPanel::onKitSelected(int index)
{
    if (index < 0) return;
    QString idStr = m_kitSelector->itemData(index).toString();
    QUuid id = QUuid::fromString(idStr);
    BrandKitManager::instance().setActiveKit(id);
    updateKitDisplay();
    emit brandKitSelected(BrandKitManager::instance().activeKit());
}

void BrandKitPanel::onNewKit()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Brand Kit", "Brand name:",
                                          QLineEdit::Normal, "My Brand", &ok);
    if (ok && !name.isEmpty()) {
        BrandKit kit(name);
        kit.addColor({"Primary", QColor(66, 133, 244), true});
        kit.addFont({"Heading", "Arial", 32, true});
        BrandKitManager::instance().addKit(kit);
        BrandKitManager::instance().setActiveKit(kit.id());
        refreshKitList();
    }
}

void BrandKitPanel::onDeleteKit()
{
    auto &mgr = BrandKitManager::instance();
    if (mgr.kits().size() <= 1) return; // Keep at least one

    BrandKit active = mgr.activeKit();
    mgr.removeKit(active.id());
    if (!mgr.kits().isEmpty())
        mgr.setActiveKit(mgr.kits().first().id());
    refreshKitList();
}

void BrandKitPanel::onAddColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose Brand Color");
    if (!color.isValid()) return;

    bool ok;
    QString name = QInputDialog::getText(this, "Color Name", "Name:",
                                          QLineEdit::Normal, "New Color", &ok);
    if (!ok) return;

    auto &mgr = BrandKitManager::instance();
    BrandKit kit = mgr.activeKit();
    kit.addColor({name, color, false});
    mgr.updateKit(kit);
    updateKitDisplay();
}

void BrandKitPanel::onAddFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Arial", 24), this, "Choose Brand Font");
    if (!ok) return;

    QString name = QInputDialog::getText(this, "Font Name", "Name:",
                                          QLineEdit::Normal, "New Font", &ok);
    if (!ok) return;

    auto &mgr = BrandKitManager::instance();
    BrandKit kit = mgr.activeKit();
    kit.addFont({name, font.family(), font.pointSize(), false});
    mgr.updateKit(kit);
    updateKitDisplay();
}

void BrandKitPanel::onAddLogo()
{
    QString path = QFileDialog::getOpenFileName(this, "Add Logo",
                                                 QString(), "Images (*.png *.jpg *.svg)");
    if (path.isEmpty()) return;

    auto &mgr = BrandKitManager::instance();
    BrandKit kit = mgr.activeKit();
    kit.addLogo(path);
    mgr.updateKit(kit);
    updateKitDisplay();
}

void BrandKitPanel::onSaveKit()
{
    auto &mgr = BrandKitManager::instance();
    BrandKit kit = mgr.activeKit();
    kit.setName(m_kitNameEdit->text());
    mgr.updateKit(kit);
    refreshKitList();
}

void BrandKitPanel::updateKitDisplay()
{
    auto &mgr = BrandKitManager::instance();
    BrandKit kit = mgr.activeKit();

    m_kitNameEdit->setText(kit.name());

    // Clear colors
    QLayoutItem *item;
    while ((item = m_colorsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < kit.colors().size(); ++i) {
        m_colorsLayout->addWidget(createColorSwatch(kit.colors()[i], i));
    }

    // Clear fonts
    while ((item = m_fontsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < kit.fonts().size(); ++i) {
        m_fontsLayout->addWidget(createFontItem(kit.fonts()[i], i));
    }

    // Logos
    m_logosList->clear();
    for (const auto &path : kit.logoPaths()) {
        QFileInfo fi(path);
        m_logosList->addItem(fi.fileName());
    }
}

QWidget *BrandKitPanel::createColorSwatch(const BrandColor &bc, int index)
{
    auto *widget = new QWidget;
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto *colorBtn = new ColorButton(bc.color);
    auto *nameLabel = new QLabel(bc.name);
    nameLabel->setStyleSheet("font-size: 10px;");
    auto *hexLabel = new QLabel(bc.color.name().toUpper());
    hexLabel->setStyleSheet("color: #696969; font-size: 9px; font-family: 'Courier New';");

    auto *primaryLabel = new QLabel(bc.isPrimary ? "[P]" : "");
    primaryLabel->setStyleSheet("color: #488ce6; font-size: 9px;");

    auto *removeBtn = new QPushButton("x");
    removeBtn->setFixedSize(18, 18);

    layout->addWidget(colorBtn);
    layout->addWidget(nameLabel);
    layout->addWidget(hexLabel);
    layout->addWidget(primaryLabel);
    layout->addStretch();
    layout->addWidget(removeBtn);

    connect(removeBtn, &QPushButton::clicked, [this, index]() {
        auto &mgr = BrandKitManager::instance();
        BrandKit kit = mgr.activeKit();
        kit.removeColor(index);
        mgr.updateKit(kit);
        updateKitDisplay();
    });

    return widget;
}

QWidget *BrandKitPanel::createFontItem(const BrandFont &bf, int index)
{
    auto *widget = new QWidget;
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto *nameLabel = new QLabel(bf.name);
    nameLabel->setStyleSheet("font-size: 10px; font-weight: bold;");
    auto *familyLabel = new QLabel(bf.fontFamily);
    familyLabel->setStyleSheet(QString("color: #969696; font-size: 10px; font-family: '%1';").arg(bf.fontFamily));
    auto *sizeLabel = new QLabel(QString("%1pt").arg(bf.defaultSize));
    sizeLabel->setStyleSheet("color: #696969; font-size: 9px;");

    auto *primaryLabel = new QLabel(bf.isPrimary ? "[P]" : "");
    primaryLabel->setStyleSheet("color: #488ce6; font-size: 9px;");

    auto *removeBtn = new QPushButton("x");
    removeBtn->setFixedSize(18, 18);

    layout->addWidget(nameLabel);
    layout->addWidget(familyLabel);
    layout->addWidget(sizeLabel);
    layout->addWidget(primaryLabel);
    layout->addStretch();
    layout->addWidget(removeBtn);

    connect(removeBtn, &QPushButton::clicked, [this, index]() {
        auto &mgr = BrandKitManager::instance();
        BrandKit kit = mgr.activeKit();
        kit.removeFont(index);
        mgr.updateKit(kit);
        updateKitDisplay();
    });

    return widget;
}
