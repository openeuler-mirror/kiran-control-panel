/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

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
