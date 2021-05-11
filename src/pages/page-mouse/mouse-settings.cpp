/**
 * @file mouse-settings.cpp
 * @brief  初始化鼠标属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include "mouse-settings.h"
#include "ui_mouse-settings.h"
#include "general-functions/general-function-class.h"
#include "dbus-interface/mouse-interface.h"

MouseSettings::MouseSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MouseSettings)
{
    ui->setupUi(this);
}

MouseSettings::~MouseSettings()
{
    delete ui;
}

/**
 * @brief 连接Dbus服务，初始化控件
 * @return true:连接Dbus服务成功
 *         false:连接Dbus服务失败
 */
bool MouseSettings::initUI()
{
    m_mouseInterface = ComKylinsecKiranSessionDaemonMouseInterface::instance();
    if(!m_mouseInterface->isValid())
    {
        qDebug() << "mouse interface invalid" << endl;
        return false;
    }

    QStringList hand_mode;
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode") ;
    ui->comboBox_hand_mode->addItems(hand_mode);

    ui->slider_speed->setMaximum(SLIDER_MAXIMUN);
    ui->slider_speed->setMinimum(SLIDER_MINIMUM);
    ui->slider_speed->setPageStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
    ui->slider_speed->setSingleStep((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);

    initPageMouseUI();
    return true;
}

/**
 * @brief 通过Dbus获取鼠标属性值，监听用户修改属性的信号，并重新设置属性值
 */
void MouseSettings::initPageMouseUI()
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
        ui->label_speed->setText(tr("Slow"));
    }
    else if(m_mouseMotionAcceleration == MOTION_STANDARD)
    {
        ui->slider_speed->setValue((SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2);
        ui->label_speed->setText(tr("Standard"));
    }
    else
    {
        ui->slider_speed->setValue(SLIDER_MAXIMUN);
        ui->label_speed->setText(tr("Fast"));
    }

    //创建定时器，在用户拖动滑动条时，滑动条值停止变化0.1s后才会设置新的鼠标移动速度
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this]{
        int value;
        double scrollSpeed;
        value = ui->slider_speed->value();
        scrollSpeed = GeneralFunctionClass::convertValue(ui->slider_speed,ui->label_speed,m_mousePressed,value);
        m_mouseMotionAcceleration = scrollSpeed;
        m_mouseInterface->setMotion_acceleration(m_mouseMotionAcceleration);
        m_timer->stop();
    });

    // 监听滑动条值变化信号，当用户拖动滑动条时，只有在鼠标松开后才会根据值范围确定滑动条值
    connect(ui->slider_speed,&QSlider::sliderPressed,[this](){
        m_mousePressed = true;
    });
    connect(ui->slider_speed,&QSlider::sliderReleased,[this](){
        m_mousePressed = false;
        emit ui->slider_speed->valueChanged(ui->slider_speed->value());
    });
    connect(ui->slider_speed, &QSlider::valueChanged, this,&MouseSettings::onSliderValueChange);

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

void MouseSettings::onSliderValueChange()
{
    //触发定时器
    m_timer->start(100);
}
