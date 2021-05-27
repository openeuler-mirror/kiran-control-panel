/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "subitem-1.h"
#include "popup.h"
#include "ui_subitem-1.h"

SubItem1::SubItem1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubItem1)
{
    ui->setupUi(this);
}

SubItem1::~SubItem1()
{
    delete ui;
}

void SubItem1::on_lineEdit_textChanged(const QString &arg1)
{
    m_unsaved = true;

}

void SubItem1::on_pushButton_clicked()
{
    Popup popup;
    popup.exec();
}

bool SubItem1::unsaved()
{
    return m_unsaved;
}
