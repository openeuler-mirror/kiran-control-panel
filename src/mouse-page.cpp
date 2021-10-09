/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-mouse is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "mouse-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/mouse-i.h>
#include <QDBusConnection>
#include "KSMMouseProxy.h"
#include "kcm-manager.h"
#include "ui_mouse-page.h"

#define TIMEOUT 100
#define SLIDER_MINIMUM 0
#define SLIDER_MAXIMUN 100
#define PAGE_BASE_SIZE_WIDTH 700
#define PAGE_BASE_SIZE_HEIGHT 670

MousePage::MousePage(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::MousePage),
                                        m_mouseInterface(nullptr)
{
    ui->setupUi(this);

    //创建定时器，在用户拖动滑动条时，滑动条值停止变化0.1s后才会设置新的鼠标移动速度
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this] {
                int value = ui->slider_speed->value();
                auto scrollSpeed = (value / (SLIDER_MAXIMUN * 1.0)) * 2.0 - 1.0;
                m_mouseMotionAcceleration = scrollSpeed;
                m_mouseInterface->setMotion_acceleration(m_mouseMotionAcceleration);
                m_timer->stop();
            });

    initUI();
}

MousePage::~MousePage()
{
    delete ui;
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_mouseInterface)
        m_mouseInterface.clear();
}

QSize MousePage::sizeHint() const
{
    return QSize(PAGE_BASE_SIZE_WIDTH, PAGE_BASE_SIZE_HEIGHT);
}

/**
 * @brief 初始化控件
 */
void MousePage::initUI()
{
    KCMManager *kcmManager = new KCMManager;

    m_mouseInterface = kcmManager->getMouseInterface();

    delete kcmManager;
    kcmManager = nullptr;

    setStyleSheet("#scrollAreaWidgetContents{border-left:1px solid #2d2d2d;}");
    QStringList hand_mode;
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode");
    ui->comboBox_hand_mode->addItems(hand_mode);

    ui->slider_speed->setMaximum(SLIDER_MAXIMUN);
    ui->slider_speed->setMinimum(SLIDER_MINIMUM);
    ui->slider_speed->setPageStep((SLIDER_MAXIMUN - SLIDER_MINIMUM) / 20);
    ui->slider_speed->setSingleStep((SLIDER_MAXIMUN - SLIDER_MINIMUM) / 20);

    initComponent();
}

/**
 * @brief 通过Dbus获取鼠标属性值，监听用户修改属性的信号，并重新设置属性值
 */
void MousePage::initComponent()
{
    m_mouseLeftHand = m_mouseInterface->left_handed();
    ui->comboBox_hand_mode->setCurrentIndex(m_mouseLeftHand);
    connect(ui->comboBox_hand_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIntex) {
                m_mouseLeftHand = currentIntex;
                m_mouseInterface->setLeft_handed(m_mouseLeftHand);
            });

    m_mouseMotionAcceleration = m_mouseInterface->motion_acceleration();
    int speed = m_mouseMotionAcceleration / 2.0 * SLIDER_MAXIMUN + SLIDER_MAXIMUN / 2;
    ui->slider_speed->setValue(speed);

    // 监听滑动条值变化信号，当用户拖动滑动条时，只有在鼠标松开后才会根据值范围确定滑动条值
    connect(ui->slider_speed, &QSlider::sliderPressed, [this]() {
        m_mousePressed = true;
    });
    connect(ui->slider_speed, &QSlider::sliderReleased, [this]() {
        m_mousePressed = false;
        emit ui->slider_speed->valueChanged(ui->slider_speed->value());
    });
    connect(ui->slider_speed, &QSlider::valueChanged, this, &MousePage::onSliderValueChange);

    m_mouseNaturalScroll = m_mouseInterface->natural_scroll();
    ui->checkBox_natural_scroll->setChecked(m_mouseNaturalScroll);
    connect(ui->checkBox_natural_scroll, &KiranSwitchButton::toggled,
            [this](bool ischecked) {
                m_mouseNaturalScroll = ischecked;
                m_mouseInterface->setNatural_scroll(ischecked);
            });

    m_middleEmulationEnabled = m_mouseInterface->middle_emulation_enabled();
    ui->checkBox_middle_emulation->setChecked(m_middleEmulationEnabled);
    connect(ui->checkBox_middle_emulation, &KiranSwitchButton::toggled,
            [this](bool ischecked) {
                m_middleEmulationEnabled = ischecked;
                m_mouseInterface->setMiddle_emulation_enabled(ischecked);
            });
}

void MousePage::onSliderValueChange()
{
    //触发定时器
    m_timer->start(TIMEOUT);
}
