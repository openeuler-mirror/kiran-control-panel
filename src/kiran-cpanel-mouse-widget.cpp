#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"
#include "widgets/kiran-list-item.h"
#include <dbus-interface/mouse-interface.h>
#include <dbus-interface/touchpad-interface.h>

#include <kiranwidgets-qt5/kiran-switch-button.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <iostream>

#define SLIDER_MINIMUM  0
#define SLIDER_MAXIMUN  99
#define SLOW            "Slow"
#define STANDARD        "Standard"
#define FAST            "Fast"
#define DBUS_MOUSE_NANE       "com.kylinsec.Kiran.SessionDaemon.Mouse"
#define DBUS_MOUSE_PATH       "/com/kylinsec/Kiran/SessionDaemon/Mouse"
#define DBUS_TOUCHPAD_NAME    "com.kylinsec.Kiran.SessionDaemon.TouchPad"
#define DBUS_TOUCHPAD_PATH    "/com/kylinsec/Kiran/SessionDaemon/TouchPad"

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

enum MOTION_ACCELERATION
{
    MOTION_SLOW = -1,
    MOTION_STANDARD,
    MOTION_FAST,
    MOTION_LAST
};
enum HandMode
{
    RIGHT_HAND, //false:0
    LEFT_HAND, //true:1
    HAND_LAST
};
enum CLickMode
{
    MODE_PRESS_AND_TAP,  //false:0
    MODE_TAP,            //true:1
    MODE_LAST
};
enum ScrollMothod
{
    SCROLL_TWO_FINGER,
    SCROLL_EDGE,
    /*暂时不设置该模式
    SCROLL_BUTTON,*/
    SCROLL_LAST
};

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(setCurrentPageWhenItemClicked(QListWidgetItem*)));
}

KiranCPanelMouseWidget::~KiranCPanelMouseWidget()
{
    delete ui;
}

bool KiranCPanelMouseWidget::connectDbus()
{
    m_mouseInterface = ComKylinsecKiranSessionDaemonMouseInterface::instance();
    m_touchPadInterface = ComKylinsecKiranSessionDaemonTouchPadInterface::instance();
    if(!m_mouseInterface || !m_touchPadInterface)
    {
        return false;
    }
    return  true;
}

bool KiranCPanelMouseWidget::initUI()
{
    bool isGetDbusConnect = connectDbus();
    if(!isGetDbusConnect)
    {
        return false;
    }

    m_comboBoxList = ui->page_touchpad->findChildren<QComboBox *>();
    m_checkBoxList = {ui->checkBox_tap_to_click,
                      ui->checkBox_tp_natural_scroll,
                      ui->checkBox_disable_while_typing};

    m_labelList = ui->page_touchpad->findChildren<QLabel *>();

    ui->listWidget->resize(sizeHint());
    ui->listWidget->setIconSize(QSize(16,16));

    addSidebarItem(tr("Mouse Settings"),":/images/mouse.svg");
    addSidebarItem(tr("TouchPad Settings"),":/images/touchpad.svg");
    ui->listWidget->setCurrentRow(ITEM_MOUSE);

    addComboBoxItem();

    ui->stackedWidget->setCurrentIndex(PAGE_MOUSE);

    ui->widget_tp_click_mode->hide();

    QList<QSlider*> sliderList = this->findChildren< QSlider* >();
    foreach (QSlider* slider, sliderList)
    {
       slider->setMaximum(SLIDER_MAXIMUN);
       slider->setMinimum(SLIDER_MINIMUM);
       slider->setPageStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
       slider->setSingleStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
    }

    initPageMouseUI();
    initPageTouchPadUI();
    return true;
}

void KiranCPanelMouseWidget::initPageMouseUI()
{
    m_mouseLeftHand = m_mouseInterface->left_handed();
    ui->comboBox_hand_mode->setCurrentIndex(m_mouseLeftHand);
    connect(ui->comboBox_hand_mode, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [this](int currentIntex){
        m_mouseLeftHand = currentIntex;
        m_mouseInterface->setLeft_handed(m_mouseLeftHand);
    });

    m_mouseMotionAcceleration = m_mouseInterface->motion_acceleration();
    if(m_mouseMotionAcceleration == MOTION_SLOW)
    {
        ui->slider_speed->setValue(SLIDER_MINIMUM);
        ui->label_speed->setText(SLOW);
    }
    else if(m_mouseMotionAcceleration == MOTION_STANDARD)
    {
        ui->slider_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
        ui->label_speed->setText(STANDARD);
    }
    else
    {
        ui->slider_speed->setValue(SLIDER_MAXIMUN);
        ui->label_speed->setText(FAST);
    }
    //connect(ui->slider_speed, &QSlider::sliderReleased, this, &KiranCPanelMouseWidget::onSliderReleased);
    connect(ui->slider_speed, &QSlider::valueChanged, this,&KiranCPanelMouseWidget::onSliderReleased);

    m_mouseNaturalScroll = m_mouseInterface->natural_scroll();
    ui->checkBox_natural_scroll->setChecked(m_mouseNaturalScroll);
    connect(ui->checkBox_natural_scroll,&KiranSwitchButton::toggled,
            [this](bool ischecked){
       m_mouseNaturalScroll = ischecked;
       m_mouseInterface->setNatural_scroll(ischecked);
    });

    m_middleEmulationEnabled = m_mouseInterface->middle_emulation_enabled();
    ui->checkBox_middle_emulation->setChecked(m_middleEmulationEnabled);
    connect(ui->checkBox_middle_emulation,&KiranSwitchButton::toggled,
            [this](bool ischecked){
       m_middleEmulationEnabled = ischecked;
       m_mouseInterface->setMiddle_emulation_enabled(ischecked);
    });
}

void KiranCPanelMouseWidget::initPageTouchPadUI()
{
    m_touchPadEnabled = m_touchPadInterface->touchpad_enabled();
    ui->checkBox_tp_disable_touchpad->setChecked(!m_touchPadEnabled);
    if(!m_touchPadEnabled)
    {
        setDisableWidget(true);
    }
    connect(ui->checkBox_tp_disable_touchpad, &QCheckBox::toggled, this,
            &KiranCPanelMouseWidget::onDisabelTouchPadToggled);

    m_touchPadLeftHand = m_touchPadInterface->left_handed();
    ui->comboBox_tp_hand_mode->setCurrentIndex(m_touchPadLeftHand);
    connect(ui->comboBox_tp_hand_mode, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [this](int currentIntex){
        m_touchPadLeftHand = currentIntex;
        m_touchPadInterface->setLeft_handed(m_touchPadLeftHand);
    });

    m_touchPadMotionAcceleration = m_touchPadInterface->motion_acceleration();
    if(m_touchPadMotionAcceleration == MOTION_SLOW)
    {
        ui->slider_tp_speed->setValue(SLIDER_MINIMUM);
        ui->label_tp_speed->setText(SLOW);
    }
    else if(m_touchPadMotionAcceleration == MOTION_STANDARD)
    {
        ui->slider_tp_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
        ui->label_tp_speed->setText(STANDARD);
    }
    else
    {
        ui->slider_tp_speed->setValue(MOTION_FAST);
        ui->label_tp_speed->setText(FAST);
    }
    connect(ui->slider_tp_speed, &QSlider::sliderReleased, this, &KiranCPanelMouseWidget::onSliderReleased);

    m_clickMethod = m_touchPadInterface->click_method();
    ui->comboBox_tp_click_mode->setCurrentIndex(m_clickMethod);
    connect(ui->comboBox_tp_click_mode, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [this](int currentIndex){
       m_clickMethod = currentIndex;
       m_touchPadInterface->setClick_method(m_clickMethod);
    });

    m_scrollMethod = m_touchPadInterface->scroll_method();
    ui->comboBox_tp_move_win_mode ->setCurrentIndex(m_scrollMethod);
    connect(ui->comboBox_tp_move_win_mode,static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
            [this](int currentIndex){
       m_scrollMethod = currentIndex;
       m_touchPadInterface->setScroll_method(m_scrollMethod);
    });

    m_touchPadNaturalScroll = m_touchPadInterface->natural_scroll();
    ui->checkBox_tp_natural_scroll->setChecked(m_touchPadNaturalScroll);
    connect(ui->checkBox_tp_natural_scroll,&KiranSwitchButton::toggled,
            [this](bool isNaturalScroll){
       m_touchPadNaturalScroll = isNaturalScroll;
       m_touchPadInterface->setNatural_scroll(m_touchPadNaturalScroll);
    });

    m_disabelWhileTyping = m_touchPadInterface->disable_while_typing();
    ui->checkBox_disable_while_typing->setChecked(m_disabelWhileTyping);
    connect(ui->checkBox_disable_while_typing,&KiranSwitchButton::toggled,
            [this](bool disabelWhileTyping){
       m_disabelWhileTyping = disabelWhileTyping;
       m_touchPadInterface->setDisable_while_typing(m_disabelWhileTyping);
    });

    m_tapToClick = m_touchPadInterface->tap_to_click();
    ui->checkBox_tap_to_click->setChecked(m_tapToClick);
    connect(ui->checkBox_tap_to_click,&KiranSwitchButton::toggled,
            [this](bool isTapToClick){
       m_tapToClick  = isTapToClick;
       m_touchPadInterface->setTap_to_click(m_tapToClick);
    });
}

void KiranCPanelMouseWidget::updatePageMouseUI()
{
    bool mouseHand = m_mouseInterface->left_handed();
    if(m_mouseLeftHand != mouseHand)
    {
        ui->comboBox_hand_mode->setCurrentIndex(mouseHand);
    }

    double mouseMotionAcceleration = m_mouseInterface->motion_acceleration();
    if(m_mouseMotionAcceleration != mouseMotionAcceleration)
    {
        if(mouseMotionAcceleration == MOTION_SLOW)
        {
            ui->slider_speed->setValue(SLIDER_MINIMUM);
        }
        else if(mouseMotionAcceleration == MOTION_STANDARD)
        {
            ui->slider_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
        }
        else
        {
            ui->slider_speed->setValue(SLIDER_MAXIMUN);
        }
    }

    bool mouseNaturalScroll = m_mouseInterface->natural_scroll();
    if(m_mouseNaturalScroll != mouseNaturalScroll)
    {
        ui->checkBox_natural_scroll->setChecked(mouseNaturalScroll);
    }

    bool middleEmulationEnabled = m_mouseInterface->middle_emulation_enabled();
    if(m_middleEmulationEnabled != middleEmulationEnabled)
    {
        ui->checkBox_middle_emulation->setChecked(middleEmulationEnabled);
    }
}

void KiranCPanelMouseWidget::updatePageTouchPadUI()
{
    bool touchPadEnabled = m_touchPadInterface->touchpad_enabled();
    if(touchPadEnabled != m_touchPadEnabled)
    {
        ui->checkBox_tp_disable_touchpad->setChecked(!touchPadEnabled);
        // get widget status
        setDisableWidget(!touchPadEnabled);
    }

    bool touchPadLeftHand = m_touchPadInterface->left_handed();
    if(touchPadLeftHand != m_touchPadLeftHand)
    {
        ui->comboBox_tp_hand_mode->setCurrentIndex(touchPadLeftHand);
    }

    double touchPadMotionAcceleration = m_touchPadInterface->motion_acceleration();
    if(touchPadMotionAcceleration != m_touchPadMotionAcceleration)
    {
        if(touchPadMotionAcceleration == MOTION_SLOW)
        {
            ui->slider_tp_speed->setValue(SLIDER_MINIMUM);
            ui->label_tp_speed->setText(SLOW);
        }
        else if(touchPadMotionAcceleration == MOTION_STANDARD)
        {
            ui->slider_tp_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
            ui->label_tp_speed->setText(STANDARD);
        }
        else
        {
            ui->slider_tp_speed->setValue(SLIDER_MAXIMUN);
            ui->label_tp_speed->setText(FAST);
        }
    }

    int clickMethod = m_touchPadInterface->click_method();
    if(clickMethod != m_clickMethod)
    {
        ui->comboBox_tp_click_mode->setCurrentIndex(clickMethod);
    }

    int scrollMethod = m_touchPadInterface->scroll_method();
    if(scrollMethod != m_scrollMethod)
    {
        ui->comboBox_tp_move_win_mode ->setCurrentIndex(scrollMethod);
    }

    bool touchPadNaturalScroll = m_touchPadInterface->natural_scroll();
    if(touchPadNaturalScroll != m_touchPadNaturalScroll)
    {
        ui->checkBox_tp_natural_scroll->setChecked(touchPadNaturalScroll);
    }

    bool disabelWhileTyping = m_touchPadInterface->disable_while_typing();
    if(disabelWhileTyping != m_disabelWhileTyping)
    {
        ui->checkBox_disable_while_typing->setChecked(disabelWhileTyping);
    }

    bool tapToClick = m_touchPadInterface->tap_to_click();
    if(tapToClick != m_tapToClick)
    {
        ui->checkBox_tap_to_click->setChecked(tapToClick);
    }
}

void KiranCPanelMouseWidget::addComboBoxItem()
{
    QStringList hand_mode;
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode") ;
    ui->comboBox_hand_mode->addItems(hand_mode);
    ui->comboBox_tp_hand_mode->addItems(hand_mode);

    QStringList tpClickMode;
    tpClickMode << tr("Press and Tap") << tr("Tap") ;
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
    double scrollSpeed;

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
        labelSpeed->setText(tr(SLOW));
        scrollSpeed = MOTION_SLOW;
    }
    else if(value >= lowMiddleNum && value <= middleNum)
    {
        senderSlider->setValue(middleNum);
        labelSpeed->setText(tr(STANDARD));
        scrollSpeed = MOTION_STANDARD;
    }
    else if(value >middleNum && value<= highMiddleNum)
    {
        senderSlider->setValue(middleNum);
        labelSpeed->setText(tr(STANDARD));
        scrollSpeed = MOTION_STANDARD;
    }
    else
    {
        senderSlider->setValue(SLIDER_MAXIMUN);
        labelSpeed->setText(tr(FAST));
        scrollSpeed = MOTION_FAST;
    }
    if(ismouseSlider)
    {
        m_mouseMotionAcceleration = scrollSpeed;
        m_mouseInterface->setMotion_acceleration(m_mouseMotionAcceleration);
    }
    else
    {
        m_touchPadMotionAcceleration = scrollSpeed;
        m_touchPadInterface->setMotion_acceleration(m_touchPadMotionAcceleration);
    }
}

void KiranCPanelMouseWidget::onDisabelTouchPadToggled(bool disabled)
{
    m_touchPadEnabled = !disabled;
    m_touchPadInterface->setTouchpad_enabled(m_touchPadEnabled);
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

