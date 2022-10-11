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

#include "kiran-module-widget.h"
#include "category.h"

#include <qt5-log-i.h>
#include <QDialogButtonBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>

#include "kiranwidgets-qt5/kiran-message-box.h"
#include "ui_kiran-module-widget.h"

#include "category-manager.h"
#include "list-widget-subitem.h"

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
    qDebug() << disconnect(m_category, &Category::subItemAdded, this, &KiranModuleWidget::handleCategorySubItemAdded);
    qDebug() << disconnect(m_category, &Category::subItemDeleted, this, &KiranModuleWidget::handleCategorySubItemDeleted);
    qDebug() << disconnect(m_category, &Category::subItemInfoChanged, this, &KiranModuleWidget::handleCategorySubItemInfoChanged);

    ui->list_subItems->clear();
    ui->widget_siderbar->hide();

    if (m_subItemWidget)
    {
        // 该处若使用 "m_subItemWidge.deleteLater()" 后续加载其他功能项目显示界面时，该功能项界面未被释放会导致后续布局变大的情况
        delete m_subItemWidget;
        m_subItemWidget = nullptr;
    }

    m_category = nullptr;
    m_subitems.clear();
    m_currentSubItem.first = nullptr;
    m_currentSubItem.second.clear();
}

void KiranModuleWidget::init()
{
    ui->list_subItems->setInvertIconPixelsEnable(true);
    ui->list_subItems->setFrameShape(QFrame::NoFrame);
    ui->list_subItems->setSelectionMode(QListWidget::SingleSelection);
    connect(ui->list_subItems, &QListWidget::itemSelectionChanged,
            this, &KiranModuleWidget::handleCurrentItemChanged, Qt::QueuedConnection);
}

void KiranModuleWidget::removeListWidgetItem(KcpPluginSubItemPtr subitem)
{
    auto listwidgetItem = m_subItemsMap.key(subitem);
    int row = ui->list_subItems->row(listwidgetItem);
    delete ui->list_subItems->takeItem(row);
    m_subItemsMap.remove(listwidgetItem);
    m_subitems.removeAll(subitem);
}

void KiranModuleWidget::appendListWidgetItem(KcpPluginSubItemPtr subitem)
{
    QString name = subitem->getName();
    QString desc = subitem->getDesc();
    QString iconPath = subitem->getIcon();
    QIcon icon = QIcon::fromTheme(iconPath);
    QString id = subitem->getID();

    auto item = new ListWidgetSubItem();
    item->setSizeHint(QSize(item->sizeHint().width(), 60));
    item->setText(name);
    item->setWeight(subitem->getWeight());
    if (icon.isNull())
    {
        KLOG_WARNING() << "can't find subitem icon:" << name << icon;
    }
    else
    {
        item->setIcon(icon);
    }
    item->setData(Qt::AccessibleTextRole, QString("ControlPanelSubItem::%1").arg(name));
    ui->list_subItems->addItem(item);

    m_subItemsMap[item] = subitem;
    m_subitems.append(subitem);
}

void KiranModuleWidget::setCategory(Category *c)
{
    setSubItems(c->getSubItems());
    m_category = c;
    connect(m_category, &Category::subItemAdded, this, &KiranModuleWidget::handleCategorySubItemAdded);
    connect(m_category, &Category::subItemDeleted, this, &KiranModuleWidget::handleCategorySubItemDeleted);
    connect(m_category, &Category::subItemInfoChanged, this, &KiranModuleWidget::handleCategorySubItemInfoChanged);
}

void KiranModuleWidget::setSubItems(QVector<KcpPluginSubItemPtr> subitems)
{
    clear();

    for (auto subitem : subitems)
    {
        appendListWidgetItem(subitem);
    }

    ui->widget_siderbar->setVisible(ui->list_subItems->count() > 1);
    ui->list_subItems->sortItems(Qt::DescendingOrder);

    if (ui->list_subItems->count() > 0)
    {
        ui->list_subItems->item(0)->setSelected(true);
        handleCurrentItemChanged();
    }
}

bool KiranModuleWidget::checkHasUnSaved()
{
    if (!m_currentSubItem.second)
    {
        return false;
    }

    return m_currentSubItem.second->haveUnSavedOptions();
}

void KiranModuleWidget::handleCurrentItemChanged()
{
    auto selectedItems = ui->list_subItems->selectedItems();
    if (selectedItems.size() != 1)
    {
        KLOG_ERROR() << "sider bar size != 1";
        return;
    }

    auto selectedItem = selectedItems.at(0);

    if (m_currentSubItem.first == selectedItem)
    {
        KLOG_DEBUG() << "subitem not changed,ignore 'itemSelectionChanged' signal!";
        return;
    }

    auto mapIter = m_subItemsMap.find(selectedItem);
    if (mapIter == m_subItemsMap.end())
    {
        KLOG_WARNING() << "can't find KcpPluginSubItemPtr by QListWidgetItem," << selectedItem->text();
        return;
    }

    KcpPluginSubItemPtr pluginSubitem = *mapIter;
    if (checkHasUnSaved())
    {
        KLOG_DEBUG() << "switch subitem to:" << pluginSubitem->getName() << "reject";
        m_currentSubItem.first->setSelected(true);
        return;
    }

    if (m_subItemWidget)
    {
        ui->centerLayout->removeWidget(m_subItemWidget);
        delete m_subItemWidget;
        m_subItemWidget = nullptr;
    }

    m_currentSubItem.first = selectedItem;
    m_currentSubItem.second = pluginSubitem;

    QWidget *widget = pluginSubitem->createWidget();
    if (widget)
    {
        KLOG_DEBUG() << "sub item widget sizeHint:" << widget->sizeHint();
        ui->centerLayout->addWidget(widget);
    }
    else
    {
        KLOG_ERROR() << "can't get subitem widget:" << pluginSubitem->getName() << pluginSubitem->getID();
    }

    m_subItemWidget = widget;
}

void KiranModuleWidget::handleCategorySubItemAdded(const QString &id)
{
    auto subitem = m_category->getSubItem(id);
    appendListWidgetItem(subitem);
    ui->list_subItems->sortItems(Qt::DescendingOrder);
}

void KiranModuleWidget::handleCategorySubItemDeleted(const QString &id)
{
    for (auto iter = m_subitems.begin(); iter != m_subitems.end(); iter++)
    {
        if ((*iter)->getID() == id)
        {
            removeListWidgetItem(*iter);
            break;
        }
    }
}

// TODO: 完成功能项目信息变更处理函数
void KiranModuleWidget::handleCategorySubItemInfoChanged(const QString &subitemID)
{
}

void KiranModuleWidget::jumpTo(const QString &subItemID, const QString &customKey)
{
    QListWidgetItem *item = nullptr;

    QListWidgetItem *widgetItem = nullptr;
    for (auto subitem : m_subitems)
    {
        if (subitem->getID() == subItemID)
        {
            widgetItem = m_subItemsMap.key(subitem, nullptr);
            break;
        }
    }

    if (item != nullptr)
    {
        item->setSelected(true);
        // clang-format off
        QTimer::singleShot(0, [this,customKey]{
            if ( m_currentSubItem.second != nullptr )
            {
                m_currentSubItem.second->jumpToSearchEntry(customKey);    
            } 
        });
        // clang-format on
    }
}