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
#pragma once

#include <QObject>
#include <QPluginLoader>
#include <QVector>

#include "plugin-subitem-interface.h"
class Plugin : public QObject
{
    Q_OBJECT
public:
    Plugin(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~Plugin() = default;

    /// @brief 插件状态是否有效
    /// @return 是否有效，加载是否成功
    bool isValid() const { return m_isValid; };

    /// @brief 获取插件ID
    /// @return  插件标识符
    QString getID() const { return m_id; };

    /// @brief 获取插件图标，主要用于兼容老版本接口，提供给启动器单独启动标题栏图标
    /// @return 图标路径
    QString getIcon() const { return m_icon; };

    /// @brief 获取插件已翻译显示名称，主要用于兼容老版本接口，提供给启动器单独启动标题栏文本
    /// @return 插件标题栏名称
    QString getName() const { return m_name; };

    /// @brief 加载指定路径下的插件
    /// @param path 插件路径,可以为老版本的插件desktop文件，或者是新版本的共享库路径
    /// @return 是否加载成功
    virtual bool load(const QString& path) = 0;

    /// @brief 卸载插件
    virtual void unload() = 0;

    /// @brief 获取插件中的功能项
    /// @return 功能项指针数组，此数组会发生改变，不能缓存
    virtual QVector<KiranControlPanel::SubItemPtr> getSubItems() = 0;

signals:
    /// @brief 插件内的功能项信息变更信号
    void subItemInfoChanged(const QString& subItemID);
    /// @brief 插件内的功能项新增删除信号
    void subItemChanged();

protected:
    bool m_isValid = false;
    QString m_id;
    QString m_name;
    QString m_icon;
};