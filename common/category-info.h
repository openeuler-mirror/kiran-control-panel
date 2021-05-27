//
// Created by lxh on 2021/5/8.
// 提供分类的desktop相关信息和插件信息的一层封装

#ifndef KIRAN_CONTROL_PANEL_COMMON_CATEGORY_INFO_H_
#define KIRAN_CONTROL_PANEL_COMMON_CATEGORY_INFO_H_

#include <QList>
#include <QSharedPointer>
#include <QStringList>

#include "plugin-info.h"


///分类信息
class CPanelCategoryInfo
{
    struct CategoryDesktopInfo
    {
        QString categoryName;
        QString name;
        QString comment;
        QString icon;
        QStringList keywords;
        int weight;
    };
public:
    static QSharedPointer<CPanelCategoryInfo> loadCategory(const QString& categoryPath);

    CPanelCategoryInfo();
    ~CPanelCategoryInfo();

    bool load(const QString& categoryPath);
    bool isValid();
    const CategoryDesktopInfo& getCategoryDesktopInfo() const;
    void insertPlugin(QSharedPointer<CPanelPluginHelper> pluginHelper);
    QSharedPointer<CPanelPluginHelper> getPlugin(int idx);
    QList<QSharedPointer<CPanelPluginHelper>> getPlugins();
    int pluginsCount();

private:
    void clear();
    bool parseDesktopInfo(const QString& desktopPath, CategoryDesktopInfo& desktopInfo);

private:
    bool m_isValid = false;
    ///分类的desktop文件信息
    CategoryDesktopInfo m_categoryDesktopInfo;
    ///插件列表
    QList<QSharedPointer<CPanelPluginHelper>> m_plugins;
};

#endif  //KIRAN_CONTROL_PANEL_COMMON_CATEGORY_INFO_H_
