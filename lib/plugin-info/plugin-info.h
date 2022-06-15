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

// 提供插件接口和插件desktop相关信息的一层封装

#ifndef KIRAN_CONTROL_PANEL_COMMON_PLUGIN_INFO_H_
#define KIRAN_CONTROL_PANEL_COMMON_PLUGIN_INFO_H_

#include <QSettings>
#include <QString>
#include <QList>
#include <QPluginLoader>

#include "kcp-plugin-interface.h"

struct PluginSubItemInfo
{
    QString id;
    QString name;
    QString icon;
    QStringList keywords;
};

struct PluginDesktopInfo
{
    QString name;
    QString comment;
    QString Icon;

    QString category;
    int weight;
    QString library;
    QList<PluginSubItemInfo> subItems;
};

class QTranslator;

class PluginHelper;
typedef QSharedPointer<PluginHelper> PluginHelperPtr;
typedef QList<PluginHelperPtr> PluginHelperPtrList;

class PluginHelper : public QObject
{
    Q_OBJECT
public:
    static PluginHelperPtr loadPlugin(const QString& desktopFile);

    PluginHelper() = default;
    ~PluginHelper();

    const PluginDesktopInfo& getPluginDesktopInfo();

    ///插件接口方法包装
    QStringList visibleSubItems();
    QWidget* getSubItemWidget(const QString& subItemName);
    bool haveUnsavedOptions();

signals:
    void visibleSubItemsChanged();

private:
    bool load(const QString& desktopFile);
    void clear();
    static bool parseDesktopInfo(const QString& desktopPath,PluginDesktopInfo& desktopInfo);
    bool loadLibrary(const QString& library, KcpPluginInterface*& pluginInterface,QObject*& pluginInstance);

private:
    QPluginLoader m_pluginHandle;
    KcpPluginInterface* m_pluginInterface = nullptr;
    PluginDesktopInfo m_pluginDesktopInfo;
};

#endif  //KIRAN_CONTROL_PANEL_COMMON_PLUGIN_INFO_H_
