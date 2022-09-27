#pragma once

#include <QWidget>
#include "kcp-plugin-subitem.h"

class TestSubItem : public KcpPluginSubItem
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
    int m_weight;
    QVector<QPair<QString, QString>> m_searchKeys;
};