#pragma once

#include "plugin.h"

class PluginManager
{
public:
    static PluginManager* instance();
    ~PluginManager();

    /// @brief 插件管理初始化，加载所有插件信息
    bool init();
    void dump();

    /// @brief 获取插件列表
    /// @return 插件列表
    QList<Plugin*> getPlugins();

private:
    PluginManager() = default;

private:
    static PluginManager* _instance;
    QList<Plugin*> m_plugins;
    bool m_isInited = false;
};