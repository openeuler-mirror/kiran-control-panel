//
// Created by lxh on 2021/5/7.
//

#include "plugin-info.h"
#include "config.h"

#include <glib.h>
#include <qt5-log-i.h>
#include <QApplication>
#include <QFileInfo>
#include <QLocale>
#include <QTranslator>

///[Desktop Entry]
#define GROUP_DESKTOP_ENTRY "Desktop Entry"
#define KEY_NAME "Name"
#define KEY_COMMENT "Comment"
#define KEY_ICON "Icon"

///[Kiran Control Panel Plugin]
#define GROUP_KIRAN_CONTROL_PANEL_PLUGIN "Kiran Control Panel Plugin"
#define KEY_WEIGHT "Weight"
#define KEY_CATEGORY "Category"
#define KEY_LIBRARY "Library"
#define KEY_SUBITEMS "SubItems"

///subitem
#define KEY_SUBITEM_ID "Id"
#define KEY_SUBITEM_NAME "Name"
#define KEY_SUBITEM_ICON "Icon"
#define KEY_SUBITEM_KEYWORDS "Keywords"

QSharedPointer<CPanelPluginHelper> CPanelPluginHelper::loadPlugin(const QString& desktop)
{
    QSharedPointer<CPanelPluginHelper> pSharedPluginHelper(new CPanelPluginHelper);
    if (!pSharedPluginHelper->load(desktop))
    {
        KLOG_ERROR_S() << "can't load plugin desktop <" << desktop << ">!";
        return nullptr;
    }
    return pSharedPluginHelper;
}

CPanelPluginHelper::~CPanelPluginHelper()
{
    clear();
}

bool CPanelPluginHelper::load(const QString& desktopFile)
{
    clear();

    if (!QFileInfo::exists(desktopFile))
    {
        KLOG_ERROR_S() << "can't parse desktop file," << desktopFile << "is not exist!";
        return false;
    }

    PluginDesktopInfo desktopInfo;
    //解析插件Desktop中的信息
    if (!parseDesktopInfo(desktopFile, desktopInfo))
    {
        KLOG_ERROR_S() << "parser desktop failed!," << desktopFile;
    }
    //加载插件共享库
    KcpPluginInterface* pluginInterface = nullptr;
    if (!loadLibrary(desktopInfo.library, pluginInterface))
    {
        KLOG_ERROR_S() << "load library failed!" << desktopInfo.library;
        return false;
    }
    //通过插件接口获取应该显示的功能项和Desktop中的信息取交集
    QStringList visibleSubItems = pluginInterface->visibleSubItems();
    QList<PluginSubItemInfo> newSubItems;
    foreach (auto subitem, desktopInfo.subItems)
    {
        if (visibleSubItems.contains(subitem.id))
        {
            newSubItems.append(subitem);
        }
    }
    desktopInfo.subItems.swap(newSubItems);
    m_pluginDesktopInfo = desktopInfo;
    m_pluginInterface = pluginInterface;
    return true;
}

const PluginDesktopInfo& CPanelPluginHelper::getPluginDesktopInfo()
{
    return m_pluginDesktopInfo;
}

bool CPanelPluginHelper::loadLibrary(const QString& library,
                                     KcpPluginInterface*& pluginInterface)
{
    QFileInfo libraryInfo(library);
    if (!libraryInfo.exists())
    {
        KLOG_ERROR_S() << "can't find library(" << library << ")!";
        return false;
    }

    m_pluginHandle.setFileName(library);
    if (!m_pluginHandle.load())
    {
        KLOG_ERROR_S() << "can't load plugin," << m_pluginHandle.errorString();
    }
    if (!m_pluginHandle.isLoaded())
    {
        KLOG_ERROR_S() << "can't load library,"
                       << m_pluginHandle.errorString()
                       << "," << library;
        return false;
    }
    KcpPluginInterface* pInterface = qobject_cast<KcpPluginInterface*>(m_pluginHandle.instance());
    if (!pInterface)
    {
        KLOG_ERROR_S() << "can't convert to plugin interface!" << m_pluginHandle.errorString();
        m_pluginHandle.unload();
        return false;
    }

    int iRet = pInterface->init();
    if (iRet != 0)
    {
        KLOG_ERROR_S() << "plugin init failed! errCode:" << iRet << library;
        m_pluginHandle.unload();
        return false;
    }

    pluginInterface = pInterface;
    return true;
}

QStringList CPanelPluginHelper::getLocaleKey(const QString& key)
{
    QLocale locale;
    QString localeName = locale.name();
    QString languageName = locale.nativeLanguageName();
    QString countryName = locale.nativeCountryName();

    // clang-format off
    QStringList keys = {
        QString("%1[%2]").arg(key).arg(localeName),
        QString("%1[%2]").arg(key).arg(languageName),
        QString("%1[%2]").arg(key).arg(countryName),
        QString("%1").arg(key)
    };
    // clang-format on

    return keys;
}

void CPanelPluginHelper::clear()
{
    if (m_pluginInterface)
    {
        m_pluginInterface->uninit();
    }
    m_pluginInterface = nullptr;
    m_pluginDesktopInfo = {};
    if (m_pluginHandle.isLoaded())
    {
        m_pluginHandle.unload();
    }
}

QWidget* CPanelPluginHelper::getSubItemWidget(const QString& subItemName)
{
    if (!m_pluginInterface)
    {
        KLOG_ERROR_S() << "plugin interface is nullptr!,can't get sub item widget!";
        return nullptr;
    }

    return m_pluginInterface->getSubItemWidget(subItemName);
}

bool CPanelPluginHelper::haveUnsavedOptions()
{
    if (!m_pluginInterface)
    {
        KLOG_WARNING_S() << "plugin interface is nullptr!";
        return false;
    }

    return m_pluginInterface->haveUnsavedOptions();
}

bool CPanelPluginHelper::parseDesktopInfo(const QString& desktopPath, PluginDesktopInfo& desktopInfo)
{
    GError* error = nullptr;
    std::string desktopFilePath = desktopPath.toStdString();
    gchar *name = nullptr, *comment = nullptr,
          *icon = nullptr, *category = nullptr,
          *library = nullptr, *subItems = nullptr;
    int weight = 0;
    bool bRes = false;
    GKeyFile* keyFile = g_key_file_new();

    gboolean loadRes = g_key_file_load_from_file(keyFile, desktopFilePath.c_str(), G_KEY_FILE_KEEP_TRANSLATIONS, &error);
    if (!loadRes)
    {
        KLOG_ERROR_S() << "can't load" << desktopPath << (error ? error->message : "");
        goto out;
    }

    ///name
    name = g_key_file_get_locale_string(keyFile,
                                        GROUP_DESKTOP_ENTRY,
                                        KEY_NAME,
                                        nullptr,
                                        &error);
    if (!name)
    {
        KLOG_ERROR_S() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_NAME << (error ? error->message : "");
        goto out;
    }
    desktopInfo.name = name;
    g_free(name);

    ///comment
    comment = g_key_file_get_locale_string(keyFile,
                                           GROUP_DESKTOP_ENTRY,
                                           KEY_COMMENT,
                                           nullptr,
                                           &error);
    if (!comment)
    {
        KLOG_ERROR_S() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_COMMENT << (error ? error->message : "");
        goto out;
    }
    desktopInfo.comment = comment;
    g_free(comment);

    ///icon
    icon = g_key_file_get_string(keyFile,
                                 GROUP_DESKTOP_ENTRY,
                                 KEY_ICON,
                                 &error);
    if (!icon)
    {
        KLOG_ERROR_S() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_ICON << (error ? error->message : "");
        goto out;
    }
    desktopInfo.Icon = icon;
    g_free(icon);

    ///weight
    weight = g_key_file_get_int64(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_WEIGHT, &error);
    if (error)
    {
        KLOG_ERROR_S() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        g_error_free(error);
        error = nullptr;
    }
    desktopInfo.weight = weight;

    //category
    category = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_CATEGORY, &error);
    if (!category)
    {
        KLOG_ERROR_S() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        goto out;
    }
    desktopInfo.category = category;
    g_free(category);

    //library
    library = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_LIBRARY, &error);
    if (!library)
    {
        KLOG_ERROR_S() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_LIBRARY << (error ? error->message : "");
        goto out;
    }
    desktopInfo.library = library;
    g_free(library);
    if (!desktopInfo.library.startsWith('/'))
    {
        desktopInfo.library.insert(0, QString(PLUGIN_LIBRARY_DIR) + "/");
    }

    //subItems
    subItems = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_SUBITEMS, &error);
    if (!subItems)
    {
        KLOG_ERROR_S() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_SUBITEMS << (error ? error->message : "");
        goto out;
    }
    {
        QString strSubItems = subItems;
        g_free(subItems);

        QStringList subItemList = strSubItems.split(',');
        foreach (auto subItem, subItemList)
        {
            PluginSubItemInfo subItemInfo;
            std::string sItemName = subItem.toStdString();
            if (!g_key_file_has_group(keyFile, sItemName.c_str()))
            {
                continue;
            }

            subItemInfo.id = subItem;

            gchar* itemName = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_NAME, nullptr, &error);
            if (!itemName)
            {
                KLOG_ERROR_S() << "parse" << desktopPath << subItem << "missing" << KEY_SUBITEM_NAME << error->message;
                continue;
            }
            subItemInfo.name = itemName;
            g_free(itemName);

            gchar* subItemIcon = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_ICON, nullptr, &error);
            if (!subItemIcon)
            {
                KLOG_WARNING_S() << desktopPath << subItem << "missing" << KEY_SUBITEM_ICON << error->message;
            }
            else
            {
                subItemInfo.icon = subItemIcon;
                g_free(subItemIcon);
            }

            gchar* keywords = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_KEYWORDS, nullptr, &error);
            if (!keywords)
            {
                KLOG_ERROR_S() << desktopPath << subItem << "missing" << KEY_SUBITEM_KEYWORDS << error->message;
                continue;
            }
            else
            {
                QString temp = keywords;
                g_free(keywords);
                subItemInfo.keywords = temp.split(',');
            }
            desktopInfo.subItems.append(subItemInfo);
        }
    }
    bRes = true;
out:
    if (error)
        g_error_free(error);
    if (keyFile)
        g_key_file_free(keyFile);
    return bRes;
}
