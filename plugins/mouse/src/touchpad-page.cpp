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

#include "touchpad-page.h"
#include "kcm-manager.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <QCheckBox>
#include <QDBusConnection>
#include "touchPad_backEnd_proxy.h"
#include "ui_touchpad-page.h"

#define TIMEOUT 2000
#define SLIDER_MINIMUM 0
#define SLIDER_MAXIMUN 100
#define PAGE_BASE_SIZE_WIDTH 700
#define PAGE_BASE_SIZE_HEIGHT 670

TouchPadPage::TouchPadPage(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::TouchPadPage),
                                              m_touchPadInterface(nullptr)
{
    ui->setupUi(this);

    //创建定时器，在用户拖动滑动条时，滑动条值停止变化0.1s后才会设置新的触摸板移动速度
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this] {
                int value = ui->slider_tp_speed->value();
                auto scrollSpeed = (value / (SLIDER_MAXIMUN * 1.0)) * 2.0 - 1.0;
                m_touchPadMotionAcceleration = scrollSpeed;
                m_touchPadInterface->setMotion_acceleration(m_touchPadMotionAcceleration);
                m_timer->stop();
            });

    initUI();
}

TouchPadPage::~TouchPadPage()
{
    delete ui;
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_touchPadInterface)
        m_touchPadInterface.clear();
}

/**
 * @brief 初始化控件
 */
void TouchPadPage::initUI()
{
    KCMManager *kcmManager = new KCMManager;

    m_touchPadInterface = kcmManager->getTouchPadInterface();

    delete kcmManager;
    kcmManager = nullptr;

    m_comboBoxList = this->findChildren<QComboBox *>();
    m_checkBoxList = {ui->checkBox_tap_to_click,
                      ui->checkBox_tp_natural_scroll,
                      ui->checkBox_disable_while_typing};

    m_labelList = this->findChildren<QLabel *>();

    addComboBoxItem();

    ui->slider_tp_speed->setMaximum(SLIDER_MAXIMUN);
    ui->slider_tp_speed->setMinimum(SLIDER_MINIMUM);
    ui->slider_tp_speed->setPageStep((SLIDER_MAXIMUN - SLIDER_MINIMUM) / 20);
    ui->slider_tp_speed->setSingleStep((SLIDER_MAXIMUN - SLIDER_MINIMUM) / 20);

    ///TODO: 暂时隐藏触摸板点击方法功能
    ui->widget_tp_click_mode->hide();

    initComponent();
}

/**
 * @brief 通过Dbus获取触摸板属性值，监听用户修改属性的信号，并重新设置属性值
 */
void TouchPadPage::initComponent()
{
    //触摸板是否开启
    m_touchPadEnabled = m_touchPadInterface->touchpad_enabled();
    ui->checkBox_tp_disable_touchpad->setChecked(m_touchPadEnabled);
    if (!m_touchPadEnabled)
        setDisableWidget(true);  //若禁用触摸板，则禁用触摸板相关设置控件

    connect(ui->checkBox_tp_disable_touchpad, &QCheckBox::toggled, this,
            &TouchPadPage::onDisabelTouchPadToggled);
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::touchpad_enabledChanged, this,
        [this](bool value) {
            if (m_touchPadEnabled != value)
            {
                m_touchPadEnabled = value;
                ui->checkBox_tp_disable_touchpad->blockSignals(true);
                ui->checkBox_tp_disable_touchpad->setChecked(value);
                ui->checkBox_tp_disable_touchpad->blockSignals(false);
                setDisableWidget(!value);
            }
        },
        Qt::QueuedConnection);

    //选择触摸板使用模式
    m_touchPadLeftHand = m_touchPadInterface->left_handed();
    ui->comboBox_tp_hand_mode->setCurrentIndex(m_touchPadLeftHand);
    connect(ui->comboBox_tp_hand_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIntex) {
                m_touchPadLeftHand = currentIntex;
                m_touchPadInterface->setLeft_handed(m_touchPadLeftHand);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::left_handedChanged, this,
        [this](bool value) {
            if (m_touchPadLeftHand != value)
            {
                m_touchPadLeftHand = value;
                ui->comboBox_tp_hand_mode->blockSignals(true);
                ui->comboBox_tp_hand_mode->setCurrentIndex(value);
                ui->comboBox_tp_hand_mode->blockSignals(false);
            }
        },
        Qt::QueuedConnection);

    //触摸板移动加速
    m_touchPadMotionAcceleration = m_touchPadInterface->motion_acceleration();
    int speed = m_touchPadMotionAcceleration / 2.0 * SLIDER_MAXIMUN + SLIDER_MAXIMUN / 2;
    ui->slider_tp_speed->setValue(speed);

    connect(ui->slider_tp_speed, &QSlider::valueChanged, this, &TouchPadPage::onSliderValueChange);
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::motion_accelerationChanged, this,
        [this](double value) {
            if (m_touchPadMotionAcceleration != value)
            {
                m_touchPadMotionAcceleration = value;
                int speed = value / 2.0 * SLIDER_MAXIMUN + SLIDER_MAXIMUN / 2;
                ui->slider_tp_speed->blockSignals(true);
                ui->slider_tp_speed->setValue(speed);
                ui->slider_tp_speed->blockSignals(false);
            }
        },
        Qt::QueuedConnection);

    //点击模式
    m_clickMethod = m_touchPadInterface->click_method();
    ui->comboBox_tp_click_mode->setCurrentIndex(m_clickMethod);
    connect(ui->comboBox_tp_click_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIndex) {
                m_clickMethod = currentIndex;
                m_touchPadInterface->setClick_method(m_clickMethod);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::click_methodChanged, this,
        [this](int value) {
            setValue(ui->comboBox_tp_click_mode, m_clickMethod, value);
        },
        Qt::QueuedConnection);

    //滚动方式
    m_scrollMethod = m_touchPadInterface->scroll_method();
    ui->comboBox_tp_move_win_mode->setCurrentIndex(m_scrollMethod);
    connect(ui->comboBox_tp_move_win_mode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int currentIndex) {
                m_scrollMethod = currentIndex;
                m_touchPadInterface->setScroll_method(m_scrollMethod);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::scroll_methodChanged, this,
        [this](int value) {
            setValue(ui->comboBox_tp_move_win_mode, m_scrollMethod, value);
        },
        Qt::QueuedConnection);

    //是否为自然滚动
    m_touchPadNaturalScroll = m_touchPadInterface->natural_scroll();
    ui->checkBox_tp_natural_scroll->setChecked(m_touchPadNaturalScroll);
    connect(ui->checkBox_tp_natural_scroll, &KiranSwitchButton::toggled,
            [this](bool isNaturalScroll) {
                m_touchPadNaturalScroll = isNaturalScroll;
                m_touchPadInterface->setNatural_scroll(m_touchPadNaturalScroll);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::natural_scrollChanged, this,
        [this](bool value) {
            setValue(ui->checkBox_tp_natural_scroll, m_touchPadNaturalScroll, value);
        },
        Qt::QueuedConnection);

    //打字时触摸板禁用
    m_disabelWhileTyping = m_touchPadInterface->disable_while_typing();
    ui->checkBox_disable_while_typing->setChecked(m_disabelWhileTyping);
    connect(ui->checkBox_disable_while_typing, &KiranSwitchButton::toggled,
            [this](bool disabelWhileTyping) {
                m_disabelWhileTyping = disabelWhileTyping;
                m_touchPadInterface->setDisable_while_typing(m_disabelWhileTyping);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::disable_while_typingChanged, this,
        [this](bool value) {
            setValue(ui->checkBox_disable_while_typing, m_disabelWhileTyping, value);
        },
        Qt::QueuedConnection);

    //轻击(不按下)触摸板功能是否生效
    m_tapToClick = m_touchPadInterface->tap_to_click();
    ui->checkBox_tap_to_click->setChecked(m_tapToClick);
    connect(ui->checkBox_tap_to_click, &KiranSwitchButton::toggled,
            [this](bool isTapToClick) {
                m_tapToClick = isTapToClick;
                m_touchPadInterface->setTap_to_click(m_tapToClick);
            });
    connect(
        m_touchPadInterface.data(), &TouchPadBackEndProxy::tap_to_clickChanged, this,
        [this](bool value) {
            setValue(ui->checkBox_tap_to_click, m_tapToClick, value);
        },
        Qt::QueuedConnection);
}

void TouchPadPage::setValue(KiranSwitchButton *receiveWidget, bool &origVal, bool newVal)
{
    if (origVal != newVal)
    {
        origVal = newVal;
        receiveWidget->blockSignals(true);
        receiveWidget->setChecked(newVal);
        receiveWidget->blockSignals(false);
    }
}

void TouchPadPage::setValue(QComboBox *receiveWidget, int &origVal, int newVal)
{
    if (origVal != newVal)
    {
        origVal = newVal;
        receiveWidget->blockSignals(true);
        receiveWidget->setCurrentIndex(newVal);
        receiveWidget->blockSignals(false);
    }
}

QSize TouchPadPage::sizeHint() const
{
    return QSize(PAGE_BASE_SIZE_WIDTH, PAGE_BASE_SIZE_HEIGHT);
}

void TouchPadPage::addComboBoxItem()
{
    QStringList hand_mode;
    hand_mode << tr("Right Hand Mode") << tr("Left Hand Mode");
    ui->comboBox_tp_hand_mode->addItems(hand_mode);

    QStringList tpClickMode;
    tpClickMode << tr("Press and Tap") << tr("Tap");
    ui->comboBox_tp_click_mode->addItems(tpClickMode);

    QStringList tpScrollWinMode;
    tpScrollWinMode << tr("Two Finger Scroll") << tr("Edge Scroll");
    ui->comboBox_tp_move_win_mode->addItems(tpScrollWinMode);
}

/**
 * @brief 根据是否禁用触摸板来设置相关控件的状态
 * @param[in] disabled 是否禁用触摸板
 */
void TouchPadPage::setDisableWidget(bool disabled)
{
    foreach (QComboBox *comboBox, m_comboBoxList)
    {
        comboBox->setDisabled(disabled);
    }
    foreach (KiranSwitchButton *checkBox, m_checkBoxList)
    {
        checkBox->setDisabled(disabled);
    }
    ui->slider_tp_speed->setDisabled(disabled);
}

void TouchPadPage::onSliderValueChange()
{
    //触发定时器
    m_timer->start(TIMEOUT);
}

void TouchPadPage::onDisabelTouchPadToggled(bool disabled)
{
    m_touchPadEnabled = disabled;
    m_touchPadInterface->setTouchpad_enabled(m_touchPadEnabled);
    setDisableWidget(!disabled);
}
