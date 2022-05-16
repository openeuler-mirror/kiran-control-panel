/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-cpanel-power is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <QGSettings/QGSettings>
#include <QListView>
#include <QSignalBlocker>

#include "general-settings-page.h"
#include "kiran-message-box.h"
#include "kiran-session-daemon/power-i.h"
#include "power.h"
#include "ui_general-settings-page.h"

#define MAX_IDLE_TIME 120

#define MATE_SESSION_SCHEMA_ID "org.mate.session"
#define KIRAN_SESSION_SCHEMA_ID "com.kylinsec.kiran.session-manager"
//#define KEY_IDLE_DELAY "idle-delay"
#define KEY_IDLE_DELAY "idleDelay"
#define DEFAULT_IDLE_DELAY 5

#define SCHEMA_KIRAN_SCREENSAVER        "com.kylinsec.kiran.screensaver"
#define KEY_IDLE_ACTIVATION_LOCK        "idleActivationLock"

GeneralSettingsPage::GeneralSettingsPage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::GeneralSettingsPage),
      m_powerInterface(PowerInterface::instance())
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
        m_sessionSettings = new QGSettings(KIRAN_SESSION_SCHEMA_ID,QByteArray(),this);
    }
    else if (QGSettings::isSchemaInstalled(MATE_SESSION_SCHEMA_ID))
    {
        m_sessionSettings = new QGSettings(MATE_SESSION_SCHEMA_ID,QByteArray(),this);
    }

    if( QGSettings::isSchemaInstalled(SCHEMA_KIRAN_SCREENSAVER) )
    {
        m_screensaverSettings = new QGSettings(SCHEMA_KIRAN_SCREENSAVER,QByteArray(),this);
    }

    if (m_sessionSettings)
    {
        connect(m_sessionSettings, &QGSettings::changed, [this](const QString& key) {
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
            ui->slider_idleTime->setValue(m_sessionSettings->get(key).toInt());
        });
    }

    if(m_screensaverSettings)
    {
        connect(m_screensaverSettings,&QGSettings::changed,[this](const QString& key){
            if(key != KEY_IDLE_ACTIVATION_LOCK)
            {
                return ;
            }

            bool value = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
            m_btn_lockScreen->setChecked(value);
        });
    }
}

void GeneralSettingsPage::initUI()
{
    ///填充选项
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

    ///填充ComboBox
    for (auto& action : actions)
    {
        QComboBox* comboBox = action.first;
        for (const auto& actionIter : action.second)
        {
            comboBox->addItem(actionIter.first, actionIter.second);
        }
    }

    ///初始化QSlider,和延迟设置的Timer
    ui->slider_brightness->setMaximum(100);
    ui->slider_brightness->setMinimum(20);
    m_brightnessTimer.setInterval(300);
    m_brightnessTimer.setSingleShot(true);

    ui->slider_idleTime->setMaximum(MAX_IDLE_TIME);
    ui->slider_idleTime->setMinimum(1);
    m_idleTimeTimer.setInterval(300);
    m_idleTimeTimer.setSingleShot(true);

    ///初始化特殊选项
    //关闭盖子选项
#ifndef TEST
    bool lidIsPresent = m_powerInterface->lid_is_present();
    ui->widget_lidLabel->setVisible(lidIsPresent);
    ui->widget_lidCombo->setVisible(lidIsPresent);
#endif

    //空闲时是否锁定屏幕及屏保
    m_btn_lockScreen = new KiranSwitchButton(this);
    ui->layout_sliderLabel->addWidget(m_btn_lockScreen);

    //NOTE: 根据#48515缺陷单所做修改,隐藏挂起按钮相关配置项，后续若有需要再进行打开
    ui->layout_suspendlabel->setMargin(0);
    ui->label_suspend->setVisible(false);

    ui->layout_suspend->setMargin(0);
    ui->combo_suspendButton->setVisible(false);
}

void GeneralSettingsPage::initConnection()
{
    connect(ui->combo_powerButton, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::handleComboBoxCurrentIdxChanged);
    connect(ui->combo_suspendButton, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::handleComboBoxCurrentIdxChanged);
    connect(ui->combo_closingLid, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeneralSettingsPage::handleComboBoxCurrentIdxChanged);
    connect(&m_brightnessTimer, &QTimer::timeout, [&]() {
        auto reply = m_powerInterface->SetBrightness(POWER_DEVICE_TYPE_MONITOR, m_brightnessValue);
        reply.waitForFinished();
        if (reply.isError())
        {
            KiranMessageBox::message(this, tr("ERROR"), reply.error().message(), KiranMessageBox::Ok);
            qWarning() << reply.error();
        }
    });
    connect(ui->slider_brightness, &QSlider::valueChanged, [this](int value) {
        setBrightnessPercent(value);
        m_brightnessTimer.start();
    });
    connect(ui->slider_idleTime, &QSlider::valueChanged, [this](int value) {
        updateIdleTimeLabel(value);
        m_idleTimeTimer.start();
    });
    connect(&m_idleTimeTimer, &QTimer::timeout, [this]() {
        if (m_sessionSettings)
        {
            int value = ui->slider_idleTime->value();
            m_sessionSettings->set(KEY_IDLE_DELAY, value);
        }
    });
    connect(m_btn_lockScreen, &QAbstractButton::toggled, [this](bool checked) {
        m_screensaverSettings->set(KEY_IDLE_ACTIVATION_LOCK,checked);
    });
}

void GeneralSettingsPage::load()
{
    int monitorBrightnessPercent = 0, comboBoxIdx = 0;

    auto getEventComboBoxIdxFunc = [this](QComboBox* comboBox, PowerEvent event) -> int {
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

    /// press poweroff button action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_powerButton, POWER_EVENT_PRESSED_POWEROFF);
    KLOG_INFO() << "power off:" << comboBoxIdx;
    ui->combo_powerButton->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// press suspend button action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_suspendButton, POWER_EVENT_PRESSED_SUSPEND);
    KLOG_INFO() << "suspend:" << comboBoxIdx;
    ui->combo_suspendButton->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// close lid action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_closingLid, POWER_EVENT_LID_CLOSED);
    ui->combo_closingLid->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// monitor brightness
    auto monitorBrightnessReply = m_powerInterface->GetBrightness(POWER_DEVICE_TYPE_MONITOR);
    monitorBrightnessReply.waitForFinished();
    if (monitorBrightnessReply.isError())
    {
        KLOG_WARNING() << "can't get monitor brightness!" << monitorBrightnessReply.error();
    }
    else
    {
        //NOTE:亮度为-1表示亮度调整不可用
        monitorBrightnessPercent = monitorBrightnessReply.value();
    }
    QSignalBlocker signalBlocker(ui->slider_brightness);
    if( monitorBrightnessPercent == -1 )
    {
        ui->slider_brightness->setMaximum(ui->slider_brightness->maximum());
        ui->slider_brightness->setEnabled(false);
    }
    else
    {
        setBrightnessPercent(monitorBrightnessPercent);
    }

    /// idle time
    if (m_sessionSettings)
    {
        QSignalBlocker idleTimeSignalBlocker(ui->slider_idleTime);
        QSignalBlocker idleTimerSwitchBlocker(m_btn_lockScreen);
        int idleTime = m_sessionSettings->get(KEY_IDLE_DELAY).toInt();
        updateIdleTimeLabel(idleTime);
        ui->slider_idleTime->setValue(idleTime);
    }
    else
    {
        m_btn_lockScreen->setChecked(false);
        m_btn_lockScreen->setEnabled(false);
    }

    if(m_screensaverSettings)
    {
        QSignalBlocker qSignalBlocker(m_btn_lockScreen);
        bool value = m_screensaverSettings->get(KEY_IDLE_ACTIVATION_LOCK).toBool();
        m_btn_lockScreen->setChecked(value);
    }
    else
    {
        m_btn_lockScreen->setChecked(false);
    }

}

void GeneralSettingsPage::handleComboBoxCurrentIdxChanged(int idx)
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
    return QSize(518, 585);
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
