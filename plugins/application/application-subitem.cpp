/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include "application-subitem.h"
#include <qt5-log-i.h>

ApplicationSubItem::ApplicationSubItem(CreateWidgetFunc func, QObject* parent)
    : QObject(parent), m_func(func)
{
}

ApplicationSubItem::~ApplicationSubItem()
{
}

// 功能项ID,用于区分功能项,应确保其唯一
void ApplicationSubItem::setID(const QString& subitemID)
{
    m_id = subitemID;
}

QString ApplicationSubItem::getID()
{
    return m_id;
}

// 功能项名称，用于显示在启动器标题栏之中
void ApplicationSubItem::setName(const QString& name)
{
    m_name = name;
}

QString ApplicationSubItem::getName()
{
    return m_name;
}

// 获取功能项分类ID，该功能项属于哪个分类
void ApplicationSubItem::setCategory(const QString& category)
{
    m_category = category;
}

QString ApplicationSubItem::getCategory()
{
    return m_category;
}

// 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
void ApplicationSubItem::setDesc(const QString& desc)
{
    m_desc = desc;
}

QString ApplicationSubItem::getDesc()
{
    return m_desc;
}

// 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
void ApplicationSubItem::setIcon(const QString& icon)
{
    m_icon = icon;
}

QString ApplicationSubItem::getIcon()
{
    return m_icon;
}

// 获取功能项权重，用于多个功能项排序
void ApplicationSubItem::setWeight(int weight)
{
    m_weight = weight;
}

int ApplicationSubItem::getWeight()
{
    return m_weight;
}

// 创建显示控件
QWidget* ApplicationSubItem::createWidget()
{
    return m_func();
}