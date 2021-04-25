#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"
#include "widgets/kiran-list-item.h"

#include <kiranwidgets-qt5/kiran-switch-button.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <iostream>

#define SLIDER_MINIMUM  0
#define SLIDER_MAXIMUN  99
using namespace std;
using namespace Kiran::WidgetPropertyHelper ;
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
enum ScrollSpeed
{
    SCROLL_SLOW,
    SCROLL_STANDARD,
    SCROLL_FAST,
    SCROLL_LAST
};

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
    initUI();

    connect(ui->btn_cancel,&QPushButton::clicked,this,&QApplication::quit);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(setCurrentPageWhenItemClicked(QListWidgetItem*)));
    connect(ui->slider_speed, &QSlider::sliderReleased, this, &KiranCPanelMouseWidget::onSliderReleased);
    connect(ui->slider_tp_speed, &QSlider::sliderReleased, this, &KiranCPanelMouseWidget::onSliderReleased);
    connect(ui->checkBox_tp_disable_touchpad, &QCheckBox::toggled, this,
            &KiranCPanelMouseWidget::onDisabelTouchPadToggled);
}

KiranCPanelMouseWidget::~KiranCPanelMouseWidget()
{
    delete ui;
}

void KiranCPanelMouseWidget::initUI()
{
    setButtonType(ui->btn_save,Kiran::BUTTON_Default);

    ui->listWidget->resize(sizeHint());
    ui->listWidget->setIconSize(QSize(16,16));

    addSidebarItem(tr("Mouse Settings"),":/images/mouse.svg");
    addSidebarItem(tr("TouchPad Settings"),":/images/touchpad.svg");
    addComboBoxItem();

    ui->listWidget->setCurrentRow(ITEM_MOUSE);
    ui->stackedWidget->setCurrentIndex(PAGE_MOUSE);

    ui->label_speed->setText(tr("Standard"));
    ui->label_tp_speed->setText(tr("Standard"));

    ui->checkBox_tp_disable_touchpad->setChecked(false);

    QList<QSlider*> sliderList = this->findChildren< QSlider* >();
    foreach (QSlider* slider, sliderList)
    {
       slider->setMaximum(SLIDER_MAXIMUN);
       slider->setMinimum(SLIDER_MINIMUM);
       slider->setPageStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
       slider->setSingleStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
    }
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
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode") ;
    ui->comboBox_hand_mode->addItems(hand_mode);
    ui->comboBox_tp_hand_mode->addItems(hand_mode);

    QStringList tpClickMode;
    tpClickMode << tr("mode 0") << tr("mode 2") << tr("mode 3");
    ui->comboBox_tp_click_mode->addItems(tpClickMode);

    QStringList tpScrollWinMode;
    tpScrollWinMode << tr("Two Finger Scroll") <<tr("Edge Scroll");
    ui->comboBox_tp_move_win_mode->addItems(tpScrollWinMode);
}

void KiranCPanelMouseWidget::addSidebarItem(QString text, QString icon)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setIcon(QIcon(icon));
    item->setText(text);
    ui->listWidget->addItem(item);
}

void KiranCPanelMouseWidget::setCurrentPageWhenItemClicked(QListWidgetItem *item)
{
    int itemNum = ui->listWidget->row(item);
    if(itemNum == ITEM_MOUSE)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_mouse);
    }
    else if(itemNum == ITEM_TOUCHPAD)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_touchpad);
    }
}

void KiranCPanelMouseWidget::onSliderReleased()
{
    QSlider *senderSlider = static_cast<QSlider *>(sender());
    QLabel *labelSpeed = new QLabel(this);
    bool ismouseSlider = false;
    int scrollSpeed;

    if(senderSlider == ui->slider_speed)
    {
        labelSpeed = static_cast<QLabel*>(ui->label_speed);
        ismouseSlider = true;
    }
    else if(senderSlider == ui->slider_tp_speed)
    {
        labelSpeed = static_cast<QLabel*>(ui->label_tp_speed);
        ismouseSlider = false;
    }

    int lowMiddleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/4;
    int middleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2;
    int highMiddleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/4*3;

    int value = senderSlider->value();
    if(value >= SLIDER_MINIMUM && value < lowMiddleNum)
    {
        senderSlider->setValue(SLIDER_MINIMUM);
        labelSpeed->setText(tr("Slow"));
        scrollSpeed = SCROLL_SLOW;
    }
    else if(value >= lowMiddleNum && value <= middleNum)
    {
        senderSlider->setValue(middleNum);
        labelSpeed->setText(tr("Standard"));
        scrollSpeed = SCROLL_STANDARD;
    }
    else if(value >middleNum && value<= highMiddleNum)
    {
        senderSlider->setValue(middleNum);
        labelSpeed->setText(tr("Standard"));
        scrollSpeed = SCROLL_STANDARD;
    }
    else
    {
        senderSlider->setValue(SLIDER_MAXIMUN);
        labelSpeed->setText(tr("Fast"));
        scrollSpeed = SCROLL_FAST;
    }
    if(ismouseSlider)
    {
        /*TODO:use dbus set mouse slider*/
        ///m_mouseScrollSpeed =scrollSpeed;
    }
    else
    {
        /*TODO:use dbus set tpuchpad slider*/
        ///m_tpScrollSpeed =scrollSpeed;
    }
}

void KiranCPanelMouseWidget::onDisabelTouchPadToggled(bool disabled)
{
    m_comboBoxList = ui->page_touchpad->findChildren<QComboBox *>();
    m_checkBoxList = {ui->checkBox_tap_to_click,
                      ui->checkBox_tp_natural_scroll,
                      ui->checkBox_disable_while_typing};

    m_labelList = ui->page_touchpad->findChildren<QLabel *>();
    if(disabled)
    {
        setDisableWidget(true);
    }
    else
    {
        setDisableWidget(false);
    }
}

void KiranCPanelMouseWidget::setDisableWidget(bool disabled)
{
    foreach (QComboBox *comboBox, m_comboBoxList)
    {
        comboBox->setDisabled(disabled);
    }
    foreach (KiranSwitchButton* checkBox, m_checkBoxList)
    {
        checkBox->setDisabled(disabled);
    }
//    foreach (QLabel* label, m_labelList)
//    {
//        if(label == ui->label_tp_disable_touchpad)
//            continue;
//        label->setDisabled(disabled);
//    }
    ui->slider_tp_speed->setDisabled(disabled);
    ui->label_tp_speed->setDisabled(disabled);
}

