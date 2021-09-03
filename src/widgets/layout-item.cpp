#include "layout-item.h"
#include <kiran-log/qt5-log-i.h>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "ui_layout-item.h"
LayoutItem::LayoutItem(QWidget *parent) : QWidget(parent),
                                          ui(new Ui::LayoutItem)
{
    ui->setupUi(this);
}

LayoutItem::~LayoutItem()
{
    delete ui;
}

QString LayoutItem::getItemText()
{
    return ui->label->text();
}

void LayoutItem::selectedItemChanged(QString coutryName)
{
    if (coutryName == getItemText())
    {
        setisPressed(true);
    }
    else
    {
        setisPressed(false);
    }
}

void LayoutItem::setItemText(QString text)
{
    ui->label->setText(text);
}

void LayoutItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void LayoutItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

void LayoutItem::setisPressed(bool pressed)
{
    if (m_isPressed == pressed)
        return;
    m_isPressed = pressed;

    this->style()->polish(this);
    this->style()->polish(ui->icon);
}

bool LayoutItem::isPressed() const
{
    return m_isPressed;
}
