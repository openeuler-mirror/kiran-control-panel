#include "system-information-widget.h"
#include "ui_system-information-widget.h"

SystemInformationWidget::SystemInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemInformationWidget)
{
    ui->setupUi(this);
    initUI();
}

SystemInformationWidget::~SystemInformationWidget()
{
    delete ui;
}

void SystemInformationWidget::initUI()
{

}
