/**
 * @file touchpad-settings.cpp
 * @brief  初始化触摸板属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include "touchpad-settings.h"
#include "ui_touchpad-settings.h"
#include "general-functions/general-function-class.h"
#include "dbus-interface/touchpad-interface.h"

#include <QCheckBox>

TouchPadSettings::TouchPadSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TouchPadSettings),
    m_touchPadInterface(nullptr)
{
    ui->setupUi(this);
    m_touchPadInterface = ComKylinsecKiranSessionDaemonTouchPadInterface::instance();
    initUI();
}

TouchPadSettings::~TouchPadSettings()
{
    delete ui;
}

/**
 * @brief 初始化控件
 */
void TouchPadSettings::initUI()
{
    ui->label_tp_speed->setStyleSheet("#label_tp_speed{color:#2ca5ea;}"
                                      "#label_tp_speed:disabled{color:#696969;}");

    m_comboBoxList = this->findChildren<QComboBox *>();
    m_checkBoxList = {ui->checkBox_tap_to_click,
                      ui->checkBox_tp_natural_scroll,
                      ui->checkBox_disable_while_typing};

    m_labelList = this->findChildren<QLabel *>();

    addComboBoxItem();

    ui->slider_tp_speed->setMaximum(SLIDER_MAXIMUN);
    ui->slider_tp_speed->setMinimum(SLIDER_MINIMUM);
    ui->slider_tp_speed->setPageStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
    ui->slider_tp_speed->setSingleStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);

    //TODO: 暂时隐藏触摸板点击方法功能
    ui->widget_tp_click_mode->hide();

    initPageTouchPadUI();
}

/**
 * @brief 通过Dbus获取触摸板属性值，监听用户修改属性的信号，并重新设置属性值
 */
void TouchPadSettings::initPageTouchPadUI()
{
    m_touchPadEnabled = m_touchPadInterface->touchpad_enabled();
    ui->checkBox_tp_disable_touchpad->setChecked(m_touchPadEnabled);
    if(!m_touchPadEnabled)
    {
        //若禁用触摸板，则禁用触摸板相关设置控件
        setDisableWidget(true);
    }
    connect(ui->checkBox_tp_disable_touchpad, &QCheckBox::toggled, this,
            &TouchPadSettings::onDisabelTouchPadToggled);

    m_touchPadLeftHand = m_touchPadInterface->left_handed();
    ui->comboBox_tp_hand_mode->setCurrentIndex(m_touchPadLeftHand);
    connect(ui->comboBox_tp_hand_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIntex){
        m_touchPadLeftHand = currentIntex;
        m_touchPadInterface->setLeft_handed(m_touchPadLeftHand);
    });

    m_touchPadMotionAcceleration = m_touchPadInterface->motion_acceleration();
    if(m_touchPadMotionAcceleration == MOTION_SLOW)
    {
        ui->slider_tp_speed->setValue(SLIDER_MINIMUM);
        ui->label_tp_speed->setText(tr("Slow"));
    }
    else if(m_touchPadMotionAcceleration == MOTION_STANDARD)
    {
        ui->slider_tp_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
        ui->label_tp_speed->setText(tr("Standard"));
    }
    else
    {
        ui->slider_tp_speed->setValue(SLIDER_MAXIMUN);
        ui->label_tp_speed->setText(tr("Fast"));
    }

    //创建定时器，在用户拖动滑动条时，滑动条值停止变化0.1s后才会设置新的触摸板移动速度
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this]{
        double scrollSpeed;
        int value;

        value = ui->slider_tp_speed->value();
        scrollSpeed = GeneralFunctionClass::convertValue(ui->slider_tp_speed,ui->label_tp_speed,m_mousePressed,value);

        m_touchPadMotionAcceleration = scrollSpeed;
        m_touchPadInterface->setMotion_acceleration(m_touchPadMotionAcceleration);
        m_timer->stop();
    });

    connect(ui->slider_tp_speed,&QSlider::sliderPressed,[this](){
        m_mousePressed = true;
    });
    connect(ui->slider_tp_speed,&QSlider::sliderReleased,[this](){
        m_mousePressed = false;
        emit ui->slider_tp_speed->valueChanged(ui->slider_tp_speed->value());
    });
    // 监听滑动条值变化信号，当用户拖动滑动条时，只有在鼠标松开后才会根据值范围确定滑动条值
    connect(ui->slider_tp_speed,&QSlider::valueChanged,this, &TouchPadSettings::onSliderValueChange);

    m_clickMethod = m_touchPadInterface->click_method();
    ui->comboBox_tp_click_mode->setCurrentIndex(m_clickMethod);
    connect(ui->comboBox_tp_click_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIndex){
       m_clickMethod = currentIndex;
       m_touchPadInterface->setClick_method(m_clickMethod);
    });

    m_scrollMethod = m_touchPadInterface->scroll_method();
    ui->comboBox_tp_move_win_mode ->setCurrentIndex(m_scrollMethod);
    connect(ui->comboBox_tp_move_win_mode,QOverload<int>::of(&QComboBox::currentIndexChanged),
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

QSize TouchPadSettings::sizeHint() const
{
    return QSize(918,750);
}

void TouchPadSettings::addComboBoxItem()
{
    QStringList hand_mode;
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode") ;
    ui->comboBox_tp_hand_mode->addItems(hand_mode);

    QStringList tpClickMode;
    tpClickMode << tr("Press and Tap") << tr("Tap") ;
    ui->comboBox_tp_click_mode->addItems(tpClickMode);

    QStringList tpScrollWinMode;
    tpScrollWinMode << tr("Two Finger Scroll") <<tr("Edge Scroll");
    ui->comboBox_tp_move_win_mode->addItems(tpScrollWinMode);
}

/**
 * @brief 根据是否禁用触摸板来设置相关控件的状态
 * @param[in] disabled 是否禁用触摸板
 */
void TouchPadSettings::setDisableWidget(bool disabled)
{
    foreach (QComboBox *comboBox, m_comboBoxList)
    {
        comboBox->setDisabled(disabled);
    }
    foreach (KiranSwitchButton* checkBox, m_checkBoxList)
    {
        checkBox->setDisabled(disabled);
    }
    ui->slider_tp_speed->setDisabled(disabled);
    ui->label_tp_speed->setDisabled(disabled);
}

void TouchPadSettings::onSliderValueChange()
{
    //触发定时器
    m_timer->start(100);
}

void TouchPadSettings::onDisabelTouchPadToggled(bool disabled)
{
    m_touchPadEnabled = disabled;
    m_touchPadInterface->setTouchpad_enabled(m_touchPadEnabled);
    if(disabled)
    {
        setDisableWidget(false);
    }
    else
    {
        setDisableWidget(true);
    }
}
