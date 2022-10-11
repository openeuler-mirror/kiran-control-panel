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

#include "panel-widget.h"

#include <qt5-log-i.h>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QTimer>

#include "category-manager.h"
#include "category-side-bar.h"
#include "ui_panel-widget.h"

PanelWidget::PanelWidget(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::PanelWidget)
{
    ui->setupUi(this);
    init();
}

PanelWidget::~PanelWidget()
{
    delete ui;
}

void PanelWidget::init()
{
    // 左侧分类窗口，不加入布局，浮于窗口上层
    m_categorySideBar = new CategorySideBar(this);
    m_categorySideBar->move(0, 0);

    connect(m_categorySideBar, &CategorySideBar::currentCategoryIndexChanged,
            this, &PanelWidget::handleCurrentCategoryChanged, Qt::QueuedConnection);

    // 内容窗口，右边预留出左侧分类图标模式的宽度
    ui->module_widget->setLeftContentsMargins(CategorySideBar::reduce_width);
}

void PanelWidget::handleCurrentCategoryChanged(const QString& prev, const QString& cur)
{
    if (m_currentCategoryID == cur)
    {
        return;
    }

    auto camanager = CategoryManager::instance();

    if (ui->module_widget->checkHasUnSaved())
    {
        KLOG_DEBUG() << "switch category reject," << prev << "->" << cur;
        m_categorySideBar->setCurrentCategoryID(prev);
        return;
    }

    KLOG_DEBUG() << "switch to category:" << cur;
    m_currentCategoryID = cur;
    Category* category = camanager->getCategory(m_currentCategoryID);
    ui->module_widget->setCategory(category);
}

void PanelWidget::jumpTo(const QString& categoryID, const QString& subItemID, const QString& customKey)
{
    m_categorySideBar->setCurrentCategoryID(categoryID);
    QTimer::singleShot(0, [this, subItemID, customKey]()
                       { ui->module_widget->jumpTo(subItemID, customKey); });
}
