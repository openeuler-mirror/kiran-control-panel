#include "kiran-mode-class-delegate.h"
#include <QApplication>
#include <QPainter>
#include <QPushButton>
void KiranModeClassDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    if (option.state.testFlag(QStyle::State_HasFocus))
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, viewOption, index);

    // 绘制按钮
    QStyleOptionButton button;
    button.rect = QRect(0, 0, 60, 60);
    button.state |= QStyle::State_Enabled;
    button.text ="fdsfsdffds";
    button.iconSize = QSize(50, 50);
    button.icon = QIcon("/root/untitled/test.png");//QIcon(QString(":/Images/%1").arg(m_list.at(i)));

    button.state |= QStyle::State_Selected;

//    if (1/*m_nType == 0*/)
//    {
//        button.state |= QStyle::State_Selected;
//        //button.icon = QIcon(QString(":/Images/%1Hover").arg(m_list.at(i)));
//    }
//    else if (0/*m_nType == 1*/)
//    {
//        button.state |= QStyle::State_Sunken;
//        //button.icon = QIcon(QString(":/Images/%1Pressed").arg(m_list.at(i)));
//    }
    //QWidget *pWidget = (i == 0) ? m_pOpenButton.data() : m_pDeleteButton.data();
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter/*, pWidget*/);
}

//! [1]
QSize KiranModeClassDelegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    return QSize(60, 60);
}
//! [1]
