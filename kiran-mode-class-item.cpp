#include "kiran-mode-class-item.h"
#include "ui_kiran-mode-class-item.h"

KiranModeClassItem::KiranModeClassItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranModeClassItem)
{
    ui->setupUi(this);
    ui->label_icon->setGeometry(cClassItemPadding, cClassItemPadding, cClassItemIconWd, cClassItemIconWd);
    ui->label_text->setGeometry(cClassItemPadding + cClassItemIconWd + cClassItemSpace, cClassItemPadding, cClassItemTextWd, cClassItemIconWd);

    setTextVisible(false);
    setStyleSheet("");
}

KiranModeClassItem::~KiranModeClassItem()
{
    delete ui;
}

void KiranModeClassItem::setText(const QString &text)
{
    ui->label_text->setText(text);
}

void KiranModeClassItem::setIcon(const QString &icon)
{
    ui->label_icon->setPixmap(QPixmap(icon));
}

void KiranModeClassItem::setTextVisible(const bool &visible)
{
    if(visible)
    {
        ui->frame->setFixedSize(textModeWd(), heightInt());
    }
    else
    {
        ui->frame->setFixedSize(iconModeWd(), heightInt());
    }

    ui->label_text->setVisible(visible);
}
//#include <QPainter>
//void KiranModeClassItem::paintEvent(QPaintEvent* )
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}

int KiranModeClassItem::textModeWd()
{
    return 2*cClassItemPadding + cClassItemSpace + cClassItemIconWd + cClassItemTextWd;
}

int KiranModeClassItem::iconModeWd()
{
    return 2*cClassItemPadding + cClassItemIconWd;
}

int KiranModeClassItem::heightInt()
{
    return 2*cClassItemPadding+cClassItemIconWd;
}
