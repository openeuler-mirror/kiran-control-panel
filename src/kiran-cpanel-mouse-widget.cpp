#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"
#include "widgets/kiran-list-item.h"

#include <kiranwidgets-qt5/kiran-switch-button.h>

enum Items
{
    ITEM_MOUSE,
    ITEM_TOUCHPAD,
    ITEM_LAST
};
enum Pages
{
    PAGE_MOUSE,
    PAGE_TOUCHPAD,
    PAGE_LAST
};

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
    initUI();
    addComboBoxItem();
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(setCurrentPageWhenItemClicked(QListWidgetItem*)));
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

    ui->listWidget->resize(sizeHint());

    m_mouseListItem = createListItem(tr("Mouse Settings"));
    m_mouseListItem->setObjectName("mouseListItem");
    m_touchpadListItem = createListItem(tr("TouchPad Settings"));
    m_touchpadListItem->setObjectName("touchpadListItem");

    ui->listWidget->setCurrentRow(ITEM_MOUSE);
    ui->stackedWidget->setCurrentIndex(PAGE_MOUSE);
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

void KiranCPanelMouseWidget::addComboBoxItem()
{
    QStringList hand_mode;
    hand_mode << tr("Right Mode") << tr("Left Mode") ;
    ui->comboBox_hand_mode->addItems(hand_mode);
    ui->comboBox_tp_hand_mode->addItems(hand_mode);

    QStringList tpClickMode;
    tpClickMode << tr("mode 0") << tr("mode 2") << tr("mode 3");
    ui->comboBox_tp_click_mode->addItems(tpClickMode);

    QStringList tpScrollWinMode;
    tpScrollWinMode << tr("One Finger") <<tr("Two Finger") << tr("Three Finger");
    ui->comboBox_tp_move_win_mode->addItems(tpScrollWinMode);
}

void KiranCPanelMouseWidget::setCurrentPageWhenItemClicked(QListWidgetItem *item)
{
    int itemNum = ui->listWidget->row(item);
    if(itemNum == ITEM_MOUSE)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_mouse);
        m_mouseListItem->setItemArrow(true);
        m_touchpadListItem->setItemArrow(false);
    }
    else if(itemNum == ITEM_TOUCHPAD)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_touchpad);
        m_touchpadListItem->setItemArrow(true);
        m_mouseListItem->setItemArrow(false);
    }
}
