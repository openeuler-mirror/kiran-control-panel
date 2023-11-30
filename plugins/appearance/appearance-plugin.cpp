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
                                                        "kcp-appearance-themes",
                                                        99,
                                                        themeSubItemCreater)),
        KiranControlPanel::SubItemPtr(new PluginSubItem("Wallpaper",
                                                        tr("Wallpaper"),
                                                        "individuation",
                                                        "",
                                                        "kcp-appearance-wallpaper",
                                                        98,
                                                        wallpaperSubItemCreater)),
        KiranControlPanel::SubItemPtr(new PluginSubItem("Fonts",
                                                        tr("Font"),
                                                        "individuation",
                                                        "",
                                                        "kcp-appearance-font",
                                                        97,
                                                        fontSubItemCreater))
    };
}