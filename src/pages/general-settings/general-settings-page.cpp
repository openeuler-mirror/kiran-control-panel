//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_general-settings-page.h" resolved

#include "general-settings-page.h"
#include <tuple>
#include "kiran-message-box.h"
#include "kiran-session-daemon/power_i.h"
#include "log.h"
#include "power.h"
#include "ui_general-settings-page.h"

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
    initUI();
    initConnection();
    load();
}

#include <QListView>
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
    ui->slider_brightness->setMinimum(0);
    m_brightnessTimer.setInterval(300);
    m_brightnessTimer.setSingleShot(true);

    ///初始化特殊选项
    //关闭盖子选项
#ifndef TEST
    bool lidIsPresent = m_powerInterface->lid_is_present();
    ui->widget_lidLabel->setVisible(lidIsPresent);
    ui->widget_lidCombo->setVisible(lidIsPresent);
#endif
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
    connect(ui->slider_brightness, &QSlider::valueChanged, [&](int value) {
        setBrightnessPercent(value);
        m_brightnessTimer.start();
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
            LOG_WARNING("get event(%d) action failed,%s", event, errMsg.toStdString().c_str());
            return -1;
        }

        action = getEventActionReply.value();
        comboBoxIdx = comboBox->findData(action);
        if (comboBoxIdx == -1)
        {
            LOG_WARNING_S() << "combobox(" << comboBox->objectName() << ") can't find this action(" << action << ")!";
            return -1;
        }

        return comboBoxIdx;
    };

    /// press poweroff button action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_powerButton, POWER_EVENT_PRESSED_POWEROFF);
    LOG_INFO_S() << "power off:" << comboBoxIdx;
    ui->combo_powerButton->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// press suspend button action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_suspendButton, POWER_EVENT_PRESSED_SUSPEND);
    LOG_INFO_S() << "suspend:" << comboBoxIdx;
    ui->combo_suspendButton->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// close lid action
    comboBoxIdx = getEventComboBoxIdxFunc(ui->combo_closingLid, POWER_EVENT_LID_CLOSED);
    ui->combo_closingLid->setCurrentIndex(comboBoxIdx == -1 ? 0 : comboBoxIdx);

    /// monitor brightness
    auto monitorBrightnessReply = m_powerInterface->GetBrightness(POWER_DEVICE_TYPE_MONITOR);
    monitorBrightnessReply.waitForFinished();
    if (monitorBrightnessReply.isError())
    {
        LOG_WARNING_S() << "can't get monitor brightness!" << monitorBrightnessReply.error();
    }
    else
    {
        monitorBrightnessPercent = monitorBrightnessReply.value();
        if (monitorBrightnessPercent == -1)
        {
            //NOTE:亮度为-1表示亮度调整不可用
            monitorBrightnessPercent = 0;
#ifndef TEST
            ui->slider_brightness->setEnabled(false);
#endif
        }
        else if (monitorBrightnessPercent > 100 || monitorBrightnessPercent < -1)
        {
            LOG_WARNING_S() << "invalid monitor brightness" << monitorBrightnessPercent;
            monitorBrightnessPercent = 0;
        }
    }
    ui->slider_brightness->blockSignals(true);
    setBrightnessPercent(monitorBrightnessPercent);
    ui->slider_brightness->blockSignals(false);
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
        LOG_WARNING_S() << "can't set event action,"
                        << reply.error()
                        << "event:" << event
                        << "action:" << action;
    }
}

void GeneralSettingsPage::setBrightnessPercent(quint16 percent)
{
    setBrightnessPercentLabel(percent);
    ui->slider_brightness->setValue(percent);
}

void GeneralSettingsPage::setBrightnessPercentLabel(quint16 percent)
{
    m_brightnessValue = percent;
    ui->label_brightnessPercent->setText(QString::number(percent) + "%");
}
