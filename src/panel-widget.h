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

#ifndef __PANEL_CONTENT_WIDGET_H__
#define __PANEL_CONTENT_WIDGET_H__

#include <QWidget>

class CategoryWidget;
namespace Ui
{
class PanelWidget;
}

class PanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PanelWidget(QWidget *parent = 0);
    ~PanelWidget();

private:
    void init();

private:
    void resizeEvent(QResizeEvent *event);

private slots:
    void handleCurrentCategoryChanged(int curCategoryIdx,
                                      int prevCategoryIdx);

private:
    Ui::PanelWidget *ui;
    CategoryWidget *m_categoryWidget;
    int m_currentCategoryIndex = -1;
};

#endif  //__PANEL_CONTENT_WIDGET_H__
