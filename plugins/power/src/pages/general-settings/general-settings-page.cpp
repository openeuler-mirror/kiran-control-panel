/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <QGSettings/QGSettings>
#include <QListView>
#include <QSignalBlocker>

#include "dbus/kwin-color-correct.h"
#include "dbus/power-profiles-wrapper.h"
#include "dbus/power.h"
#include "general-settings-page.h"
#include "kiran-message-box.h"
#include "kiran-session-daemon/power-i.h"
#include "ui_general-settings-page.h"

#define MAX_IDLE_TIME 120

#define MATE_SESSION_SCHEMA_ID "org.mate.session"
#define KIRAN_SESSION_SCHEMA_ID "com.kylinsec.kiran.session-manager"
// #define KEY_IDLE_DELAY "idle-delay"
#define KEY_IDLE_DELAY "idleDelay"
#define DEFAULT_IDLE_DELAY 5

#define SCHEMA_KIRAN_SCREENSAVER "com.kylinsec.kiran.screensaver"
#define KEY_IDLE_ACTIVATION_LOCK "idleActivationLock"

GeneralSettingsPage::GeneralSettingsPage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::GeneralSettingsPage),
      m_powerInterface(PowerInterface::getInstance()),
      m_kwinColorCorrect(new KWinColorCorrect(this)),
      m_powerprofiles(new PowerProfilesWrapper(this))
{
    ui->setupUi(this);
    init();
}

GeneralSettingsPage::~GeneralSettingsPage()
{
    delete ui;
}

void GeneralSettingsPage::init()
{
    initSessionSetting();
    initUI();
    initConnection();
    load();
}

void GeneralSettingsPage::initSessionSetting()
{
    if (QGSettings::isSchemaInstalled(KIRAN_SESSION_SCHEMA_ID))
    {
        m_sessionSettings = new QGSettings(KIRAN_SESSION_SCHEMA_ID, QByteArray(), this);
    }
    else if (QGSettings::isSchemaInstalled(MATE_SESSION_SCHEMA_ID))
    {
        m_sessionSettings = new QGSettings(MATE_SESSION_SCHEMA_ID, QByteArray(), this);
    }

    if (m_sessionSettings)
    {
        connect(m_sessionSettings, &QGSettings::changed, [this](const QString& key)
                {
            if (key != KEY_IDLE_DELAY)
            {
                return;
            }

            int value = m_sessionSettings->get(key).toInt();
            if (value == ui->slider_idleTime->value())
            {
                return;
            }

            QSignalBlocker blocker(ui->slider_idleTime);
            ui->slider_idleTime->setValue(m_sessionSettings->get(key).toInt()); });
    }


    if (QGSettings::isSchemaInstalled(SCHEMA_KIRAN_SCREENSAVER))
    {
        m_screensaverSettings = new QGSettings(SCHEMA_KIRAN_SCREENSAVER, QByteArray(), this);
    }

    if (m_screensaverSettings)
    {
        connect(m_screensaverSettings, &QGSettings::changed, [this](const QString& key)
                {
            if(key != KEY_IDLE_ACTIVATION_LOCK)
            {
                return ;
            }

            bool value = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
            m_switchIdlelockScreen->setChecked(value); });
    }
    else
    {
        KLOG_WARNING() << "qgsettings" << SCHEMA_KIRAN_SCREENSAVER << "is not install!";
    }
}

void GeneralSettingsPage::initUI()
{
    /// 填充选项
    typedef QList<QPair<QString, PowerAction>> Actions;
    // clang-format off
    QList<QPair<QComboBox*,Actions>> actions = {
        {ui->combo_powerButton,{
            {tr("shutdown"), POWER_ACTION_COMPUTER_SHUTDOWN},
            {tr("hibernate"), POWER_ACTION_COMPUTER_HIBERNATE},
            {tr("suspend"), POWER_ACTION_COMPUTER_SUSPEND},
            {tr("display off"), POWER_ACTION_DISPLAY_OFF},
            {tr("do nothing"), POWER_ACTION_NOTHING}}},
        {ui->combo_suspendButton,{
            {tr("suspend"),POWER_ACTION_COMPUTER_SUSPEND},
            {tr("hibernate"),POWER_ACTION_COMPUTER_HIBERNATE},
            {tr("display off"),POWER_ACTION_DISPLAY_OFF},
            {tr("do nothing"),POWER_ACTION_NOTHING}}},
        {ui->combo_closingLid,{
            {tr("suspend"),POWER_ACTION_COMPUTER_SUSPEND},
            {tr("hibernate"),POWER_ACTION_COMPUTER_HIBERNATE},
            {tr("shutdown"),POWER_ACTION_COMPUTER_SHUTDOWN},
            {tr("do nothing"),POWER_ACTION_NOTHING}}}
    };
    // clang-format on

    /// 填充ComboBox
    for (auto& action : actions)
    {
        QComboBox* comboBox = action.first;
        for (const auto& actionIter : action.second)
        {
            comboBox->addItem(actionIter.first, actionIter.second);
        }
    }

    // 初始化计算机模式
    if (m_powerprofiles->isValid())
    {
        ui->combo_computerMode->addItems(m_powerprofiles->supportedProfiles());
    }
    else
    {
        ui->widget_computerMode->setVisible(false);
    }

    /// 初始化QSlider,和延迟设置的Timer
    ui->slider_brightness->setMaximum(100);
    ui->slider_brightness->setMinimum(20);
    m_brightnessSettingTimer.setInterval(300);
    m_brightnessSettingTimer.setSingleShot(true);

    ui->slider_idleTime->setMaximum(MAX_IDLE_TIME);
    ui->slider_idleTime->setMinimum(1);
    m_idleTimeSettingTimer.setInterval(300);
    m_idleTimeSettingTimer.setSingleShot(true);

    bool lidIsPresent = m_powerInterface->lidIsPresent();
    ui->widget_lid->setVisible(lidIsPresent);

    // 色温
    ui->slider_colorTemp->setMinimum(0);
    ui->slider_colorTemp->setMaximum(100);

    m_colorTempSettingTimer.setInterval(300);
    m_colorTempSettingTimer.setSingleShot(true);

    m_switchAutoColorTemp = new KiranSwitchButton(this);
    m_switchAutoColorTemp->setAccessibleName("SwitchAutoColorTemp");
    ui->layout_autoColortemp->addWidget(m_switchAutoColorTemp);

    if (!m_kwinColorCorrect->isValid())
    {
        ui->widget_colorTemperature->setVisible(false);
    }

    // 空闲时是否锁定屏幕及屏保
    m_switchIdlelockScreen = new KiranSwitchButton(this);
    m_switchIdlelockScreen->setAccessibleName("SwitchIdleLockScreen");
    ui->layout_idleLock->addWidget(m_switchIdlelockScreen);

    // 待机唤醒是否需要输入密码
    m_switchSuspendLockScreen = new KiranSwitchButton(this);
    m_switchSuspendLockScreen->setAccessibleName("SwitchSuspendLockScreen");
    ui->layout_suspendLockScreen->addWidget(m_switchSuspendLockScreen);

    // NOTE: 根据#48515缺陷单所做修改,隐藏挂起按钮相关配置项，后续若有需要再进行打开
    ui->widget_suspend->setVisible(false);
}

void GeneralSettingsPage::initConnection()
{
    connect(ui->combo_powerButton, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onEventActionChanged);
    connect(ui->combo_suspendButton, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onEventActionChanged);
    connect(ui->combo_closingLid, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::onEventActionChanged);

    connect(ui->combo_computerMode, &QComboBox::currentTextChanged,
            this, &GeneralSettingsPage::onComboComputerModeChanged);

    connect(&m_brightnessSettingTimer, &QTimer::timeout,
            this, &GeneralSettingsPage::onBrightnessSettingTimerTimeout);
    connect(ui->slider_brightness, &QSlider::valueChanged,
            this, &GeneralSettingsPage::onSliderBrightnessValueChanged);

    connect(m_switchAutoColorTemp, &QAbstractButton::toggled,
            this, &GeneralSettingsPage::onSwitchAutoColorTempToggoled);
    connect(&m_colorTempSettingTimer, &QTimer::timeout,
            this, &GeneralSettingsPage::onColorTempSettingTimerTimeout);
    connect(ui->slider_colorTemp, &QSlider::valueChanged,
            this, &GeneralSettingsPage::onSliderColorTempValueChanged);

    connect(ui->slider_idleTime, &QSlider::valueChanged,
            this, &GeneralSettingsPage::onSliderIdleTimeValueChanged);
    connect(&m_idleTimeSettingTimer, &QTimer::timeout,
            this, &GeneralSettingsPage::onIdleTimeSettingTimerTimeout);

    connect(m_switchIdlelockScreen, &QAbstractButton::toggled,
            this, &GeneralSettingsPage::onSwitchIdleLockToggled);
    connect(m_switchSuspendLockScreen, &QAbstractButton::toggled,
            this, &GeneralSettingsPage::onSwitchSuspendLockToggled);
}

void GeneralSettingsPage::load()
{
    auto getEventComboBoxIdxFunc = [this](QComboBox* comboBox, PowerEvent event) -> int
    {
        int comboBoxIdx = -1;
        int action = -1;

        auto getEventActionReply = m_powerInterface->GetEventAction(event);
        getEventActionReply.waitForFinished();
        if (getEventActionReply.isError())
        {
            QString errMsg = getEventActionReply.error().message();
            KLOG_WARNING("get event(%d) action failed,%s", event, errMsg.toStdString().c_str());
            return -1;
        }

        action = getEventActionReply.value();
        comboBoxIdx = comboBox->findData(action);
        if (comboBoxIdx == -1)
        {
            KLOG_WARNING() << "combobox(" << comboBox->objectName() << ") can't find this action(" << action << ")!";
            return -1;
        }

        return comboBoxIdx;
    };

    int currentActionIdx = 0;
    // 按下电源按钮执行操作
    currentActionIdx = getEventComboBoxIdxFunc(ui->combo_powerButton, POWER_EVENT_PRESSED_POWEROFF);
    KLOG_INFO() << "power off:" << currentActionIdx;
    ui->combo_powerButton->setCurrentIndex(currentActionIdx == -1 ? 0 : currentActionIdx);

    // 按下挂起按钮执行操作
    currentActionIdx = getEventComboBoxIdxFunc(ui->combo_suspendButton, POWER_EVENT_PRESSED_SUSPEND);
    KLOG_INFO() << "suspend:" << currentActionIdx;
    ui->combo_suspendButton->setCurrentIndex(currentActionIdx == -1 ? 0 : currentActionIdx);

    // 计算机模式
    if( m_powerprofiles->isValid() )
    {
        QSignalBlocker blocker(ui->combo_computerMode);
        ui->combo_computerMode->setCurrentText(m_powerprofiles->activeProfile());
    }

    // 关闭盖子执行操作
    currentActionIdx = getEventComboBoxIdxFunc(ui->combo_closingLid, POWER_EVENT_LID_CLOSED);
    ui->combo_closingLid->setCurrentIndex(currentActionIdx == -1 ? 0 : currentActionIdx);

    // 显示器亮度调整
    auto monitorBrightnessReply = m_powerInterface->GetBrightness(POWER_DEVICE_TYPE_MONITOR);
    auto monitorBrightnessPercent = monitorBrightnessReply.value();
    if (monitorBrightnessReply.isError())
    {
        KLOG_WARNING() << "can't get monitor brightness!" << monitorBrightnessReply.error();
    }
    else
    {
        // 亮度为-1表示亮度调整不可用
        QSignalBlocker sliderBrightnessBlock(ui->slider_brightness);
        if (monitorBrightnessPercent == -1)
        {
            ui->slider_brightness->setMaximum(ui->slider_brightness->maximum());
            ui->slider_brightness->setEnabled(false);
        }
        else
        {
            setBrightnessPercent(monitorBrightnessPercent);
        }
    }

    // 色温
    if (m_kwinColorCorrect->isValid())
    {
        QSignalBlocker autoColorTempBlocker(m_switchAutoColorTemp);
        QSignalBlocker colorTempSliderBlocker(ui->slider_colorTemp);

        auto mode = m_kwinColorCorrect->getMode();

        m_switchAutoColorTemp->setChecked(mode == KWinColorCorrect::MODE_AUTOMATIC);
        ui->widget_manualColorTemp->setVisible(mode != KWinColorCorrect::MODE_AUTOMATIC);
        auto percent = m_kwinColorCorrect->getPercent();
        ui->slider_colorTemp->setValue(percent * 100);

        KLOG_INFO() << "kwin color correct: " << mode << percent;
    }

    // 多久判定为空闲
    QSignalBlocker sliderIdleTimeBlocker(ui->slider_idleTime);
    if (m_sessionSettings)
    {
        int idleTime = m_sessionSettings->get(KEY_IDLE_DELAY).toInt();
        updateIdleTimeLabel(idleTime);
        ui->slider_idleTime->setValue(idleTime);
    }
    else
    {
        ui->slider_idleTime->setEnabled(false);
    }

    // 空闲时是否锁定屏幕
    QSignalBlocker idleLockScreenBlocker(m_switchIdlelockScreen);
    if (m_screensaverSettings)
    {
        bool idleActivationLock = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
        m_switchIdlelockScreen->setChecked(idleActivationLock);
        m_switchIdlelockScreen->setCheckable(true);
    }
    else
    {
        m_switchIdlelockScreen->setChecked(false);
        m_switchIdlelockScreen->setCheckable(false);
    }

    // 待机唤醒时需要输入密码
    QSignalBlocker suspendLockScreenBlocker(m_switchSuspendLockScreen);
    auto lockwhenSuspend = m_powerInterface->screenLockedWhenSuspend();
    m_switchSuspendLockScreen->setChecked(lockwhenSuspend);
}

void GeneralSettingsPage::onEventActionChanged(int idx)
{
    PowerEvent event;
    PowerAction action;

    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox == ui->combo_powerButton)
    {
        event = POWER_EVENT_PRESSED_POWEROFF;
    }
    else if (comboBox == ui->combo_suspendButton)
    {
        event = POWER_EVENT_PRESSED_SUSPEND;
    }
    else if (comboBox == ui->combo_closingLid)
    {
        event = POWER_EVENT_LID_CLOSED;
    }
    else
    {
        qWarning() << "can't get power event enum!" << comboBox;
        return;
    }

    action = (PowerAction)comboBox->itemData(idx).toInt();
    auto reply = m_powerInterface->SetEventAction(event, action);
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "can't set event action,"
                       << reply.error()
                       << "event:" << event
                       << "action:" << action;
    }
}

void GeneralSettingsPage::onBrightnessSettingTimerTimeout()
{
    auto reply = m_powerInterface->SetBrightness(POWER_DEVICE_TYPE_MONITOR, m_brightnessValue);
    reply.waitForFinished();
    if (reply.isError())
    {
        KiranMessageBox::message(this, tr("ERROR"), reply.error().message(), KiranMessageBox::Ok);
        qWarning() << reply.error();
    }
}

void GeneralSettingsPage::onSliderBrightnessValueChanged(int value)
{
    setBrightnessPercent(value);
    m_brightnessSettingTimer.start();
}

void GeneralSettingsPage::onSliderIdleTimeValueChanged(int value)
{
    updateIdleTimeLabel(value);
    m_idleTimeSettingTimer.start();
}

void GeneralSettingsPage::onIdleTimeSettingTimerTimeout()
{
    if (m_sessionSettings)
    {
        int value = ui->slider_idleTime->value();
        m_sessionSettings->set(KEY_IDLE_DELAY, value);
    }
}

void GeneralSettingsPage::onSwitchIdleLockToggled(bool checked)
{
    if (m_screensaverSettings)
    {
        m_screensaverSettings->set(KEY_IDLE_ACTIVATION_LOCK, checked);
    }
}

void GeneralSettingsPage::onSwitchSuspendLockToggled(bool checked)
{
    m_powerInterface->LockScreenWhenSuspend(checked);
}

void GeneralSettingsPage::onSwitchAutoColorTempToggoled(bool checked)
{
    if (checked)
    {
        if (!m_kwinColorCorrect->setAutomaticMode())
        {
            KLOG_WARNING() << "set kwin color correct automatic mode failed";
            return;
        }
        KLOG_DEBUG() << "kwin set auto color temp:" << checked << "success";
        ui->widget_manualColorTemp->setVisible(false);
    }
    else
    {
        m_kwinColorCorrect->setManualMode(m_kwinColorCorrect->getPercent());
        ui->widget_manualColorTemp->setVisible(true);
    }
}

void GeneralSettingsPage::onColorTempSettingTimerTimeout()
{
    auto colorTemp = ui->slider_colorTemp->value();
    double percent = colorTemp / 100.0;
    m_kwinColorCorrect->setManualMode(percent);
}

void GeneralSettingsPage::onSliderColorTempValueChanged(int value)
{
    m_colorTempSettingTimer.start();
}

void GeneralSettingsPage::onComboComputerModeChanged(const QString& text)
{
    m_powerprofiles->setActiveProfile(text);
}

void GeneralSettingsPage::setBrightnessPercent(int percent)
{
    if (percent < ui->slider_brightness->minimum())
    {
        percent = ui->slider_brightness->minimum();
    }

    ui->slider_brightness->setValue(percent);
    m_brightnessValue = percent;
    ui->label_brightnessPercent->setText(QString("%1%").arg(percent));
}

QSize GeneralSettingsPage::sizeHint() const
{
    return {500, 657};
}

void GeneralSettingsPage::updateIdleTimeLabel(int min)
{
    QString idleTime;

    int hour = min / 60;
    int minute = min % 60;
    QStringList temp;
    if (hour)
    {
        temp.append(tr("%1hour").arg(hour));
    }
    if (minute)
    {
        temp.append(tr("%1minute").arg(minute));
    }
    idleTime = temp.join(" ");

    ui->label_idleTime->setText(idleTime);
}
