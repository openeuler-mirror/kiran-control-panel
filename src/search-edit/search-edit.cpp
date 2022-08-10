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

#include "search-edit.h"
#include "plugin-manager.h"
#include "search-delegate.h"
#include "search-model.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QCompleter>
#include <QEvent>
#include <QTimer>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <kiran-message-box.h>

SearchEdit::SearchEdit(QWidget *parent)
    : KiranSearchBox(parent)
{
    init();
}

SearchEdit::~SearchEdit()
{
}

void SearchEdit::init()
{
    m_searchModel = new SearchModel(this);
    m_searchDelegate = new SearchDelegate(this);

    m_completer = new QCompleter(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setModel(m_searchModel);
    m_completer->setFilterMode(Qt::MatchContains);

    m_completer->popup()->setItemDelegate(m_searchDelegate);
    m_completer->setWidget(this);
    m_completer->installEventFilter(this);

    setCompleter(m_completer);
    // clang-format off
    connect(m_completer, QOverload<const QModelIndex &>::of(&QCompleter::activated), [this](const QModelIndex &index) {
        if( !index.isValid() )
        {
            return ;
        }
        QString categoryID,subItemID;
        auto filterModel = m_completer->completionModel();
        categoryID = filterModel->data(index,SearchModel::roleCategoryID).toString();
        subItemID = filterModel->data(index,SearchModel::roleSubItemID).toString();
        emit requestJumpTo(categoryID,subItemID);
        QTimer::singleShot(0,this,&QLineEdit::clear);
    });
    connect(this,&QLineEdit::returnPressed,[this](){
        QString searchkey = text();
        if( searchkey.isEmpty() )
        {
            return;
        }

        auto items = m_searchModel->findItems(searchkey);
        if( items.isEmpty() )
        {
            auto clickedButton = KiranMessageBox::message(this,tr("Info"),tr("Failed to find related items, please re-enter!"),KiranMessageBox::Ok|KiranMessageBox::No);
            if( clickedButton == KiranMessageBox::Ok )
                clear();
            return;
        }

        auto item = items.at(0);
        auto categoryID = item->data(SearchModel::roleCategoryID).toString();
        auto subItemID = item->data(SearchModel::roleSubItemID).toString();
        clear();
        
        emit requestJumpTo(categoryID,subItemID);
    });
    // clang-format on
}