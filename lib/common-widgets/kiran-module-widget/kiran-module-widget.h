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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRANMODULEWIDGET_H
#define KIRANMODULEWIDGET_H

#include <QMap>
#include <QWidget>
#include "plugin-subitem-interface.h"
namespace Ui
{
class KiranModuleWidget;
}

class QListWidgetItem;
class Category;
class PluginSubitemInterface;
class KiranModuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KiranModuleWidget(QWidget *parent = 0);
    ~KiranModuleWidget();

    void setLeftContentsMargins(const int &leftmargin);

    void clear();

    void setCategory(Category *c);
    void setSubItems(QVector<KiranControlPanel::SubItemPtr> subitems);

    bool checkHasUnSaved();
    void jumpTo(const QString &subItemID, const QString &customKey);

private slots:
    void handleCurrentItemChanged();
    void handleCategorySubItemAdded(const QString &id);
    void handleCategorySubItemDeleted(const QString &id);
    void handleCategorySubItemInfoChanged(const QString &subitemID);

private:
    void init();
    void removeListWidgetItem(KiranControlPanel::SubItemPtr subitem);
    void appendListWidgetItem(KiranControlPanel::SubItemPtr subitem);

private:
    Ui::KiranModuleWidget *ui;

    QWidget *m_subItemWidget = nullptr;
    Category *m_category = nullptr;

    QVector<KiranControlPanel::SubItemPtr> m_subitems;
    /// 保存当前选中的功能项信息
    QPair<QListWidgetItem *, KiranControlPanel::SubItemPtr> m_currentSubItem;
    /// 维系功能项目界面显示条目与功能项目关联的字典
    QMap<QListWidgetItem *, KiranControlPanel::SubItemPtr> m_subItemsMap;
};

#endif  // KIRANMODULEWIDGET_H
