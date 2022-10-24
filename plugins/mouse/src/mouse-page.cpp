/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "mouse-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/mouse-i.h>
#include <QDBusConnection>
#include "kcm-manager.h"
#include "mouse_backEnd_proxy.h"
#include "ui_mouse-page.h"

#define TIMEOUT 100
#define SLIDER_MINIMUM 0
#define SLIDER_MAXIMUN 100
#define PAGE_BASE_SIZE_WIDTH 500
#define PAGE_BASE_SIZE_HEIGHT 657

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
    connect(
        m_mouseInterface.data(), &MouseBackEndProxy::left_handedChanged, this,
        [this](bool value) {
            if (m_mouseLeftHand != value)
            {
                m_mouseLeftHand = value;
                ui->comboBox_hand_mode->blockSignals(true);
                ui->comboBox_hand_mode->setCurrentIndex(m_mouseLeftHand);
                ui->comboBox_hand_mode->blockSignals(false);
            }
        },
        Qt::QueuedConnection);

    m_mouseMotionAcceleration = m_mouseInterface->motion_acceleration();
    int speed = m_mouseMotionAcceleration / 2.0 * SLIDER_MAXIMUN + SLIDER_MAXIMUN / 2;
    ui->slider_speed->setValue(speed);

    connect(ui->slider_speed, &QSlider::valueChanged, this, &MousePage::onSliderValueChange);

    connect(
        m_mouseInterface.data(), &MouseBackEndProxy::motion_accelerationChanged, this,
        [this](double value) {
            if (m_mouseMotionAcceleration != value)
            {
                m_mouseMotionAcceleration = value;
                int speed = value / 2.0 * SLIDER_MAXIMUN + SLIDER_MAXIMUN / 2;
                ui->slider_speed->blockSignals(true);
                ui->slider_speed->setValue(speed);
                ui->slider_speed->blockSignals(false);
            }
        },
        Qt::QueuedConnection);

    m_mouseNaturalScroll = m_mouseInterface->natural_scroll();
    ui->checkBox_natural_scroll->setChecked(m_mouseNaturalScroll);
    connect(ui->checkBox_natural_scroll, &KiranSwitchButton::toggled,
            [this](bool ischecked) {
                m_mouseNaturalScroll = ischecked;
                m_mouseInterface->setNatural_scroll(ischecked);
            });
    connect(
        m_mouseInterface.data(), &MouseBackEndProxy::natural_scrollChanged, this,
        [this](bool value) {
            if (m_mouseNaturalScroll != value)
            {
                m_mouseNaturalScroll = value;
                ui->checkBox_natural_scroll->blockSignals(true);
                ui->checkBox_natural_scroll->setChecked(value);
                ui->checkBox_natural_scroll->blockSignals(false);
            }
        },
        Qt::QueuedConnection);

    m_middleEmulationEnabled = m_mouseInterface->middle_emulation_enabled();
    ui->checkBox_middle_emulation->setChecked(m_middleEmulationEnabled);
    connect(ui->checkBox_middle_emulation, &KiranSwitchButton::toggled,
            [this](bool ischecked) {
                m_middleEmulationEnabled = ischecked;
                m_mouseInterface->setMiddle_emulation_enabled(ischecked);
            });
    connect(
        m_mouseInterface.data(), &MouseBackEndProxy::middle_emulation_enabledChanged, this,
        [this](bool value) {
            if (m_middleEmulationEnabled != value)
            {
                m_middleEmulationEnabled = value;
                ui->checkBox_middle_emulation->blockSignals(true);
                ui->checkBox_middle_emulation->setChecked(value);
                ui->checkBox_middle_emulation->blockSignals(false);
            }
        },
        Qt::QueuedConnection);
}

void MousePage::onSliderValueChange()
{
    //触发定时器
    m_timer->start(TIMEOUT);
}
