/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#include "page.h"
#include <qt5-log-i.h>
#include <NetworkManagerQt/Settings>
Page::Page(QWidget *parent) : QWidget(parent)
{
}

Page::~Page()
{
}

void Page::initNotifierConnection()
{
    connect(notifier(), &Notifier::activeConnectionAdded, [=](const QString &path) {
        KLOG_DEBUG() << "activeConnectionAdded:" << path;
    });

    connect(notifier(), &Notifier::activeConnectionRemoved, [=](const QString &path) {
        KLOG_DEBUG() << "activeConnectionRemoved:" << path;
//        refreshConnectionLists();
        handleNotifierConnectionChanged();
    });

    connect(settingsNotifier(), &SettingsNotifier::connectionRemoved, [=](const QString &path) {
        KLOG_DEBUG() << "SettingsNotifier::connectionRemoved:" << path;
        handleNotifierConnectionChanged();
    });

    connect(settingsNotifier(), &SettingsNotifier::connectionAdded, [=](const QString &path) {
        handleNotifierConnectionChanged();
    });
}

void Page::refreshConnectionLists()
{
    KLOG_DEBUG() << "Page::refreshConnectionLists()";
}

void Page::handleNotifierConnectionChanged()
{
    KLOG_DEBUG() << "Page::handleNotifierConnectionChanged()";
}
