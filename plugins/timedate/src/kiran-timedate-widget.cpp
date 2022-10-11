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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "kiran-timedate-widget.h"
#include "kiran-timedate-global-data.h"
#include "mask-widget.h"
#include "ui_kiran-timedate-widget.h"

#include "date-time-settings/date-time-settings.h"
#include "display-format-settings/display-format-settings.h"
#include "timezone-settings/time-zone-settings.h"

#include <kiran-switch-button.h>
#include <qt5-log-i.h>
#include <widget-property-helper.h>
#include <QDateTime>
#include <QDebug>
#include <QFontDatabase>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QTimerEvent>

enum KiranTimeDateStackPageEnum
{
    PAGE_TIMEZONE_SETTING,
    PAGE_DATETIME_SETTING,
    PAGE_DISPLAY_FORMAT_SETTING,
    PAGE_END
};

KiranTimeDateWidget::KiranTimeDateWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::KiranTimeDateWidget),
      m_updateTimer(0),
      m_maskWidget(new MaskWidget(this))
{
    ui->setupUi(this);
    initUI();
    m_updateTimer = startTimer(1000);
}

KiranTimeDateWidget::~KiranTimeDateWidget()
{
    delete ui;
}

void KiranTimeDateWidget::initUI()
{
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();

    /// 显示时区和所在区域
    ui->label_utc->setContentsMargins(-1, 24, -1, -1);
    ui->label_dateTime->setContentsMargins(-1, 8, -1, -1);

    /// 自动同步
    m_autoSyncSwitch = new KiranSwitchButton(this);
    ui->widget_autoSync->layout()->addWidget(m_autoSyncSwitch);

    // 自动同步开关的触发，设置"手动设置时间"标签的enable,判断当前页并切换
    connect(m_autoSyncSwitch, &KiranSwitchButton::toggled,
            this, &KiranTimeDateWidget::handleAutoSyncToggled);

    connect(globalData, &KiranTimeDateGlobalData::systemNTPChanged,
            this, &KiranTimeDateWidget::handleSystemNTPChanged);
    connect(globalData, &KiranTimeDateGlobalData::systemCanNTPChanged,
            this, &KiranTimeDateWidget::handleSysntemCanNTPChanged);

    /* 初始化侧边栏Tab列表 */
    ui->tabList->setInvertIconPixelsEnable(true);
    ui->tabList->setIconSize(QSize(16, 16));
    connect(ui->tabList, &KiranSidebarWidget::itemSelectionChanged,
            this, &KiranTimeDateWidget::handleSidebarSelectionChanged);

    /// 时区设置
    initTimeZoneSettingsPage();

    /// 日期时间设置
    initDateTimeSettingsPage();
    connect(globalData, &KiranTimeDateGlobalData::longDateFormatIndexChanged,
            this, &KiranTimeDateWidget::handleSystemLongDisplayFormatChanged);
    connect(globalData, &KiranTimeDateGlobalData::secondsShowingChanged,
            this, &KiranTimeDateWidget::handleSystemSecondShowingChanged);
    connect(globalData, &KiranTimeDateGlobalData::hourFormatChanged,
            this, &KiranTimeDateWidget::handleSystemHourFormatChanged);
    /// 时间显示格式设置
    m_showSeconds = globalData->secondsShowing();
    m_curTimeDateFormat = globalData->longDateFormatList().at(KiranTimeDateGlobalData::instance()->longDateFormatIndex());
    m_hourFormat = globalData->hourFormat();
    initDisplayFormatSettingsPage();

    // 获取ntp是否可开启
    bool can_ntp = globalData->systemCanNTP();
    m_autoSyncSwitch->setChecked(false);
    m_autoSyncSwitch->setEnabled(can_ntp);
    if (can_ntp)
    {
        // 时钟同步状态
        bool ntpStatus = globalData->systemNTP();
        m_autoSyncSwitch->setChecked(ntpStatus);
    }

    /// 更新侧边栏时间文本和时区显示
    updateTimeZoneLabel();
    /// 时区更改时，修改时间文本和时区显示
    connect(globalData, &KiranTimeDateGlobalData::systemTimeZoneChanged, this, &KiranTimeDateWidget::handleSystemTimeZoneChanged);
    updateTimeLabel();
    /// 设置默认页
    ui->tabList->setCurrentRow(0);
}

void KiranTimeDateWidget::initTimeZoneSettingsPage()
{
    auto sideBarItem = new QListWidgetItem(ui->tabList);
    sideBarItem->setText(tr("Change Time Zone"));
    sideBarItem->setIcon(QIcon(":/kcp-timedate/images/time-zone.svg"));
    ui->tabList->addItem(sideBarItem);

    m_zoneSettingsPage = new TimezoneSettings(this);
    ui->stack->insertWidget(PAGE_TIMEZONE_SETTING, m_zoneSettingsPage);
}

void KiranTimeDateWidget::initDateTimeSettingsPage()
{
    auto sideBarItem = new QListWidgetItem(ui->tabList);
    sideBarItem->setText(tr("Set Time Manually"));
#if 0
    QIcon setTimeManualIcon;
    setTimeManualIcon.addPixmap(QPixmap(":/kcp-timedate-images/time.png"), QIcon::Normal);
    setTimeManualIcon.addPixmap(QPixmap(":/kcp-timedate-images/time_d.png"), QIcon::Disabled);
#endif
    sideBarItem->setIcon(QIcon(":/kcp-timedate/images/time.svg"));
    ui->tabList->addItem(sideBarItem);

    m_dateTimeSettingsPage = new DateTimeSettings(this);
    ui->stack->insertWidget(PAGE_DATETIME_SETTING, m_dateTimeSettingsPage);
}

void KiranTimeDateWidget::initDisplayFormatSettingsPage()
{
    auto sideBarItem = new QListWidgetItem(ui->tabList);
    sideBarItem->setText(tr("Time date format setting"));
    sideBarItem->setIcon(QIcon(":/kcp-timedate/images/time-format.svg"));
    ui->tabList->addItem(sideBarItem);

    m_formatSettingsPage = new DisplayFormatSettings(this);
    ui->stack->insertWidget(PAGE_DISPLAY_FORMAT_SETTING, m_formatSettingsPage);
}

void KiranTimeDateWidget::updateTimeLabel()
{
    QString dateTime;
    QDateTime curDateTime = QDateTime::currentDateTime();
    QTime curTime = curDateTime.time();
    std::string curDateFormat = m_curTimeDateFormat.toStdString();
    std::string curTimeFormat = m_hourFormat == TIMEDATE_HOUSR_FORMAT_12_HOURS ? "%I:%M" : "%H:%M";
    if (m_showSeconds)
    {
        curTimeFormat += ":%S";
    }

    std::string timeDateFormat = curDateFormat + " " + curTimeFormat;
    time_t rawtime;
    struct tm* info;
    char buffer[256] = {0};
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), timeDateFormat.c_str(), info);

    ui->label_dateTime->setText(buffer);
}

void KiranTimeDateWidget::updateTimeZoneLabel()
{
    ZoneInfo zoneInfo;
    KiranTimeDateGlobalData* globalData = KiranTimeDateGlobalData::instance();
    QString currentTimeZoneID = globalData->systemTimeZone();

    if (globalData->findZoneInfoByZoneID(currentTimeZoneID, zoneInfo))
    {
        QString city = zoneInfo.zone_city;
        QStringList splitRes = city.split('/');

        city = splitRes.last();
        int hour = qAbs(zoneInfo.zone_utc) / 3600;
        int minute = (qAbs(zoneInfo.zone_utc) - hour * 3600) / 60;
        QString utc = QString("UTC%1%2:%3")
                          .arg(zoneInfo.zone_utc >= 0 ? "+" : "-")
                          .arg(hour, 2, 10, QChar('0'))
                          .arg(minute, 2, 10, QChar('0'));
        QLocale systemLocale = QLocale::system();
        QString displayText = QString(tr("%1(%2)")).arg(city).arg(utc);
        ui->label_utc->setText(displayText);
    }
    else
    {
        ui->label_utc->setText("???");
    }
}

void KiranTimeDateWidget::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_updateTimer)
    {
        updateTimeLabel();
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void KiranTimeDateWidget::setMaskWidgetVisible(bool visible)
{
    m_maskWidget->setVisible(visible);
    if (visible)
    {
        this->stackUnder(m_maskWidget);
    }
}

void KiranTimeDateWidget::handleSidebarSelectionChanged()
{
    QList<QListWidgetItem*> selecteds = ui->tabList->selectedItems();
    if (selecteds.size() != 1)
    {
        qFatal("tabList: selecteds size != 1");
        return;
    }

    int row = ui->tabList->row(selecteds.at(0));
    ui->stack->setCurrentIndex(row);
    switch (row)
    {
    case PAGE_TIMEZONE_SETTING:
        m_zoneSettingsPage->reset();
        break;
    case PAGE_DATETIME_SETTING:
        m_dateTimeSettingsPage->reset();
        break;
    default:
        break;
    }
}

void KiranTimeDateWidget::handleAutoSyncToggled(bool checked)
{
    bool autoSyncEnable = checked;
    QListWidgetItem* dateTimeItem = ui->tabList->item(PAGE_DATETIME_SETTING);
    QWidget* dateTimeItemWidget = ui->tabList->itemWidget(dateTimeItem);
    if (autoSyncEnable != KiranTimeDateGlobalData::instance()->systemNTP())
    {
        QPair<bool, QString> setNtpRes;

        setMaskWidgetVisible(true);
        setNtpRes = ComKylinsecKiranSystemDaemonTimeDateInterface::instance()->SyncSetNTP(autoSyncEnable);
        setMaskWidgetVisible(false);

        if (!setNtpRes.first)
        {
            qWarning() << "SetNTP failed," << setNtpRes.second;
            m_autoSyncSwitch->setChecked(autoSyncEnable ? Qt::Unchecked : Qt::Checked);
            return;
        }
    }
    if (autoSyncEnable)
    {
        dateTimeItem->setFlags(dateTimeItem->flags() & (~Qt::ItemIsEnabled));
        if (ui->tabList->currentRow() == PAGE_DATETIME_SETTING)
        {
            ui->tabList->setCurrentRow(PAGE_TIMEZONE_SETTING);
        }
    }
    else
    {
        dateTimeItem->setFlags(dateTimeItem->flags() | Qt::ItemIsEnabled);
    }
}
void KiranTimeDateWidget::handleSystemNTPChanged(bool ntp)
{
    m_autoSyncSwitch->setChecked(ntp ? Qt::Checked : Qt::Unchecked);
}

void KiranTimeDateWidget::handleSysntemCanNTPChanged(bool canNtp)
{
    m_autoSyncSwitch->setEnabled(canNtp ? true : false);
}

void KiranTimeDateWidget::handleSystemTimeZoneChanged(QString timeZone)
{
    updateTimeZoneLabel();
}

void KiranTimeDateWidget::handleSystemLongDisplayFormatChanged(int idx)
{
    m_curTimeDateFormat = KiranTimeDateGlobalData::instance()->longDateFormatList().at(idx);
    updateTimeLabel();
}

void KiranTimeDateWidget::handleSystemSecondShowingChanged(bool enable)
{
    // 改变日期时间显示界面时钟下方显示
    m_showSeconds = enable;
    updateTimeLabel();
}

void KiranTimeDateWidget::handleSystemHourFormatChanged(TimedateHourFormat hourFormat)
{
    m_hourFormat = hourFormat;
    updateTimeLabel();
}

QSize KiranTimeDateWidget::sizeHint() const
{
    return {780, 650};
}
