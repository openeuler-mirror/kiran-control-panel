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

#include <QMap>
#include <QObject>
#include <QReadWriteLock>
#include <QVector>

#include "plugin-subitem-interface.h"

// 继承于QObject
class Category : public QObject
{
    Q_OBJECT
    friend class CategoryManager;

public:
    Category(QObject* parent = nullptr);
    ~Category();

    QString getName() const;
    QString getID() const;
    QString getIcon() const;
    int getWeight() const;

    QStringList getSubItemIDs();
    QVector<KiranControlPanel::SubItemPtr> getSubItems();
    KiranControlPanel::SubItemPtr getSubItem(const QString& subitemID);

signals:
    void subItemAdded(const QString& id);
    void subItemDeleted(const QString& id);
    // 提供信号，但是发出由CategoryManager连接插件封装相关信号发出
    void subItemInfoChanged(const QString& subitemID);

private:
    void setName(const QString& name);
    void setID(const QString& id);
    void setIcon(const QString& icon);
    void setWeight(int weight);

    /// @brief 追加多个功能项
    /// @param plugin 功能项所在的插件
    /// @param pluginSubItem 功能项指针
    void appendSubItems(QVector<KiranControlPanel::SubItemPtr> pluginSubItem);

    /// @brief 追加功能项
    /// @param plugin  该功能项所在的插件
    /// @param subitem 功能项指针
    void appendSubItem(KiranControlPanel::SubItemPtr subitem);
    void removeSubItem(KiranControlPanel::SubItemPtr pluginSubItem);
    void removeSubItem(const QString& subitem);

private:
    QReadWriteLock m_rwLock;
    QString m_name;
    QString m_id;
    QString m_icon;
    int m_weight;

    // 功能项数组，未经排序，后续在KiranModuleWidget之中的QListWidget进行sortItems排序
    QVector<KiranControlPanel::SubItemPtr> m_subitems;
    // 维系子功能项ID和功能项之前的映射关键
    QMap<QString, KiranControlPanel::SubItemPtr> m_subitemIDMap;
};
