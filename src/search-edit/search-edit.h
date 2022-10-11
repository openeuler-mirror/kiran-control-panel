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

#ifndef KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
#define KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_

#include <QLineEdit>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QStyleOption>

#include <kiran-search-box.h>

class QStandardItemModel;
class QCompleter;
class QListView;
class SearchModel;
class SearchDelegate;
class SearchEdit : public KiranSearchBox
{
    Q_OBJECT
public:
    explicit SearchEdit(QWidget* parent = nullptr);
    ~SearchEdit();

private:
    void init();
    QStandardItemModel* buildSearchModel();
    void setSearchPopupVisible(bool searchPopupVisible);

signals:
    void requestJumpTo(const QString& categoryID, const QString& subItemID, const QString& customKey = QString());

private:
    SearchModel* m_searchModel = nullptr;
    SearchDelegate* m_searchDelegate = nullptr;
    QCompleter* m_completer = nullptr;
};

#endif  //KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
