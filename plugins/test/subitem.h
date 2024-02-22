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

#include <QWidget>
#include "plugin-subitem-interface.h"

class TestSubItem : public PluginSubitemInterface
{
public:
    TestSubItem(){};
    ~TestSubItem(){};

    void setID(const QString& id)
    {
        m_id = id;
    }
    QString getID() override
    {
        return m_id;
    }

    void setName(const QString& name)
    {
        m_name = name;
    }
    QString getName() override
    {
        return m_name;
    }

    void setCategory(const QString& category)
    {
        m_category = category;
    }
    QString getCategory() override
    {
        return m_category;
    }

    void setDesc(const QString& desc)
    {
        m_desc = desc;
    }
    QString getDesc() override
    {
        return m_desc;
    }

    void setIcon(const QString& icon)
    {
        m_icon = icon;
    }

    QString getIcon() override
    {
        return m_icon;
    }

    void setWeight(int weight)
    {
        m_weight = weight;
    }

    int getWeight() override
    {
        return m_weight;
    }

    void setSearchKeys(QVector<QPair<QString, QString>> searchkeys)
    {
        m_searchKeys.swap(searchkeys);
    }

    virtual QVector<QPair<QString, QString>> getSearchKeys() override
    {
        return m_searchKeys;
    }

    virtual QWidget* createWidget() override
    {
        QWidget* widget = new QWidget;
        //
        return widget;
    }

    virtual bool jumpToSearchEntry(const QString& key) override
    {
        return false;
    }

    virtual bool haveUnSavedOptions() override
    {
        return false;
    }

private:
    QString m_id;
    QString m_name;
    QString m_category;
    QString m_desc;
    QString m_icon;
    int m_weight = 0;
    QVector<QPair<QString, QString>> m_searchKeys;
};