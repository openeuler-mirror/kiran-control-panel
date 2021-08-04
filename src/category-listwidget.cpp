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

#include "category-listwidget.h"
#include "category-listwidget-item.h"
#include "plugin-manager.h"

#include <qt5-log-i.h>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

CategoryListWidget::CategoryListWidget(QWidget *parent)
    : QListWidget(parent)
{
    init();
}

void CategoryListWidget::init()
{
    initUI();
}

void CategoryListWidget::initUI()
{
    setAttribute(Qt::WA_Hover, true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet(styleSheetStr());
    setViewportMargins(cListWidgetLeftPadding, 12, cListWidgetRightPadding, 12);
    setSelectionBehavior(QListView::SelectRows);
    setSelectionMode(QListView::SingleSelection);
}

void CategoryListWidget::setIconMode(const bool &iconMode)
{
    m_isIconMode = iconMode;
    iconMode ? setStyleSheet(styleSheetStr()) : setStyleSheet(styleExpandSheetStr());
}

void CategoryListWidget::setTextShow(const bool &showText)
{
    QListWidgetItem *tempItem = nullptr;
    QWidget *widget = nullptr;
    for (int i = 0; i < count(); i++)
    {
        tempItem = item(i);
        widget = itemWidget(tempItem);
        auto categoryItemWidget = qobject_cast<CategoryListWidgetItemWidget *>(widget);
        if (categoryItemWidget)
        {
            categoryItemWidget->setTextVisible(showText);
        }
    }
}

void CategoryListWidget::resizeEvent(QResizeEvent *event)
{
    setViewportMargins(cListWidgetLeftPadding, 12, verticalScrollBar()->isVisible() ? (cListWidgetRightPadding - 6) : cListWidgetRightPadding, 12);
    QListWidget::resizeEvent(event);
}

bool CategoryListWidget::isIconMode() const
{
    return m_isIconMode;
}

QString CategoryListWidget::styleExpandSheetStr()
{
    return QString(" QListWidget{"
                   "border: 0px;"
                   "border-top: 0px;"
                   "border-right: 1px solid rgba(255, 255, 255, 20);"
                   "border-bottom: 0px;"  //                    "border-bottom-left-radius: 8px;"
                   "background-color:#333333;"
                   "outline:0px;"
                   "}"
                   "QListView::item{"
                   "border-radius:10px;"
                   "}"
                   "QListWidget::item:hover {"
                   "background-color: rgba(255, 255, 255, 10);"
                   "}"
                   "QListWidget::item:selected {"
                   "background-color:#43a3f2;"
                   "}");
}

QString CategoryListWidget::styleSheetStr()
{
    return QString(" QListWidget{"
                   "border: 0px;"
                   "border-top: 0px;"
                   "border-right: 1px solid rgba(255, 255, 255, 20);"
                   "border-bottom: 0px;"
                   "outline:0px;"
                   "background-color:#222222;"
                   "}"
                   "QListView::item{"
                   "border-radius:10px;"
                   "}"
                   "QListWidget::item:hover {"
                   "background-color: rgba(255, 255, 255, 10);"
                   "}"
                   "QListWidget::item:selected {"
                   "background-color:#43a3f2;"
                   "}");
}

QItemSelectionModel::SelectionFlags CategoryListWidget::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event == nullptr)
    {
        return QItemSelectionModel::NoUpdate;
    }
    //禁用用户按住鼠标切换用户情况
    if (event->type() == QEvent::MouseMove)
    {
        return QItemSelectionModel::NoUpdate;
    }
    if ((event->type() == QEvent::MouseButtonPress))
    {
        const QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if ((mouseEvent->modifiers() & Qt::ControlModifier) != 0)
        {
            return QItemSelectionModel::NoUpdate;
        }
    }
    return QListWidget::selectionCommand(index, event);
}
