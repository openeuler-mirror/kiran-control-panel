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

#ifndef KIRAN_CONTROL_PANEL_SEARCH_MODEL_H
#define KIRAN_CONTROL_PANEL_SEARCH_MODEL_H

#include <QStandardItemModel>

//通过分类管理层级信息初始化搜索信息形成 "分类->功能项" 的搜索项
//通过功能项提供的搜索关键词形成 "分类->功能项: 关键词" 的搜索项
//如果分类下只存在一个单例则不显示功能项目名称
class SearchModel : public  QStandardItemModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject* parent = nullptr);
    ~SearchModel();

    static const int roleCategoryID;
    static const int roleSubItemID;
    static const int roleSearchKey;

private:
    void init();
    void loadSearchModel();
    void appendItem(const QString& text, const QString& category, const QString& subItem, const QString& searchKey = QString());

private slots:
    void handleSubItemsChanged();
    void handleSubItemAdded(const QString& id);
    void handleSubItemDeleted(const QString& id);
    void handleSubItemInfoChanged(const QString& subitemID);
};

#endif  // KIRAN_CONTROL_PANEL_SEARCH_MODEL_H
