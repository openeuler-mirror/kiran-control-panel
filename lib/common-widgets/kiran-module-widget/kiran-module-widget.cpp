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
    disconnect(this, SIGNAL(handlePluginVisibleSubItemsChanged()));

    ui->list_subItems->clear();
    ui->widget_siderbar->hide();

    m_currentSubItemIdx = -1;
    if (m_subItemWidget)
    {
        m_subItemWidget->removeEventFilter(this);
        // 该处若使用 "m_subItemWidge.deleteLater()" 会导致后续布局变大的情况
        delete m_subItemWidget;
        m_subItemWidget = nullptr;
    }

    for (auto plugin : m_plugins)
    {
        disconnect(plugin.data(), &PluginHelper::visibleSubItemsChanged,
                   this, &KiranModuleWidget::handlePluginVisibleSubItemsChanged);
    }
    m_plugins.clear();
}

void KiranModuleWidget::setPlugins(const PluginHelperPtrList &plugins)
{
    clear();

    m_plugins = plugins;

    /// 填充第二层列表
    for (int i = 0; i < plugins.count(); i++)
    {
        auto pluginHelper = plugins.at(i);
        connect(pluginHelper.data(), &PluginHelper::visibleSubItemsChanged, this, &KiranModuleWidget::handlePluginVisibleSubItemsChanged);
        auto visibleSubItems = pluginHelper->visibleSubItems();
        auto pluginDesktopInfo = pluginHelper->getPluginDesktopInfo();
        auto subItems = pluginDesktopInfo.subItems;
        foreach (auto subItem, subItems)
        {
            if (!visibleSubItems.contains(subItem.id))
                continue;

            KLOG_DEBUG() << "    sub item:" << subItem.name;
            auto *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), 60));
            item->setText(subItem.name);
            QIcon icon = QIcon::fromTheme(subItem.icon);
            if (!icon.isNull())
            {
                item->setIcon(icon);
            }
            else
            {
                KLOG_WARNING() << "can't find item icon:" << pluginDesktopInfo.name << " -> " << subItem.name;
            }
            item->setData(ROLE_PLUGIN_HELPER_INDEX, i);
            item->setData(ROLE_SUBITEM_ID, subItem.id);
            ui->list_subItems->addItem(item);
        }
    }

    ui->widget_siderbar->setVisible(ui->list_subItems->count() > 1);

    if (ui->list_subItems->count() > 0)
    {
        ui->list_subItems->item(0)->setSelected(true);
        // NOTE：为了获取一个正确的初始化大小，手动调用槽函数，将功能项第一条的窗口加入显示
        handleCurrentItemChanged();
    }
}

void KiranModuleWidget::init()
{
    ui->list_subItems->setInvertIconPixelsEnable(true);
    ui->list_subItems->setFrameShape(QFrame::NoFrame);
    ui->list_subItems->setSelectionMode(QListWidget::SingleSelection);
    connect(ui->list_subItems, &QListWidget::itemSelectionChanged,
            this, &KiranModuleWidget::handleCurrentItemChanged, Qt::QueuedConnection);
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
                                                      QString(tr("The edited content in %1 is not saved."
                                                              " After switching, the edited content will be lost."
                                                              " Are you sure you want to save？"))
                                                          .arg(subItemName),
                                                      KiranMessageBox::Yes | KiranMessageBox::No);
        if (clickedButton == KiranMessageBox::No)
        {
            haveUnsavedOptions = false;
        }
    }
    return haveUnsavedOptions;
}

/// NOTE:此处最好不拿prev参数作为调用QListWidget的参数，可能存在prev item已从列表中删除
void KiranModuleWidget::handleCurrentItemChanged()
{
    auto selectedItems = ui->list_subItems->selectedItems();
    if (selectedItems.size() != 1)
    {
        KLOG_ERROR() << "sider bar size != 1";
        return;
    }

    auto selectedItem = selectedItems.at(0);
    int row = ui->list_subItems->row(selectedItem);

    /// 当前子功能项未改变
    if (m_currentSubItemIdx == row)
    {
        KLOG_DEBUG() << "sub item index not changed!";
        return;
    }

    /// 检查是否存在未保存项
    if (checkHasUnSaved())
    {
        KLOG_DEBUG() << "switch sub item" << row << "reject";
        ui->list_subItems->item(m_currentSubItemIdx)->setSelected(true);
        return;
    }

    /// 更新当前选择的子功能项
    KLOG_DEBUG() << "update current sub item Idx" << row;
    m_currentSubItemIdx = row;

    /// 清理之前的功能项控件
    if (m_subItemWidget)
    {
        m_subItemWidget->removeEventFilter(this);
        ui->centerLayout->removeWidget(m_subItemWidget);
        delete m_subItemWidget;
        m_subItemWidget = nullptr;
    }

    if (!selectedItem)
    {
        return;
    }

    /// 调用接口获取新的功能项控件
    QVariant varPluginIndex = selectedItem->data(ROLE_PLUGIN_HELPER_INDEX);
    QVariant varSubItemID = selectedItem->data(ROLE_SUBITEM_ID);
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
    else
    {
        KLOG_ERROR() << "can't get subitem widget" << subItemID;
    }
    m_subItemWidget = widget;
    m_subItemWidget->installEventFilter(this);
}

void KiranModuleWidget::handlePluginVisibleSubItemsChanged()
{
    setPlugins(PluginHelperPtrList(m_plugins));
    /// 后续单独处理子项新增以及删除条目，不再重新刷新
    /// send()->PluginHelper 拿到PluginHelper的索引
    /// 通过ROLE_PLUGIN_HELPER_INDEX找到该插件对应的节点，再比对新增删除
}

void KiranModuleWidget::jumpTo(const QString &subItemID)
{
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < ui->list_subItems->count(); i++)
    {
        auto temp = ui->list_subItems->item(i);
        if (temp->data(ROLE_SUBITEM_ID).toString() == subItemID)
        {
            item = temp;
            break;
        }
    }

    if (item != nullptr)
    {
        item->setSelected(true);
    }
}

#include <QEvent>
#include <QResizeEvent>
bool KiranModuleWidget::eventFilter(QObject *watched, QEvent *event)
{
    if( watched == m_subItemWidget )
    {
        if( event->type() == QEvent::Resize )
        {
            QResizeEvent* qResizeEvent = dynamic_cast<QResizeEvent*>(event);
            KLOG_DEBUG() << "---> sub item resize event:" << qResizeEvent->size();
        }
    }
    return QWidget::eventFilter(watched,event);
}