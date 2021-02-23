/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "dialog001.h"
#include "ui_dialog001.h"
#include "popup.h"

Dialog001::Dialog001(QWidget *parent) :
    QDialog(parent), gHasUnsavedOptions(false),
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
    gHasUnsavedOptions = true;
}

void Dialog001::on_pushButton_clicked()
{
    Popup popup;
    popup.exec();
}
