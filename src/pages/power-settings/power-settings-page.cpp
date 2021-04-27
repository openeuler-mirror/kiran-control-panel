//
// Created by lxh on 2021/4/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_power-settings-page.h" resolved

#include "power-settings-page.h"
#include <kiran-session-daemon/power_i.h>
#include "log.h"
#include "power.h"
#include "ui_power-settings-page.h"

PowerSettingsPage::PowerSettingsPage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::PowerSettingsPage),
      m_powerInterface(PowerInterface::instance())
{
    ui->setupUi(this);
    init();
}

PowerSettingsPage::~PowerSettingsPage()
{
    delete ui;
}

void PowerSettingsPage::init()
{
    initUI();
    initConnection();
    load();
}

void PowerSettingsPage::initUI()
{
    ///支持的空闲超时时间
    static QList<int> idleTimeouts = {60, 180, 300, 600, 900, 1200, 1800, 2700, 3600, 7200, 10800, 14400, 18000, 0};
    foreach (const int& timeout, idleTimeouts)
    {
        ui->combo_idleTime->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
        ui->combo_displayOffTime->addItem(getTimeDescription(timeout), QVariant::fromValue(timeout));
    }

    ///计算机空闲进行的操作
    // clang-format off
    QMap<QString, PowerAction> computerIdleActionsMap = {
        {tr("Display Off"), POWER_ACTION_DISPLAY_OFF},
        {tr("Suspend"), POWER_ACTION_COMPUTER_SUSPEND},
        {tr("Shutdown"), POWER_ACTION_COMPUTER_SHUTDOWN},
        {tr("Hibernate"), POWER_ACTION_COMPUTER_HIBERNATE},
        {tr("Do nothing"), POWER_ACTION_NOTHING}
    };
    // clang-format on
    for (auto iter = computerIdleActionsMap.begin();
         iter != computerIdleActionsMap.end();
         iter++)
    {
        ui->combo_idleAction->addItem(iter.key(), iter.value());
    }
}

void PowerSettingsPage::initConnection()
{
    connect(ui->combo_idleTime, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PowerSettingsPage::handleIdleTimeActionCurrentIdxChanged);
    connect(ui->combo_idleAction, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PowerSettingsPage::handleIdleTimeActionCurrentIdxChanged);
    connect(ui->combo_displayOffTime, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PowerSettingsPage::handleMonitorOffTimeoutCurrentIdxChanged);
}

void PowerSettingsPage::load()
{
    int findComboIdx;

    ///加载计算机空闲超时时间和操作
    {
        auto getComputerIdleTimeoutReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_COMPUTER,
                                                                           POWER_SUPPLY_MODE_AC);
        getComputerIdleTimeoutReply.waitForFinished();
        if (getComputerIdleTimeoutReply.isError())
        {
            LOG_WARNING_S() << "can't get computer idle timeout!," << getComputerIdleTimeoutReply.error();
        }
        else
        {
            IdleAction action = getComputerIdleTimeoutReply.value();
            findComboIdx = ui->combo_idleTime->findData(action.idleTimeout);
            if (findComboIdx == -1)
            {
                LOG_WARNING_S("can't find computer timeout(%d),append it to combobox!", action.idleTimeout);
                ui->combo_idleTime->addItem(getTimeDescription(action.idleTimeout), action.idleTimeout);
                findComboIdx = ui->combo_idleTime->findData(action.idleTimeout);
            }
            ui->combo_idleTime->blockSignals(true);
            ui->combo_idleTime->setCurrentIndex(findComboIdx);
            ui->combo_idleTime->blockSignals(false);

            findComboIdx = ui->combo_idleAction->findData(action.idleAction);
            if (findComboIdx == -1)
            {
                LOG_WARNING_S("can't find computer idle action(%d),append it to combobox!", action.idleAction);
                ui->combo_idleAction->addItem(QString::number(action.idleAction), action.idleAction);
                findComboIdx = ui->combo_idleAction->findData(action.idleAction);
            }
            ui->combo_idleAction->blockSignals(true);
            ui->combo_idleAction->setCurrentIndex(findComboIdx);
            ui->combo_idleAction->blockSignals(false);
        }
    }

    ///加载显示器空闲熄灭时间
    {
        auto getMonitorIdleTimeoutReply = m_powerInterface->GetIdleAction(POWER_DEVICE_TYPE_BACKLIGHT,
                                                                          POWER_SUPPLY_MODE_AC);
        getMonitorIdleTimeoutReply.waitForFinished();
        if (getMonitorIdleTimeoutReply.isError())
        {
            LOG_WARNING_S() << "can't get monitor idle timeout!," << getMonitorIdleTimeoutReply.error();
        }
        else
        {
            IdleAction action = getMonitorIdleTimeoutReply.value();
            findComboIdx = ui->combo_displayOffTime->findData(action.idleTimeout);
            if (findComboIdx == -1)
            {
                LOG_WARNING_S("can't find monitor idle timeout(%d),append it to combobox!", action.idleTimeout);
                ui->combo_displayOffTime->addItem(getTimeDescription(action.idleTimeout), action.idleTimeout);
                findComboIdx = ui->combo_displayOffTime->findData(action.idleTimeout);
            }
            ui->combo_displayOffTime->blockSignals(true);
            ui->combo_displayOffTime->setCurrentIndex(findComboIdx);
            ui->combo_displayOffTime->blockSignals(false);
        }
    }
}

QString PowerSettingsPage::getTimeDescription(int seconds)
{
    static const int minute_unit = 60;
    static const int hour_unit = minute_unit * 60;
    static const int day_unit = hour_unit * 24;

    int day = seconds / day_unit;
    int hour = (seconds - (day * day_unit)) / hour_unit;
    int minute = (seconds - (day * day_unit) - (hour * hour_unit)) / minute_unit;

    QString formatTime;
    if (day != 0)
    {
        formatTime.append(QString(tr("%1Day")).arg(day));
    }
    if (hour != 0)
    {
        formatTime.append(QString(tr("%1Hour")).arg(hour));
    }
    if (minute != 0)
    {
        formatTime.append(QString("%1Minute").arg(minute));
    }
    if (formatTime.isEmpty())
    {
        formatTime.append(tr("never"));
    }
    return formatTime;
}

bool PowerSettingsPage::getCurrentIdleAction(IdleAction& idleAction)
{
    bool toIntOk = false;

    int timeout = ui->combo_idleTime->currentData().toInt(&toIntOk);
    if (!toIntOk)
    {
        LOG_WARNING_S() << "combobox " << ui->combo_idleTime->objectName()
                        << "item data can't convert to int," << ui->combo_idleTime->currentData();
        return false;
    }

    int action = (PowerAction)ui->combo_idleAction->currentData().toInt(&toIntOk);
    if (!toIntOk)
    {
        LOG_WARNING_S() << "combobox " << ui->combo_idleAction->objectName()
                        << "imet data can't convert to int," << ui->combo_idleAction->currentData();
        return false;
    }

    idleAction.idleTimeout = timeout;
    idleAction.idleAction = action;

    return true;
}

void PowerSettingsPage::handleIdleTimeActionCurrentIdxChanged(int idx)
{
    IdleAction idleAction{};
    if (!getCurrentIdleAction(idleAction))
    {
        return;
    }

    auto reply = m_powerInterface->SetIdleAction(POWER_DEVICE_TYPE_COMPUTER,
                                                 POWER_SUPPLY_MODE_AC,
                                                 idleAction.idleTimeout,
                                                 idleAction.idleAction);
    if (reply.isError())
    {
        LOG_WARNING_S() << "set idle action for computer failed" << reply.error();
    }
}

void PowerSettingsPage::handleMonitorOffTimeoutCurrentIdxChanged(int idx)
{
    bool isOk = false;
    int displayOffTime = ui->combo_displayOffTime->currentData().toInt(&isOk);
    auto reply = m_powerInterface->SetIdleAction(POWER_DEVICE_TYPE_BACKLIGHT,
                                                 POWER_SUPPLY_MODE_AC,
                                                 displayOffTime,
                                                 POWER_ACTION_DISPLAY_OFF);
    reply.waitForFinished();
    if (reply.isError())
    {
        LOG_WARNING_S() << "set idle action for monitor failed," << reply.error();
    }
}