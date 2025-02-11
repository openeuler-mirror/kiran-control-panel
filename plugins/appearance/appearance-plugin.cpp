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
#include "appearance-plugin.h"
#include "config.h"
#include "kiran-appearance-service-wrapper.h"
#include "logging-category.h"
#include "plugin-subitem.h"

#include "pages/font/fonts.h"
#include "pages/theme/theme-page.h"
#include "pages/wallpaper/wallpaper.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <tuple>

AppearancePlugin::AppearancePlugin(QObject* parent)
    : QObject(parent)
{
}

AppearancePlugin::~AppearancePlugin()
{
}

int AppearancePlugin::init(KiranControlPanel::PanelInterface* interface)
{
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(DBusWrapper::KiranAppearanceServiceName))
    {
        KLOG_ERROR(qLcAppearance) << DBusWrapper::KiranAppearanceServiceName << "service isn't registered!";
        return false;
    }

    m_panelInterface = interface;

    initSubItem();
    return 0;
}

void AppearancePlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> AppearancePlugin::getSubItems()
{
    return m_subitems;
}

void AppearancePlugin::initSubItem()
{
    auto wallpaperSubItemCreater = []() -> QWidget*
    {
        return new Wallpaper();
    };
    auto fontSubItemCreater = []() -> QWidget*
    {
        return new Fonts();
    };
    auto themeSubItemCreater = []() -> QWidget*
    {
        return new ThemePage();
    };

    m_subitems = {
        KiranControlPanel::SubItemPtr(new PluginSubItem("Themes",
                                                        tr("Theme"),
                                                        "individuation",
                                                        "",
                                                        "ksvg-kcp-appearance-themes",
                                                        99,
                                                        themeSubItemCreater)),
        KiranControlPanel::SubItemPtr(new PluginSubItem("Wallpaper",
                                                        tr("Wallpaper"),
                                                        "individuation",
                                                        "",
                                                        "ksvg-kcp-appearance-wallpaper",
                                                        98,
                                                        wallpaperSubItemCreater)),
        KiranControlPanel::SubItemPtr(new PluginSubItem("Fonts",
                                                        tr("Font"),
                                                        "individuation",
                                                        "",
                                                        "ksvg-kcp-appearance-font",
                                                        97,
                                                        fontSubItemCreater))};
}