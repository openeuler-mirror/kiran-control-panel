#include "plugin-manager.h"
#include "plugin-loader.h"

#include <QMutex>

PluginManager* PluginManager::_instance = nullptr;

PluginManager::~PluginManager()
{
    qDeleteAll(m_plugins);
}

PluginManager* PluginManager::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if (Q_UNLIKELY(!_instance))
    {
        if (!_instance)
        {
            _instance = new PluginManager();
        }
    }

    return _instance;
}

bool PluginManager::init()
{
    if (m_isInited)
    {
        return true;
    }

    m_plugins = PluginLoader::loadAllPlugins();
    m_isInited = true;
    return true;
}

void PluginManager::dump()
{
    for (auto plugin : m_plugins)
    {
        fprintf(stdout, "plugin -- id: %-20s    name: %-20s\n",
                plugin->getID().toStdString().c_str(),
                plugin->getName().toStdString().c_str());
        auto subitemList = plugin->getSubItems();
        for (auto subitem : subitemList)
        {
            fprintf(stdout, "\tsubitem id: %-15s    name: %-15s\n",
                    subitem->getID().toStdString().c_str(),
                    subitem->getName().toStdString().c_str());
        }
    }
}

QList<Plugin*> PluginManager::getPlugins()
{
    return m_plugins;
}