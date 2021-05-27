//
// Created by lxh on 2021/5/7.
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
class CPanelPluginHelper
{
public:
    static QSharedPointer<CPanelPluginHelper> loadPlugin(const QString& desktopFile);

    CPanelPluginHelper() = default;
    ~CPanelPluginHelper();

    bool load(const QString& desktopFile);
    const PluginDesktopInfo& getPluginDesktopInfo();
    ///插件接口方法包装
    QWidget* getSubItemWidget(const QString& subItemName);
    bool haveUnsavedOptions();

private:
    void clear();
    ///通过传入一个key，返回当前语言环境的key列表
    ///eg: 传入Name，返回{"Name[zh_CN]","Name[zh]","Name"}
    QStringList getLocaleKey(const QString& key);
    static bool parseDesktopInfo(const QString& desktopPath,PluginDesktopInfo& desktopInfo);
    bool loadLibrary(const QString& library, KcpPluginInterface*& pluginInterface);

private:
    QPluginLoader m_pluginHandle;
    KcpPluginInterface* m_pluginInterface = nullptr;
    PluginDesktopInfo m_pluginDesktopInfo;
};

#endif  //KIRAN_CONTROL_PANEL_COMMON_PLUGIN_INFO_H_
