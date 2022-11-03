#include "appearance-plugin.h"
#include "appearance-subitem.h"
#include "config.h"

#include "pages/font/fonts.h"
#include "pages/theme/themes.h"
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
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(APPEARANCE_DBUS_NAME).value())
    {
        KLOG_ERROR() << APPEARANCE_DBUS_NAME << "service isn't registered!";
        return false;
    }

    m_panelInterface = interface;
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-appearance",
                            ".",
                            TRANSLATE_PREFIX,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel appearance load translation failed";
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }

    initSubItem();
    return 0;
}

void AppearancePlugin::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
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
        return new Themes();
    };

    struct SubItemStruct
    {
        QString id;
        QString name;
        QString category;
        QString desc;
        QString icon;
        int weight;
        CreateWidgetFunc func;
    };
    QList<SubItemStruct> subitemInfos = {
        {"Themes",
         tr("Theme"),
         "individuation",
         "",
         "kcp-appearance-themes",
         99,
         themeSubItemCreater},
        {"Wallpaper",
         tr("Wallpaper"),
         "individuation",
         "",
         "kcp-appearance-wallpaper",
         98,
         wallpaperSubItemCreater},
        {"Fonts",
         tr("Font"),
         "individuation",
         "",
         "kcp-appearance-font",
         97,
         fontSubItemCreater}};

    for (const SubItemStruct& subitemInfo : subitemInfos)
    {
        AppearanceSubItem* subitem = new AppearanceSubItem(subitemInfo.func);
        subitem->setID(subitemInfo.id);
        subitem->setName(subitemInfo.name);
        subitem->setCategory(subitemInfo.category);
        subitem->setDesc(subitemInfo.desc);
        subitem->setIcon(subitemInfo.icon);
        subitem->setWeight(subitemInfo.weight);
        m_subitems.append(KiranControlPanel::SubItemPtr(subitem));
    }
}