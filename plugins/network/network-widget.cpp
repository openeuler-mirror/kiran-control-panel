/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include "network-widget.h"
#include <kiran-sidebar-item.h>
#include <NetworkManagerQt/Manager>
#include "connection-manager.h"
#include "details/details-page.h"
#include "ethernet/ethernet-page.h"
#include "logging-category.h"
#include "ui_network-widget.h"
#include "vpn/vpn-page.h"
#include "wireless/wireless-page.h"

enum SideBarDataRole
{
    SIDEBAR_DATA_ROLE_KEY = Qt::UserRole + 1,
    SIDEBAR_DATA_ROLE_PAGE_ENUM,
    SIDEBAR_DATA_ROLE_LAST
};

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
Widget::Widget(QWidget *parent) : QWidget(parent),
                                  ui(new ::Ui::Widget)
{
    ui->setupUi(this);
    ui->sidebar->viewport()->setAutoFillBackground(false);
    init();
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::jumpToSearchEntry(const QString &key)
{
    for (auto item : ui->sidebar->findItems("", Qt::MatchContains))
    {
        auto itemKey = item->data(SIDEBAR_DATA_ROLE_KEY).toString();
        if (itemKey == key)
        {
            ui->sidebar->setCurrentItem(item);
            return true;
        }
    }
    return false;
}

void Widget::init()
{
    connect(ui->sidebar, &KiranSidebarWidget::currentItemChanged, this, [this](QListWidgetItem *current, QListWidgetItem *previous)
            {
        if ( current && ui->sidebar->row(current) >= 0 )
        {
            auto idx = ui->sidebar->row(current);
            auto widget = ui->stacked->widget(idx);
            auto page = qobject_cast<Page*>(widget);
            page->reset();        
            ui->stacked->setCurrentIndex(idx);
        } });

    // 感知设备新增/删除，动态调整侧边栏选项
    connect(PM_INSTANCE, &PageManager::pageAvailableChanged, this, &Widget::updateSubPage);
    updateSubPage();

    ui->sidebar->setCurrentRow(0);
}

void Widget::updateSubPage()
{
    auto manager = PM_INSTANCE;
    bool updateCurrentPage = false;

    // 统计可用子功能页
    QList<PageType> avaliableTypes;
    for (int i = 0; i < PAGE_LAST; i++)
    {
        auto pageType = static_cast<PageType>(i);
        auto pagepAvaliable = manager->isAvaliable(pageType);
        if (pagepAvaliable)
        {
            avaliableTypes << pageType;
        }
    }

    // 去除不可用页
    QList<PageType> newPageTypes = avaliableTypes;
    for (int i = ui->sidebar->count() - 1; i >= 0; --i)
    {
        auto item = ui->sidebar->item(i);
        if( item == ui->sidebar->currentItem() )
        {
            updateCurrentPage = true;
        }

        auto itemType = static_cast<PageType>(item->data(SIDEBAR_DATA_ROLE_PAGE_ENUM).toInt());
        if (!avaliableTypes.contains(itemType))
        {
            auto pageIdx = ui->sidebar->row(item);
            delete item;

            auto page = ui->stacked->widget(pageIdx);
            ui->stacked->removeWidget(page);
            delete page;
        }
        else
        {
            newPageTypes.removeAll(itemType);
        }
    }

    // 添加新增的页面
    for (auto newPageType : newPageTypes)
    {
        // 根据类型找到页面插入位置
        int insertPos = ui->sidebar->count() ? ui->sidebar->count() : 0;
        for (int i = 0; i < ui->sidebar->count(); i++)
        {
            auto itemType = static_cast<PageType>(ui->sidebar->item(i)->data(SIDEBAR_DATA_ROLE_PAGE_ENUM).toInt());
            if (newPageType < itemType)
            {
                insertPos = i;
                break;
            }
        }

        auto pageInfo = manager->description(newPageType);
        
        // 同步新增侧边栏以及StackedWidget，保持顺序
        // 此处Item新增不能指定Parent否则会排列最后,可见QListWidget::insertItem方法
        auto newSubItem = new KiranSidebarItem(QIcon::fromTheme(pageInfo.icon),
                                               pageInfo.name);
        newSubItem->setData(SIDEBAR_DATA_ROLE_KEY, pageInfo.key);
        newSubItem->setData(SIDEBAR_DATA_ROLE_PAGE_ENUM, newPageType);
        ui->sidebar->insertItem(insertPos, newSubItem);

        auto page = manager->create(newPageType);
        page->setParent(this);
        ui->stacked->insertWidget(insertPos, page);
    }

    if( updateCurrentPage )
    {
        ui->sidebar->setCurrentRow(0);
    }
}
}  // namespace Network
}  // namespace Kiran