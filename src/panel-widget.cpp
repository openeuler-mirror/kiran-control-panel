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

#include "panel-widget.h"

#include <qt5-log-i.h>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QTimer>

#include "category-widget.h"
#include "plugin-manager.h"
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
    //左侧分类窗口，不加入布局，浮于窗口上层
    m_categoryWidget = new CategoryWidget(this);
    m_categoryWidget->move(0, 0);
    connect(m_categoryWidget, &CategoryWidget::sigCurrentCategoryChanged,
            this, &PanelWidget::handleCurrentCategoryChanged, Qt::QueuedConnection);

    //内容窗口，右边预留出左侧分类图标模式的宽度
    ui->module_widget->setLeftContentsMargins(m_categoryWidget->iconModeWidth());
}

void PanelWidget::resizeEvent(QResizeEvent *event)
{
    m_categoryWidget->resize(m_categoryWidget->width(), this->height());
    QWidget::resizeEvent(event);
}

void PanelWidget::handleCurrentCategoryChanged(int curCategoryIdx, int prevCategoryIdx)
{
    if( (m_currentCategoryIndex != curCategoryIdx) &&
        (m_currentCategoryIndex!=-1) )
    {
        if( ui->module_widget->checkHasUnSaved() )
        {
            KLOG_DEBUG() << "switch category reject," << prevCategoryIdx << "->" << curCategoryIdx;
            m_categoryWidget->setCurrentCategory(m_currentCategoryIndex);
            return;
        }
    }

    if( m_currentCategoryIndex!=curCategoryIdx )
    {
        KLOG_DEBUG() << "update current category Idx" << curCategoryIdx;
        m_currentCategoryIndex = curCategoryIdx;

        auto categorys = CPanelPluginManager::getInstance()->getCategorys();
        auto category = categorys.at(m_currentCategoryIndex);
        KLOG_DEBUG() << "update module widget for category:" << (category?category->getCategoryDesktopInfo().name:"null");
        ui->module_widget->setPlugins(category?category->getPlugins():QList<QSharedPointer<CPanelPluginHelper>>());
    }
}
