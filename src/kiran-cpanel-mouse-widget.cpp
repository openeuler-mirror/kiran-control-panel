#include "kiran-cpanel-mouse-widget.h"
#include "ui_kiran-cpanel-mouse-widget.h"

KiranCPanelMouseWidget::KiranCPanelMouseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCPanelMouseWidget)
{
    ui->setupUi(this);
}

KiranCPanelMouseWidget::~KiranCPanelMouseWidget()
{
    delete ui;
}
