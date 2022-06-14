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

#ifndef KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
#define KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_

#include <QLineEdit>
#include <QItemDelegate>

//TODO:完成控制面板搜索

class CPanelSearchDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CPanelSearchDelegate(QObject* parent = nullptr);
    ~CPanelSearchDelegate();

public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class QStandardItemModel;
class QCompleter;
class QListView;
class CPanelSearchEdit : public QLineEdit
{
    Q_OBJECT
public:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit CPanelSearchEdit(QWidget* parent = nullptr);
    ~CPanelSearchEdit();

private:
    void init();
    QStandardItemModel* buildSearchModel();
    void setSearchPopupVisible(bool searchPopupVisible);

signals:
    void sigJump(int categoryIdx,int subItemIdx);

private:
    QStandardItemModel* m_searchModel = nullptr;
    QCompleter* m_completer = nullptr;
};

#endif  //KIRAN_CONTROL_PANEL_SRC_PANEL_SEARCH_EDIT_H_
