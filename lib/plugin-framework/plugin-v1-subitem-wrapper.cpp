#include "plugin-v1-subitem-wrapper.h"

PluginSubItemWrapper::PluginSubItemWrapper(KcpPluginInterface* pluginInterface)
    : m_pluginInterface(pluginInterface)
{
}

QString PluginSubItemWrapper::getID()
{
    return m_id;
}

void PluginSubItemWrapper::setID(const QString& id)
{
    m_id = id;
}

QString PluginSubItemWrapper::getName()
{
    return m_name;
}

void PluginSubItemWrapper::setName(const QString& name)
{
    m_name = name;
}

QString PluginSubItemWrapper::getCategory()
{
    return m_category;
}

void PluginSubItemWrapper::setCategory(const QString& category)
{
    m_category = category;
}

QString PluginSubItemWrapper::getIcon()
{
    return m_icon;
}

void PluginSubItemWrapper::setIcon(const QString& icon)
{
    m_icon = icon;
}

int PluginSubItemWrapper::getWeight()
{
    return m_weight;
}

void PluginSubItemWrapper::setWeight(int weight)
{
    m_weight = weight;
}

QWidget* PluginSubItemWrapper::createWidget()
{
    return m_pluginInterface->getSubItemWidget(getID());
}

bool PluginSubItemWrapper::haveUnSavedOptions()
{
    return m_pluginInterface->haveUnsavedOptions();
}