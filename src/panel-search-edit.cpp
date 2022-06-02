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


#include "panel-search-edit.h"
#include "plugin-manager.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QCompleter>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QSpacerItem>
#include <QStandardItemModel>

#define MODEL_ROLE_CATEGORY_IDX Qt::UserRole
#define MODEL_ROLE_SUBITEM_IDX Qt::UserRole + 2
#define MODEL_ROLE_PATH Qt::UserRole + 3

CPanelSearchDelegate::CPanelSearchDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

CPanelSearchDelegate::~CPanelSearchDelegate()
{
}

void CPanelSearchDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

CPanelSearchEdit::CPanelSearchEdit(QWidget *parent)
    : QLineEdit(parent)
{
    init();
}

CPanelSearchEdit::~CPanelSearchEdit()
{
}

void CPanelSearchEdit::init()
{
    setObjectName("edit_search");

    //为搜索文本预留位置
    setTextMargins(30, 0, 10, 0);

    //控件和布局初始化
    auto *searchIcon = new QLabel(this);
    searchIcon->setMaximumSize(20, 20);
    searchIcon->setCursor(QCursor(Qt::ArrowCursor));
    searchIcon->setPixmap(QPixmap(":/kiran-control-panel/images/search.svg"));

    auto *spacerItem = new QSpacerItem(322, 40, QSizePolicy::Expanding);

    auto *layout = new QHBoxLayout();
    layout->setContentsMargins(10, 0, 0, 0);
    layout->addWidget(searchIcon);
    layout->addSpacerItem(spacerItem);
    setLayout(layout);

    ///填充搜索model
    m_searchModel = buildSearchModel();

    m_completer = new QCompleter(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setModel(m_searchModel);

    m_completer->popup()->setObjectName("popup_searchEdit");
    m_completer->popup()->installEventFilter(this);
    setCompleter(m_completer);

    setSearchPopupVisible(false);
}

QStandardItemModel *CPanelSearchEdit::buildSearchModel()
{
    auto categorys = CPanelPluginManager::getInstance()->getCategorys();
    auto itemModel = new QStandardItemModel(this);
    auto addSearchItemsFunc = [](QStandardItemModel *model,
                                 QStringList searchKeys,
                                 QString icon,
                                 QString tooltip,
                                 QString path,
                                 int categoryIdx,
                                 int subItemIdx) -> void {
        foreach (auto searchKey, searchKeys)
        {
            auto *newItem = new QStandardItem;
            newItem->setText(searchKey);
            newItem->setIcon(QIcon(icon));
            newItem->setToolTip(tooltip);
            newItem->setData(categoryIdx, MODEL_ROLE_CATEGORY_IDX);
            newItem->setData(subItemIdx, MODEL_ROLE_SUBITEM_IDX);
            newItem->setData(path + "->" + searchKey, MODEL_ROLE_PATH);
            model->appendRow(newItem);
        }
    };

    for (int i = 0; i < categorys.count(); i++)
    {
        auto category = categorys.at(i);
        auto categoryInfo = category->getCategoryDesktopInfo();
        auto plugins = category->getPlugins();
        int categorySubItemCount = 0;
        addSearchItemsFunc(itemModel, category->getCategoryDesktopInfo().keywords, categoryInfo.icon, categoryInfo.comment, "", i, -1);
        foreach (auto plugin, plugins)
        {
            auto pluginInfo = plugin->getPluginDesktopInfo();
            foreach (auto subItem, pluginInfo.subItems)
            {
                addSearchItemsFunc(itemModel, subItem.keywords, subItem.icon, "", categoryInfo.name, i, categorySubItemCount++);
            }
        }
    }

    return itemModel;
}

///设置搜索弹出框是否可见的属性，提供给stylesheet
void CPanelSearchEdit::setSearchPopupVisible(bool searchPopupVisible)
{
    setProperty("searchPopupVisible", searchPopupVisible);
    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();
}

///事件过滤器
bool CPanelSearchEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_completer->popup())
    {
        ///过滤自动补全弹出的显示和隐藏事件，同步设置QWidget动态属性，以便更新相应的stylesheet
        if (event->type() == QEvent::Show)
        {
            setSearchPopupVisible(true);
        }
        else if (event->type() == QEvent::Hide)
        {
            setSearchPopupVisible(false);
        }
    }
    return QObject::eventFilter(watched, event);
}
