#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"
#include "widgets/kiran-list-item.h"

#include <kiranwidgets-qt5/kiran-switch-button.h>

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
    initUI();
}

KiranCPanelMouseWidget::~KiranCPanelMouseWidget()
{
    delete ui;
}

void KiranCPanelMouseWidget::initUI()
{
    m_naturalScrollSwitch = new KiranSwitchButton(this);
    m_middleEmulationSwitch = new KiranSwitchButton(this);
    ui->hLayout_natural_scroll->addWidget(m_naturalScrollSwitch);
    ui->hLayout_middle_emulation->addWidget(m_middleEmulationSwitch);

    m_mouseListItem = createListItem(tr("Mouse Settings"));
    m_touchpadListItem = createListItem(tr("TouchPad Settings"));
}

KiranListItem* KiranCPanelMouseWidget::createListItem(QString text)
{
    QListWidgetItem* newItem = nullptr;
    KiranListItem* customItem = nullptr;

    newItem = new QListWidgetItem(ui->listWidget);
    customItem = new KiranListItem(ui->listWidget);

    newItem->setSizeHint(QSize(246,60));
    newItem->setTextAlignment(Qt::AlignVCenter);

    customItem->setText(text);

    ui->listWidget->addItem(newItem);
    ui->listWidget->setItemWidget(newItem , customItem);

    ui->listWidget->setGridSize(QSize(246,84));
    return customItem;
}
