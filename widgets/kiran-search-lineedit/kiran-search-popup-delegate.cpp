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
#include <QDebug>

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

    // QStyledItemDelegate::paint(painter, itemOption, index); //!-- 保持原有风格不变

    paintKeywordHighlight( painter,itemOption,pStyle,index );
}

void KiranSearchPopupDelegate::paintKeywordHighlight(QPainter *painter, const QStyleOptionViewItem &itemOption, QStyle *pStyle, const QModelIndex &index) const
{
    painter->save();
    QString text = index.data().toString();
    int indexReg = text.indexOf( m_regFindKeyWords );
    QFontMetrics  fontMetrics = painter->fontMetrics(); //!--- 得到这个painter的像素
    int regTextWidth = fontMetrics.width(m_regFindKeyWords);
    int regTextHeight = itemOption.rect.height();
    QStyleOptionViewItem option = itemOption;
    //    if (option.state & QStyle::State_Selected)
    //    {
    //        //!--- 选中样式
    //        option.backgroundBrush = QBrush(Qt::red);
    //    }

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

    //            const QWidget *m_QWidget = itemOption.widget;
    //    QStyle *m_QStyle = m_QWidget ? m_QWidget->style() : QApplication::style(); //!--- 得到当前的style
    //    QFontMetrics  m_QFontMetrics = painter->fontMetrics(); //!--- 得到这个painter的像素
    //    QString indexColString = ( index.model()->data(index,Qt::DisplayRole).toString() );

    //    QRect m_QRect = m_QStyle->subElementRect( QStyle::SE_ItemViewItemText,&itemOption,m_QWidget );//!--- 得到Item的自己的Rect

    //    QPalette::ColorRole textDisplayRole = QPalette::NoRole; //!--- 设置text的role
    //    if (itemOption.state & QStyle::State_Selected)
    //    {
    //        //!--- 当选中字体的时候字体显示高亮
    //        textDisplayRole = QPalette::HighlightedText;
    //    }
    //    int ItemHeight = itemOption.rect.height();
    //    painter->translate( itemOption.rect.x(),itemOption.rect.y() );
    //    painter->setFont(QApplication::font(itemOption.widget));
    //    painter->setPen(itemOption.palette.brush(QPalette::Text).color());
    //    int indexFindLeft = indexColString.indexOf( m_regFindKeyWords ); //!--- 得到查找字体在当前整个Item字体中的位置

    //    int findKeyWordWidth = m_QFontMetrics.width(m_regFindKeyWords); //!--- 得到查找字体的像素宽度
    //    int colTextWidth = m_QFontMetrics.width( itemOption.text ); //!--- 得到整个字体的像素宽度
    //    int preFindKeyWordWidth = m_QFontMetrics.width( indexColString.mid(0,indexFindLeft) ); //!-- 得到查找字体前面的字体的像素宽度

    //    QString drawLine = m_QFontMetrics.elidedText( indexColString, Qt::ElideMiddle, m_QRect.width()); //!--- 当字体超过Item的长度时显示为省略号

    //    //!--- 以下为绘制关键字
    //    if( index.column() >=0){
    //        if( indexFindLeft < 0 || m_regFindKeyWords == ""){
    //            this->displayText(indexColString,QLocale::Chinese);
    //        }
    //        if( indexFindLeft >= 0 && m_regFindKeyWords != ""){
    //            for( int i = 0 ; i < indexColString.length();++i){
    //                if( i < indexFindLeft  ){
    //                    this->displayText(drawLine.mid(0,indexFindLeft),QLocale::Chinese);
    //                    i = indexFindLeft;
    //                }else if(  i > ( indexFindLeft - 1 + m_regFindKeyWords.length() ) ){
    //                    this->displayText( drawLine.mid(indexFindLeft+m_regFindKeyWords.length(),indexColString.length()-m_regFindKeyWords.length()-indexFindLeft ),QLocale::Chinese);
    //                    i = indexColString.length();
    //                }else{
    //                    painter->setPen("#43a3f2");

    //                    if( colTextWidth == findKeyWordWidth ){
    //                        m_QStyle->drawItemText( painter,QRect( (itemOption.rect.width()-findKeyWordWidth)/2,0,findKeyWordWidth,ItemHeight),
    //                                                itemOption.displayAlignment ,QApplication::palette(),true, drawLine.mid( indexFindLeft,m_regFindKeyWords.length()),textDisplayRole );
    //                    }else if( colTextWidth < itemOption.rect.width() && colTextWidth != findKeyWordWidth ){
    //                        int repaintTextWidth = ( (itemOption.rect.width()-colTextWidth)/2+preFindKeyWordWidth);
    //                        m_QStyle->drawItemText( painter,QRect( repaintTextWidth,0,findKeyWordWidth,ItemHeight),
    //                                                itemOption.displayAlignment ,QApplication::palette(),true, drawLine.mid( indexFindLeft,m_regFindKeyWords.length()),textDisplayRole );
    //                    }else if( colTextWidth > itemOption.rect.width() ){
    //                        m_QStyle->drawItemText( painter,QRect( preFindKeyWordWidth+1,0,findKeyWordWidth,ItemHeight),
    //                                                itemOption.displayAlignment ,QApplication::palette(),true, drawLine.mid( indexFindLeft,m_regFindKeyWords.length()),textDisplayRole );
    //                    }

    //                    i = indexFindLeft + m_regFindKeyWords.length();
    //                }
    //            }
    //        }
    //    }
}

//QSize KiranSearchPopupDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    return QSize(160, 60);
//}
