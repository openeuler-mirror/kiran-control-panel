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
#include "plugin-v1.h"
#include "config.h"
#include "logging-category.h"
#include "plugin-v1-subitem-wrapper.h"

#include <glib.h>
#include <qt5-log-i.h>
#include <QFileInfo>
#include <QScopedPointer>

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

PluginV1::PluginV1(QObject* parent)
    : Plugin(parent)
{
}

PluginV1::~PluginV1()
{
    unload();
}

bool PluginV1::load(const QString& desktopPath)
{
    if (isValid())
    {
        KLOG_WARNING(qLcPluginFramework) << "plugin is already loaded!,please unload first!";
        return false;
    }

    QFileInfo fileInfo(desktopPath);
    if (!fileInfo.exists())
    {
        KLOG_ERROR(qLcPluginFramework) << "can't load plugin," << desktopPath << ",file isn't exist!";
        return false;
    }

    m_libraryPath = getLibraryPathFromDesktop(desktopPath);
    m_pluginLoader.setFileName(m_libraryPath);
    if (!m_pluginLoader.load())
    {
        KLOG_ERROR(qLcPluginFramework) << "can't load plugin," << m_pluginLoader.errorString();
    }
    if (!m_pluginLoader.isLoaded())
    {
        KLOG_ERROR(qLcPluginFramework) << "can't load plugin,"
                                       << m_pluginLoader.errorString()
                                       << "," << m_libraryPath;
        return false;
    }

    m_interface = qobject_cast<KcpPluginInterface*>(m_pluginLoader.instance());
    if (!m_interface)
    {
        KLOG_ERROR(qLcPluginFramework) << "can't convert to plugin interface!" << m_pluginLoader.errorString();
        m_pluginLoader.unload();
        return false;
    }

    int iret = m_interface->init();
    if (iret != 0)
    {
        KLOG_ERROR(qLcPluginFramework) << "plugin init failed!" << m_libraryPath << "error code:" << iret;
        m_pluginLoader.unload();
        return false;
    }

    m_visiableSubItems = m_interface->visibleSubItems();

    // 加载插件共享库
    if (!parseDesktopInfo(desktopPath))
    {
        KLOG_ERROR(qLcPluginFramework) << "can't parse desktop," << desktopPath;
        m_visiableSubItems.clear();
        m_interface->uninit();
        m_pluginLoader.unload();
        m_libraryPath.clear();
        m_isValid = false;
        return false;
    }

    m_isValid = true;
    return true;
}

void PluginV1::unload()
{
    if (!m_isValid)
        return;

    m_isValid = false;
    m_id = "";
    m_name = "";
    m_icon = "";

    m_subItemVec.clear();

    m_visiableSubItems.clear();
    m_libraryPath.clear();
    m_interface = nullptr;

    m_pluginLoader.unload();
}

QVector<KiranControlPanel::SubItemPtr> PluginV1::getSubItems()
{
    return m_subItemVec;
}

QString PluginV1::getLibraryPath()
{
    return m_libraryPath;
}

QString PluginV1::getLibraryPathFromDesktop(const QString& desktopPath)
{
    GKeyFile* keyFile = g_key_file_new();
    gchar* library = nullptr;
    GError* error = nullptr;

    gboolean loadRes = g_key_file_load_from_file(keyFile, desktopPath.toStdString().c_str(), G_KEY_FILE_KEEP_TRANSLATIONS, &error);
    if (!loadRes)
    {
        KLOG_ERROR(qLcPluginFramework) << "load" << desktopPath << "failed" << (error ? error->message : "");
        g_clear_error(&error);
        return "";
    }

    library = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_LIBRARY, &error);
    if (!library)
    {
        KLOG_ERROR(qLcPluginFramework) << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_LIBRARY << (error ? error->message : "");
        g_clear_error(&error);
        g_key_file_free(keyFile);
        return "";
    }
    QString res = library;
    if (!res.startsWith('/'))
    {
        res.insert(0, QString(PLUGIN_LIBRARY_DIR) + "/");
    }
    g_key_file_free(keyFile);
    g_free(library);
    return res;
}

bool PluginV1::parseDesktopInfo(const QString& desktopPath)
{
    GError* error = nullptr;
    bool bRes = false;
    GKeyFile* keyFile = g_key_file_new();
    gchar *name = nullptr, *icon = nullptr,
          *category = nullptr, *subItems = nullptr;
    int weight;
    QString categoryStr;

    gboolean loadRes = g_key_file_load_from_file(keyFile, desktopPath.toStdString().c_str(), G_KEY_FILE_KEEP_TRANSLATIONS, &error);
    if (!loadRes)
    {
        KLOG_ERROR(qLcPluginFramework) << "can't load" << desktopPath << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }

    // 插件名称
    name = g_key_file_get_locale_string(keyFile,
                                        GROUP_DESKTOP_ENTRY,
                                        KEY_NAME,
                                        nullptr,
                                        &error);
    if (!name)
    {
        KLOG_ERROR(qLcPluginFramework) << "can't get" << GROUP_DESKTOP_ENTRY << KEY_NAME << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    m_name = name;
    g_free(name);

    // 插件图标
    icon = g_key_file_get_string(keyFile,
                                 GROUP_DESKTOP_ENTRY,
                                 KEY_ICON,
                                 &error);
    if (!icon)
    {
        KLOG_ERROR(qLcPluginFramework) << "can't get" << GROUP_DESKTOP_ENTRY << KEY_ICON << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    m_icon = icon;
    g_free(icon);

    // 插件权重
    weight = g_key_file_get_int64(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_WEIGHT, &error);
    if (error)
    {
        KLOG_ERROR(qLcPluginFramework) << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        g_clear_error(&error);
    }

    // 分类
    category = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_CATEGORY, &error);
    if (!category)
    {
        KLOG_ERROR(qLcPluginFramework) << GROUP_KIRAN_CONTROL_PANEL_PLUGIN << KEY_WEIGHT << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }
    categoryStr = category;
    g_free(category);

    // 功能项
    subItems = g_key_file_get_string(keyFile, GROUP_KIRAN_CONTROL_PANEL_PLUGIN, KEY_SUBITEMS, &error);
    if (!subItems)
    {
        KLOG_ERROR(qLcPluginFramework) << GROUP_KIRAN_CONTROL_PANEL_PLUGIN
                                       << KEY_SUBITEMS
                                       << (error ? error->message : "");
        g_clear_error(&error);
        goto out;
    }

    {
        QString strSubItems = subItems;
        g_free(subItems);

        QStringList subItemList = strSubItems.split(',');
        foreach (auto subItem, subItemList)
        {
            std::string sItemName = subItem.toStdString();
            if (!g_key_file_has_group(keyFile, sItemName.c_str()) || !m_visiableSubItems.contains(sItemName.c_str()))
            {
                continue;
            }

            QScopedPointer<PluginSubItemWrapper> subitemPointer(new PluginSubItemWrapper(m_interface));
            subitemPointer->setID(subItem);

            gchar* itemName = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_NAME, nullptr, &error);
            if (!itemName)
            {
                KLOG_ERROR(qLcPluginFramework) << "parse" << desktopPath << subItem
                                               << "missing" << KEY_SUBITEM_NAME << error->message;
                g_clear_error(&error);
                continue;
            }
            subitemPointer->setName(itemName);
            g_free(itemName);

            gchar* subItemIcon = g_key_file_get_locale_string(keyFile, sItemName.c_str(), KEY_SUBITEM_ICON, nullptr, &error);
            if (!subItemIcon)
            {
                KLOG_WARNING(qLcPluginFramework) << desktopPath << subItem
                                                 << "missing" << KEY_SUBITEM_ICON << error->message;
                g_clear_error(&error);
            }
            else
            {
                subitemPointer->setIcon(subItemIcon);
                g_free(subItemIcon);
            }

            subitemPointer->setWeight(weight);
            subitemPointer->setCategory(categoryStr);
            m_subItemVec.append(KiranControlPanel::SubItemPtr(subitemPointer.take()));
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