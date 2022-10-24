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

#ifndef __PANEL_CONTENT_WIDGET_H__
#define __PANEL_CONTENT_WIDGET_H__

#include <QWidget>

class CategorySideBar;
class KiranModuleWidget;
class PanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PanelWidget(QWidget *parent = 0);
    ~PanelWidget();

    void jumpTo(const QString& categoryID, const QString& subItemID, const QString& customKey);

private:
    void init();

private slots:
    void handleCurrentCategoryChanged(const QString& prev,
                                      const QString& cur);

private:
    CategorySideBar* m_categorySideBar;
    KiranModuleWidget* m_moduleWidget;
    /// @brief 缓存当前ModelWidget之中显示的分类ID
    QString m_currentCategoryID;
};

#endif  //__PANEL_CONTENT_WIDGET_H__
