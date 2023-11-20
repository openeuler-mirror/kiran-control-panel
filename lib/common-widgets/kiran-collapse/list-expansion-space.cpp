#include "list-expansion-space.h"
#include "ui_list-expansion-space.h"

ListExpansionSpace::ListExpansionSpace(QWidget *parent) : QWidget(parent),
                                                          ui(new Ui::ListExpansionSpace)
{
    ui->setupUi(this);
    init();
}

ListExpansionSpace::~ListExpansionSpace()
{
    delete ui;
}

void ListExpansionSpace::init()
{
    QPalette pll = this->palette();
    pll.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 0)));
    ui->listWidget->setPalette(pll);
    ui->listWidget->setFrameShape(QListWidget::NoFrame);
}

void ListExpansionSpace::addListExpansionSpaceItem(QWidget *widget)
{
    auto item = new QListWidgetItem();
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, widget);
    emit addedListWidgetItem();
}

void ListExpansionSpace::removeListExpansionSpaceCurrentItem()
{
    auto curItem = ui->listWidget->currentItem();
    ui->listWidget->removeItemWidget(curItem);
    delete curItem;

    emit removedListWidgetItem();
}

void ListExpansionSpace::removeListExpansionSpaceItem(int index)
{
    QListWidgetItem *pItem = ui->listWidget->item(index);
    ui->listWidget->removeItemWidget(pItem);
    delete pItem;
}
