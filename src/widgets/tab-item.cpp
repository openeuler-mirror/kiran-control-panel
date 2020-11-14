#include "tab-item.h"
#include "ui_tab-item.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>

TabItem::TabItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabItem),
    m_isSelected(false)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    m_effect = new QGraphicsDropShadowEffect(this);

}

TabItem::~TabItem()
{
    delete ui;
}

void TabItem::setText(const QString &text)
{
    ui->text->setText(text);
}

bool TabItem::isSelected() const
{
    return m_isSelected;
}

const QPixmap* TabItem::pixmap()
{
    return ui->image->pixmap();
}

void TabItem::setSelected(bool isSelected)
{
    if( m_isSelected==isSelected ){
        return;
    }
    m_isSelected = isSelected;
    style()->polish(ui->indicator);
    emit isSelectedChanged(m_isSelected);
}
