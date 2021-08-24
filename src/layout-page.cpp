#include "layout-page.h"
#include "ui_layout-page.h"

LayoutPage::LayoutPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayoutPage)
{
    ui->setupUi(this);
}

LayoutPage::~LayoutPage()
{
    delete ui;
}
