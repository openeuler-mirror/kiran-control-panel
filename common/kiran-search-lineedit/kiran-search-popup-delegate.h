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

#ifndef KIRANSEARCHPOPUPDELEGATE_H
#define KIRANSEARCHPOPUPDELEGATE_H

#include <QStyledItemDelegate>

class StudentListViewModel;
class StudentFrame;

class KiranSearchPopupDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit KiranSearchPopupDelegate(QObject *parent = 0);
    ~KiranSearchPopupDelegate();
    void setSearchText(const QString &text);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QString m_regFindKeyWords;
    void paintKeywordHighlight(QPainter *painter, const QStyleOptionViewItem &itemOption, QStyle *pStyle, const QModelIndex &index) const;
    //QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // KIRANSEARCHPOPUPDELEGATE_H
