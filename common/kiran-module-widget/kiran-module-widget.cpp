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

#include "kiran-module-widget.h"
#include <qt5-log-i.h>
#include <QDialogButtonBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>

#include "kiranwidgets-qt5/kiran-message-box.h"
#include "ui_kiran-module-widget.h"

#define ROLE_PLUGIN_HELPER_INDEX Qt::UserRole
#define ROLE_SUBITEM_ID Qt::UserRole + 1

KiranModuleWidget::KiranModuleWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::KiranModuleWidget)
{
    ui->setupUi(this);
    init();
    ui->widget_siderbar->setStyleSheet("QWidget#widget_siderbar{border-right:1px solid rgba(177,177,177,0.1)};");
}

KiranModuleWidget::~KiranModuleWidget()
{
    delete ui;
}

void KiranModuleWidget::setLeftContentsMargins(const int &leftmargin)
{
    ui->horizontalLayout->setContentsMargins(leftmargin, 0, 0, 0);
}

void KiranModuleWidget::clear()
{
    ui->list_subItems->clear();
    m_currentSubItemIdx = -1;
    if (m_subItemWidget)
    {
        m_subItemWidget->deleteLater();
        m_subItemWidget = nullptr;
    }
    m_plugins.clear();
}

void KiranModuleWidget::setPlugins(const QList<QSharedPointer<CPanelPluginHelper>> &plugins)
{
    clear();

    m_plugins = plugins;

    ///填充第二层列表
    for (int i = 0; i < plugins.count(); i++)
    {
        const auto &pluginHelper = plugins.at(i);
        auto pluginDesktopInfo = pluginHelper->getPluginDesktopInfo();
        KLOG_DEBUG() << "plugin:" << pluginDesktopInfo.name;
        auto subItems = pluginDesktopInfo.subItems;
        foreach (auto subItem, subItems)
        {
            KLOG_DEBUG() << "    sub item:" << subItem.name;
            auto *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(subItem.name);
            item->setIcon(QIcon::fromTheme(subItem.icon));
            item->setData(ROLE_PLUGIN_HELPER_INDEX, i);
            item->setData(ROLE_SUBITEM_ID, subItem.id);
            ui->list_subItems->addItem(item);
        }
    }

    if (ui->list_subItems->count() >= 0)
    {
        ui->list_subItems->setCurrentRow(0);
        //NOTE：为了获取一个正确的初始化大小，手动调用槽函数，将功能项第一条的窗口加入显示
        handleSubItemChanged(ui->list_subItems->item(0), nullptr);
    }

    if (ui->list_subItems->count() <= 1)
    {
        ui->list_subItems->hide();
    }
    else
    {
        ui->list_subItems->show();
    }
}

void KiranModuleWidget::init()
{
    //设置item的总占用大小,包括间隙.
    ui->list_subItems->setGridSize(QSize(296, 84));
    ui->list_subItems->setIconSize(QSize(15, 15));

    connect(ui->list_subItems, &QListWidget::currentItemChanged,
            this, &KiranModuleWidget::handleSubItemChanged, Qt::QueuedConnection);
}

bool KiranModuleWidget::checkHasUnSaved()
{
    if (m_currentSubItemIdx == -1)
    {
        return false;
    }
    QString subItemName = ui->list_subItems->item(m_currentSubItemIdx)->text();
    auto item = ui->list_subItems->item(m_currentSubItemIdx);
    int pluginIdx = item->data(ROLE_PLUGIN_HELPER_INDEX).toInt();
    bool haveUnsavedOptions = m_plugins.at(pluginIdx)->haveUnsavedOptions();

    if (haveUnsavedOptions)
    {
        auto clickedButton = KiranMessageBox::message(this, tr("Warning"),
                                                      QString("The edited content in %1 is not saved."
                                                              " After switching, the edited content will be lost."
                                                              " Are you sure you want to save？")
                                                          .arg(subItemName),
                                                      KiranMessageBox::Yes | KiranMessageBox::No);
        if( clickedButton==KiranMessageBox::No )
        {
            haveUnsavedOptions = false;
        }
    }
    return haveUnsavedOptions;
}

///NOTE:此处最好不拿prev参数作为调用QListWidget的参数，可能存在prev item已从列表中删除
void KiranModuleWidget::handleSubItemChanged(QListWidgetItem *current, QListWidgetItem *prev)
{
    int currentIdx = current ? ui->list_subItems->row(current) : -1;

    ///当前子功能项未改变
    if (m_currentSubItemIdx == currentIdx)
    {
        return;
    }

    ///检查是否存在未保存项
    if (checkHasUnSaved())
    {
        KLOG_DEBUG() << "switch sub item" << currentIdx << "reject";
        ui->list_subItems->setCurrentItem(ui->list_subItems->item(m_currentSubItemIdx));
        return;
    }

    ///更新当前选择的子功能项
    KLOG_DEBUG() << "update current sub item Idx" << currentIdx;
    m_currentSubItemIdx = currentIdx;

    ///清理之前的功能项控件
    if (m_subItemWidget)
    {
        ui->centerLayout->removeWidget(m_subItemWidget);
        delete m_subItemWidget;
        m_subItemWidget = nullptr;
    }

    if (!current)
    {
        return;
    }

    ///调用接口获取新的功能项控件
    QVariant varPluginIndex = current->data(ROLE_PLUGIN_HELPER_INDEX);
    QVariant varSubItemID = current->data(ROLE_SUBITEM_ID);
    if (!varPluginIndex.isValid() || !varSubItemID.isValid())
    {
        KLOG_FATAL("plugin subitem data error!");
    }

    bool toIntOk = false;
    int pluginIndex = varPluginIndex.toInt(&toIntOk);
    if (!toIntOk)
    {
        KLOG_FATAL("plugin subitem data error!");
    }
    QString subItemID = varSubItemID.toString();

    QWidget *widget = m_plugins.at(pluginIndex)->getSubItemWidget(subItemID);
    if (widget)
    {
        KLOG_DEBUG() << "sub item widget sizeHint:" << widget->sizeHint();
        ui->centerLayout->addWidget(widget);
    }
    m_subItemWidget = widget;
}
