/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-timedate is licensed under Mulan PSL v2.
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

// You may need to build the project (run Qt uic code generator) to get "ui_display-format-settings.h" resolved

#include "display-format-settings.h"
#include "kiran-timedate-global-data.h"
#include "ui_display-format-settings.h"

#include <kiran-switch-button.h>
#include <kiran-system-daemon/timedate_i.h>

DisplayFormatSettings::DisplayFormatSettings(QWidget* parent) : QWidget(parent), ui(new Ui::DisplayFormatSettings)
{
    ui->setupUi(this);
    init();
}

DisplayFormatSettings::~DisplayFormatSettings()
{
    delete ui;
}

void DisplayFormatSettings::init()
{
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();

    m_showSecondSwitch = new KiranSwitchButton(this);
    ui->layout_timeSeconds->addWidget(m_showSecondSwitch);

    /* ComboBox填充数据 */
    char exampleDateTimeBuf[1024] = {0};
    time_t exampleTime = 0;
    struct tm* exampleTm = gmtime(&exampleTime);

    QStringList longDateFormatList = globalData->longDateFormatList();
    for (auto format : longDateFormatList)
    {
        std::string stdStrFormat = format.toStdString();
        strftime(exampleDateTimeBuf, sizeof(exampleDateTimeBuf), stdStrFormat.c_str(), exampleTm);
        ui->combo_longDateFormat->addItem(exampleDateTimeBuf, format);
    }

    QStringList shortDateFormatList = globalData->shortDateFormatList();
    for (auto format : shortDateFormatList)
    {
        std::string stdStrFormat = format.toStdString();
        strftime(exampleDateTimeBuf, sizeof(exampleDateTimeBuf), stdStrFormat.c_str(), exampleTm);
        ui->combo_shortDateFormat->addItem(exampleDateTimeBuf, format);
    }

    ui->combo_timeFormat->addItem(tr("24-hours"), TIMEDATE_HOUSR_FORMAT_24_HOURS);
    ui->combo_timeFormat->addItem(tr("12-hours"), TIMEDATE_HOUSR_FORMAT_12_HOURS);

    /* 设置当前值 */
    int idx = globalData->longDateFormatIndex();
    if (idx >= 0 && idx < ui->combo_longDateFormat->count())
    {
        ui->combo_longDateFormat->setCurrentIndex(idx);
    }

    idx = globalData->shortDateFormatIndex();
    if (idx >= 0 && idx < ui->combo_shortDateFormat->count())
    {
        ui->combo_shortDateFormat->setCurrentIndex(idx);
    }

    TimedateHourFormat hourFormat = globalData->hourFormat();
    idx = ui->combo_timeFormat->findData(hourFormat);
    ui->combo_timeFormat->setCurrentIndex(idx);

    bool secondsShowing = globalData->secondsShowing();
    m_showSecondSwitch->setChecked(secondsShowing);

    /* 连接信号槽 */
    connect(ui->combo_longDateFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DisplayFormatSettings::handleLongDisplayFormatChanged);
    connect(globalData, &KiranTimeDateGlobalData::longDateFormatIndexChanged, this, &DisplayFormatSettings::handleSystemLongDisplayFormatChanged);

    connect(ui->combo_shortDateFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DisplayFormatSettings::handleShortgDisplayFormatChanged);
    connect(globalData, &KiranTimeDateGlobalData::shortDateFormatIndexChanged, this, &DisplayFormatSettings::handleSystemShortDisplayFormatChanged);

    connect(ui->combo_timeFormat, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DisplayFormatSettings::handleHourFormatChanged);
    connect(globalData, &KiranTimeDateGlobalData::hourFormatChanged, this, &DisplayFormatSettings::handleSystemHourFormatChanged);

    connect(m_showSecondSwitch, &KiranSwitchButton::toggled, this, &DisplayFormatSettings::handleTimeSecondVisibleChanged);
    connect(globalData, &KiranTimeDateGlobalData::secondsShowingChanged, this, &DisplayFormatSettings::handleSystemTimeSecondVisibleChanged);
}

void DisplayFormatSettings::handleLongDisplayFormatChanged(int idx)
{
    QPair<bool, QString> reply = TimeDate::instance()->SyncSetDateFormatByIndex(TIMEDATE_FORMAT_TYPE_LONG, idx);
    if (!reply.first)
    {
        qWarning() << "set long date format failed," << reply.second;
        ui->combo_longDateFormat->setCurrentIndex(KiranTimeDateGlobalData::instance()->longDateFormatIndex());
    }
}

void DisplayFormatSettings::handleSystemLongDisplayFormatChanged(int idx)
{
    if (idx >= 0 && idx < ui->combo_longDateFormat->count())
    {
        ui->combo_longDateFormat->setCurrentIndex(idx);
    }
}

void DisplayFormatSettings::handleShortgDisplayFormatChanged(int idx)
{
    QPair<bool, QString> reply = TimeDate::instance()->SyncSetDateFormatByIndex(TIMEDATE_FORMAT_TYPE_SHORT, idx);
    if (!reply.first)
    {
        qWarning() << "set short date formate failed," << reply.second;
        ui->combo_shortDateFormat->setCurrentIndex(KiranTimeDateGlobalData::instance()->shortDateFormatIndex());
    }
}

void DisplayFormatSettings::handleSystemShortDisplayFormatChanged(int idx)
{
    if (idx >= 0 && idx < ui->combo_shortDateFormat->count())
    {
        ui->combo_shortDateFormat->setCurrentIndex(idx);
    }
}

void DisplayFormatSettings::handleHourFormatChanged(int idx)
{
    QVariant hourFormatVar = ui->combo_timeFormat->itemData(idx);
    int hourFormat = hourFormatVar.toInt();
    QPair<bool, QString> reply = TimeDate::instance()->SyncSetHourFormat(hourFormat);
    if (!reply.first)
    {
        qWarning() << "set hour formate failed," << reply.second;
        ui->combo_timeFormat->setCurrentIndex(KiranTimeDateGlobalData::instance()->hourFormat());
    }
}

void DisplayFormatSettings::handleSystemHourFormatChanged(TimedateHourFormat format)
{
    //更新日期时间格式设置界面
    int idx = ui->combo_timeFormat->findData(format);
    if (idx != -1)
    {
        ui->combo_timeFormat->setCurrentIndex(idx);
    }
}

void DisplayFormatSettings::handleTimeSecondVisibleChanged(bool enable)
{
    QPair<bool, QString> reply = TimeDate::instance()->SyncEnableSecondsShowing(enable);
    if (!reply.first)
    {
        qWarning() << "set seconds showing failed," << reply.second;
        m_showSecondSwitch->setChecked(KiranTimeDateGlobalData::instance()->secondsShowing());
    }
}

void DisplayFormatSettings::handleSystemTimeSecondVisibleChanged(bool enable)
{
    m_showSecondSwitch->setChecked(enable);
}
