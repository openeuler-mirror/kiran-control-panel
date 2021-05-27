/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-search-popup-delegate.h"
#include <QPainter>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

KiranSearchPopupDelegate::KiranSearchPopupDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

KiranSearchPopupDelegate::~KiranSearchPopupDelegate()
{
}

void KiranSearchPopupDelegate::setSearchText(const QString &text)
{
    m_regFindKeyWords = text;
}

void KiranSearchPopupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption = option;

    initStyleOption( &itemOption,index );
    if (itemOption.state & QStyle::State_HasFocus)
    {
        //!--- 去掉选中时候的Item的虚线框，也就是焦点
        itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
    }
    QStyle *pStyle = itemOption.widget? itemOption.widget->style() : QApplication::style();
    //原文字不绘出
    itemOption.text = "";
    //保持原有风格不变
    pStyle->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, itemOption.widget);

    paintKeywordHighlight( painter,itemOption,pStyle,index );
}

void KiranSearchPopupDelegate::paintKeywordHighlight(QPainter *painter, const QStyleOptionViewItem &itemOption, QStyle *pStyle, const QModelIndex &index) const
{
    painter->save();

    QString text = index.data().toString();
    int indexReg = text.indexOf( m_regFindKeyWords );
    QFontMetrics  fontMetrics = painter->fontMetrics();
    int regTextWidth = fontMetrics.width(m_regFindKeyWords);
    int regTextHeight = itemOption.rect.height();
    QStyleOptionViewItem option = itemOption;

    if(indexReg < 0)
    {
        this->displayText(text, QLocale::Chinese);
    }
    else
    {
        int startX = 10;
        if(indexReg != 0)
        {
            painter->setPen("#ffffff");
            QString leftText = text.left(indexReg);
            int leftTextWidth = fontMetrics.width(leftText);
            //itemOption.rect表示当前item相对于父窗口listView的位置。
            pStyle->drawItemText(painter, QRect(startX, itemOption.rect.y(), leftTextWidth, regTextHeight),
                                 option.displayAlignment ,QApplication::palette(),true, leftText );
            startX += leftTextWidth;
        }


        painter->setPen("#43a3f2");
        pStyle->drawItemText(painter, QRect(startX, itemOption.rect.y(), regTextWidth, regTextHeight),
                             option.displayAlignment ,QApplication::palette(),true, m_regFindKeyWords );
        startX += regTextWidth;

        if((indexReg+m_regFindKeyWords.length()) < text.length())
        {
            QString rightText = text.right(text.length()-(indexReg+m_regFindKeyWords.length()));
            painter->setPen("#ffffff");
            int rigthTextWidth = fontMetrics.width(rightText);
            pStyle->drawItemText(painter, QRect(startX, itemOption.rect.y(), rigthTextWidth, regTextHeight),
                                 option.displayAlignment ,QApplication::palette(),true, rightText );
        }
    }

    painter->restore();
}