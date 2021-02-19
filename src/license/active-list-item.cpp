#include "active-list-item.h"
#include "ui_active-list-item.h"

ActListItem::ActListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActListItem)
{
    ui->setupUi(this);
    iniUI();
}

ActListItem::~ActListItem()
{
    delete ui;
}

void ActListItem::iniUI()
{

}

void ActListItem::setItemText(const QString &text)
{
    ui->label_item_name->setText(text);
}
