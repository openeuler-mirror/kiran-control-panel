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
#include "timedate-subitem.h"
#include <QEvent>
#include <QMap>

TimeDateSubItem::TimeDateSubItem(QObject* parent)
    : QObject(parent)
{
}

TimeDateSubItem::~TimeDateSubItem()
{
}

bool TimeDateSubItem::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_timedateWidget && event->type() == QEvent::Destroy)
    {
        m_timedateWidget = nullptr;
    }

    return QObject::eventFilter(watched, event);
}

QString TimeDateSubItem::getID()
{
    return "TimeDate";
}

QString TimeDateSubItem::getName()
{
    return tr("Time Date Settings");
}

QString TimeDateSubItem::getCategory()
{
    return "timedate";
}

QString TimeDateSubItem::getDesc()
{
    return "";
}

QString TimeDateSubItem::getIcon()
{
    return "ksvg-kcp-timedate";
}

int TimeDateSubItem::getWeight()
{
    return 999;
}

QWidget* TimeDateSubItem::createWidget()
{
    m_timedateWidget = new KiranTimeDateWidget();
    m_timedateWidget->installEventFilter(this);
    return m_timedateWidget;
}

QVector<QPair<QString, QString>> TimeDateSubItem::getSearchKeys()
{
    return {{tr("Chnage time Zone"), "ChangeTimeZone"},
            {tr("Set time Manually"), "SetTimeManually"},
            {tr("Time date format setting"), "TimeDateFormatSetting"}};
}

bool TimeDateSubItem::jumpToSearchEntry(const QString& key)
{
    if (!m_timedateWidget)
        return false;

    static QMap<QString, KiranTimeDateStackPageEnum> pageMap = {
        {"ChangeTimeZone", PAGE_TIMEZONE_SETTING},
        {"SetTimeManually", PAGE_DATETIME_SETTING},
        {"TimeDateFormatSetting", PAGE_DISPLAY_FORMAT_SETTING}};

    auto iter = pageMap.find(key);
    if (iter == pageMap.end())
    {
        return false;
    }

    m_timedateWidget->jumpToSubPage(iter.value());
    return true;
}