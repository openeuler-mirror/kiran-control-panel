/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "dialog003.h"
#include "ui_dialog003.h"

Dialog003::Dialog003(QWidget *parent) :
    QDialog(parent),modelbase(),
    ui(new Ui::Dialog003)
{
    ui->setupUi(this);
}

Dialog003::~Dialog003()
{
    delete ui;
}
