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
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "plugin-info.h"
#include "config.h"

#include <glib.h>
#include <qt5-log-i.h>
#include <QApplication>
#include <QFileInfo>
#include <QLocale>
#include <QMetaMethod>
#include <QObject>
#include <QSet>
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

/// subitem
#define KEY_SUBITEM_ID "Id"
#define KEY_SUBITEM_NAME "Name"
#define KEY_SUBITEM_ICON "Icon"
#define KEY_SUBITEM_KEYWORDS "Keywords"

PluginHelperPtr PluginHelper::loadPlugin(const QString& desktop)
{
    PluginHelperPtr pSharedPluginHelper(new PluginHelper);
    if (!pSharedPluginHelper->load(desktop))
    {
        KLOG_ERROR() << "can't load plugin desktop <" << desktop << ">!";
        return nullptr;
    }
    return pSharedPluginHelper;
}

PluginHelper::~PluginHelper()
{
    clear();
}

bool PluginHelper::load(const QString& desktopFile)
{
    clear();

    if (!QFileInfo::exists(desktopFile))
    {
        KLOG_ERROR() << "can't parse desktop file," << desktopFile << "is not exist!";
        return false;
    }

    PluginDesktopInfo desktopInfo;

    // 解析插件Desktop中的信息
    if (!parseDesktopInfo(desktopFile, desktopInfo))
    {
        KLOG_ERROR() << "parser desktop failed!," << desktopFile;
    }

    // 加载插件共享库
    KcpPluginInterface* pluginInterface = nullptr;
    QObject* pluginInstance = nullptr;
    if (!loadLibrary(desktopInfo.library, pluginInterface, pluginInstance))
    {
        KLOG_ERROR() << "load library failed!" << desktopInfo.library;
        return false;
    }

    // 检查插件是否向外提供控制面板指定信号,若提供则连接该信号至PluginHelper转发至外部
    auto pluginMetaObject = pluginInstance->metaObject();
    static QSet<QString> PluginSignalMap = {
        "visibleSubItemsChanged()"
    };
    for (auto pluginSignal : PluginSignalMap)
    {
        int sigIdx = pluginMetaObject->indexOfSignal(pluginSignal.toStdString().c_str());
        if (sigIdx != -1)
        {
            // 和SIGNAL宏一样
            QString connectSignalName = "2"+pluginSignal;

            if (!connect(pluginInstance, connectSignalName.toStdString().c_str(),
                         this, connectSignalName.toStdString().c_str()))
            {
                KLOG_ERROR() << "can't connect plugin" << desktopInfo.name << pluginSignal;
            }
            else
            {
                KLOG_DEBUG() << "connect plugin" << desktopInfo.name << pluginSignal << "to plugin helper success";
            }
        }
    }

    m_pluginDesktopInfo = desktopInfo;
    m_pluginInterface = pluginInterface;
    return true;
}

const PluginDesktopInfo& PluginHelper::getPluginDesktopInfo()
{
    return m_pluginDesktopInfo;
}

QStringList PluginHelper::visibleSubItems()
{
    return m_pluginInterface->visibleSubItems();
}

bool PluginHelper::loadLibrary(const QString& library,
                               KcpPluginInterface*& pluginInterface,
                               QObject*& pluginInstance)
{
    QFileInfo libraryInfo(library);
    if (!libraryInfo.exists())
    {
        KLOG_ERROR() << "can't find library(" << library << ")!";
        return false;
    }

    m_pluginHandle.setFileName(library);
    if (!m_pluginHandle.load())
    {
        KLOG_ERROR() << "can't load plugin," << m_pluginHandle.errorString();
    }
    if (!m_pluginHandle.isLoaded())
    {
        KLOG_ERROR() << "can't load library,"
                     << m_pluginHandle.errorString()
                     << "," << library;
        return false;
    }

    // TODO:连接QPluginLoader::instance()信号到PluginHelper之中
    //  QObject* pluginInstance = m_pluginHandle.instance();
    //  const QMetaObject* metaObject = pluginInstance->metaObject();
    //  int methodCount = metaObject->methodCount();
    //  for(int i=0;i<methodCount;i++)
    //  {
    //      KLOG_INFO() << "meta method: " <<  metaObject->method(i).typeName() << metaObject->method(i).methodType() <<  metaObject->method(i).name();
    //  }
    //  KLOG_INFO() << "singal <notifyVisiableCategoryChanged>:" << metaObject->indexOfSignal("notifyVisiableCategoryChanged()");

    KcpPluginInterface* pInterface = qobject_cast<KcpPluginInterface*>(m_pluginHandle.instance());
    if (!pInterface)
    {
        KLOG_ERROR() << "can't convert to plugin interface!" << m_pluginHandle.errorString();
        m_pluginHandle.unload();
        return false;
    }

    int iRet = pInterface->init();
    if (iRet != 0)
    {
        KLOG_ERROR() << "plugin init failed! errCode:" << iRet << library;
        m_pluginHandle.unload();
        return false;
    }

    pluginInterface = pInterface;
    pluginInstance = m_pluginHandle.instance();
    return true;
}

void PluginHelper::clear()
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

QWidget* PluginHelper::getSubItemWidget(const QString& subItemName)
{
    if (!m_pluginInterface)
    {
        KLOG_ERROR() << "plugin interface is nullptr!,can't get sub item widget!";
        return nullptr;
    }

    return m_pluginInterface->getSubItemWidget(subItemName);
}

bool PluginHelper::haveUnsavedOptions()
{
    if (!m_pluginInterface)
    {
        KLOG_WARNING() << "plugin interface is nullptr!";
        return false;
    }

    return m_pluginInterface->haveUnsavedOptions();
}

bool PluginHelper::parseDesktopInfo(const QString& desktopPath, PluginDesktopInfo& desktopInfo)
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
        KLOG_ERROR() << "can't load" << desktopPath << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }

    /// name
    name = g_key_file_get_locale_string(keyFile,
                                        GROUP_DESKTOP_ENTRY,
                                        KEY_NAME,
                                        nullptr,
                                        &error);
    if (!name)
    {
        KLOG_ERROR() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_NAME << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    desktopInfo.name = name;
    g_free(name);

    /// comment
    comment = g_key_file_get_locale_string(keyFile,
                                           GROUP_DESKTOP_ENTRY,
                                           KEY_COMMENT,
                                           nullptr,
                                           &error);
    if (!comment)
    {
        KLOG_ERROR() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_COMMENT << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    desktopInfo.comment = comment;
    g_free(comment);

    /// icon
    icon = g_key_file_get_string(keyFile,
                                 GROUP_DESKTOP_ENTRY,
                                 KEY_ICON,
                                 &error);
    if (!icon)
    {
        KLOG_ERROR() << "can't get" << GROUP_DESKTOP_ENTRY << KEY_ICON << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    desktopInfo.Icon = icon;
    g_free(icon);

    /// weight
    weight = g_key_file_get_int64(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_WEIGHT, &error);
    if (error)
    {
        KLOG_ERROR() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        g_clear_error(&error);
    }
    desktopInfo.weight = weight;

    // category
    category = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_CATEGORY, &error);
    if (!category)
    {
        KLOG_ERROR() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    desktopInfo.category = category;
    g_free(category);

    // library
    library = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_LIBRARY, &error);
    if (!library)
    {
        KLOG_ERROR() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_LIBRARY << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    desktopInfo.library = library;
    g_free(library);
    if (!desktopInfo.library.startsWith('/'))
    {
        desktopInfo.library.insert(0, QString(PLUGIN_LIBRARY_DIR) + "/");
    }
    // subItems
    subItems = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_SUBITEMS, &error);
    if (!subItems)
    {
        KLOG_ERROR() << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_SUBITEMS << (error ? error->message : "");
        g_clear_error(&error);
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
                KLOG_ERROR() << "parse" << desktopPath << subItem << "missing" << KEY_SUBITEM_NAME << error->message;
                g_clear_error(&error);
                continue;
            }
            subItemInfo.name = itemName;
            g_free(itemName);

            gchar* subItemIcon = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_ICON, nullptr, &error);
            if (!subItemIcon)
            {
                KLOG_WARNING() << desktopPath << subItem << "missing" << KEY_SUBITEM_ICON << error->message;
                g_clear_error(&error);
            }
            else
            {
                subItemInfo.icon = subItemIcon;
                g_free(subItemIcon);
            }

            gchar* keywords = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_KEYWORDS, nullptr, &error);
            if (!keywords)
            {
                KLOG_ERROR() << desktopPath << subItem << "missing" << KEY_SUBITEM_KEYWORDS << error->message;
                g_clear_error(&error);
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