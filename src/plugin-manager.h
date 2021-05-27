//
// Created by lxh on 2021/5/7.
// 提供分类和插件进行加载管理的相关方法

#ifndef KIRAN_CONTROL_PANEL_COMMON_PLUGIN_MANAGER_H_
#define KIRAN_CONTROL_PANEL_COMMON_PLUGIN_MANAGER_H_

#include "category-info.h"
#include "kcp-plugin-interface.h"
#include "plugin-info.h"

class CPanelPluginManager
{
public:
    using CategoryInfoList = QList<QSharedPointer<CPanelCategoryInfo>>;
    static CPanelPluginManager* getInstance();
    static void deleteInstance();
    ~CPanelPluginManager();

    void loadAll();
    CategoryInfoList getCategorys();

private:
    CPanelPluginManager();
    void clear();

private:
    CategoryInfoList m_categoryInfos;

private:
    static CPanelPluginManager* _instance;
};


#endif  //KIRAN_CONTROL_PANEL_COMMON_PLUGIN_MANAGER_H_
