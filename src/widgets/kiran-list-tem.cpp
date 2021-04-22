#include "kiran-list-item.h"
#include "ui_kiran-list-item.h"

KiranListItem::KiranListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranListItem)
{
    ui->setupUi(this);
}

KiranListItem::~KiranListItem()
{
    delete ui;
}

void KiranListItem::setText(QString text)
{
    ui->text->setText(text);
}

void KiranListItem::setItemArrow(bool isSelected)
{
    if(isSelected)
        ui->arrow->setStyleSheet("QLabel#arrow{border-image:url(:/images/arrow.svg)};");
    else
        ui->arrow->setStyleSheet("QLabel#arrow{border-image:none");
}

