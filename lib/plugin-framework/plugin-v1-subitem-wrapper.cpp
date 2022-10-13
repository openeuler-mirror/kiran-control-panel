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