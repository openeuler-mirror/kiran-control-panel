#pragma once

#include "plugin.h"

class PluginLoader
{
public:
    PluginLoader(){};
    ~PluginLoader(){};

    static QList<Plugin*> loadAllPlugins();
    static Plugin* loadSinglePlugin(const QString& path);
};