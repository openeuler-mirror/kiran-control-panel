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
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QTimer>

#include "category-manager.h"
#include "category-side-bar.h"
#include "kiran-module-widget/kiran-module-widget.h"

PanelWidget::PanelWidget(QWidget* parent)
    : QWidget(parent)
{
    init();
}

PanelWidget::~PanelWidget()
{
}

void PanelWidget::init()
{
    auto layout = new QGridLayout(this);
    layout->setMargin(4);
    layout->setSpacing(4);

    m_categorySideBar = new CategorySideBar(this);
    m_categorySideBar->setFixedWidth(196);
    layout->addWidget(m_categorySideBar, 0, 0);

    m_moduleWidget = new KiranModuleWidget(this);
    layout->addWidget(m_moduleWidget, 0, 1);

    connect(m_categorySideBar, &CategorySideBar::currentCategoryIndexChanged,
            this, &PanelWidget::handleCurrentCategoryChanged, Qt::QueuedConnection);
}

void PanelWidget::handleCurrentCategoryChanged(const QString& prev, const QString& cur)
{
    if (m_currentCategoryID == cur)
    {
        return;
    }

    auto camanager = CategoryManager::instance();

    if (m_moduleWidget->checkHasUnSaved())
    {
        KLOG_DEBUG() << "switch category reject," << prev << "->" << cur;
        m_categorySideBar->setCurrentCategoryID(prev);
        return;
    }

    KLOG_DEBUG() << "switch to category:" << cur;
    m_currentCategoryID = cur;
    Category* category = camanager->getCategory(m_currentCategoryID);
    m_moduleWidget->setCategory(category);
}

void PanelWidget::jumpTo(const QString& categoryID, const QString& subItemID, const QString& customKey)
{
    m_categorySideBar->setCurrentCategoryID(categoryID);
    QTimer::singleShot(0, [this, subItemID, customKey]()
                       { m_moduleWidget->jumpTo(subItemID, customKey); });
}
