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

#ifndef KIRANMODULECLASSLISTWIDGETITEMWIDGET_H
#define KIRANMODULECLASSLISTWIDGETITEMWIDGET_H

#include <QEvent>
#include <QWidget>

const int cListWidgetLeftPadding = 12;   //ListWidget内空间左右的边距。
const int cListWidgetRightPadding = 20;  //ListWidget内空间左右的边距。
const int cClassItemIconWd = 40;         //按钮未选中时的大小.
const int cClassItemWgtTBPadding = 10;   //按钮选中后上下的颜色边距.
const int cClassItemWgtLRPadding = 12;   //按钮选中后左右的颜色边距.
const int cClassItemTextWd = 166;        //文字部分的宽度.
const int cClassItemSpace = 15;          //item按钮之间的距离.
const int cClassItemMargin = 0;          //

class CategoryListWidgetItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CategoryListWidgetItemWidget(QWidget *parent = nullptr);

    void setText(const QString &text);

    void setIcon(const QString &icon);
    static int textModeWd();
    static int iconModeWd();
    static int heightInt();

private:
    void paintEvent(QPaintEvent *event);
    QPixmap pixmapToRound(const QPixmap &src, const QSize &size, int radius);

private:
    QString m_text;
    QPixmap m_icon;
    QRect m_iconRect;
    QRect m_textRect;
    bool m_showText;
};

#endif  // KIRANMODULECLASSLISTWIDGETITEMWIDGET_H
