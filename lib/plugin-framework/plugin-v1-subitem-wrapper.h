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

#include <QList>
#include <QPair>

#include "kcp-plugin-interface.h"
#include "kcp-plugin-subitem.h"

/// @brief 该处是对老版本接口通过Desktop拿功能项信息的一个封装，用于给上层提供统一的接口
class PluginSubItemWrapper : public KcpPluginSubItem
{
public:
    PluginSubItemWrapper(KcpPluginInterface* pluginInterface);

    QString getID() override;
    void setID(const QString& id);

    QString getName() override;
    void setName(const QString& name);

    QString getCategory() override;
    void setCategory(const QString& category);

    QString getDesc() override { return QString(); };

    QString getIcon() override;
    void setIcon(const QString& icon);

    int getWeight() override;
    void setWeight(int weight);

    QVector<QPair<QString, QString>> getSearchKeys() override { return {}; };

    QWidget* createWidget() override;

    bool jumpToSearchEntry(const QString& key) { return false; };
    bool haveUnSavedOptions() override;

private:
    KcpPluginInterface* m_pluginInterface;
    QString m_id;
    QString m_name;
    QString m_category;
    QString m_icon;
    int m_weight;
};