/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "dialog001.h"
#include "ui_dialog001.h"

Dialog001::Dialog001(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog001)
{
    ui->setupUi(this);
}

Dialog001::~Dialog001()
{
    delete ui;
}

void Dialog001::on_lineEdit_textChanged(const QString &arg1)
{

}
