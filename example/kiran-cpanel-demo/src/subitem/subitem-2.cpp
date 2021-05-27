/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "subitem-2.h"
#include "ui_subitem-2.h"

SubItem2::SubItem2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubItem2)
{
    ui->setupUi(this);
}

SubItem2::~SubItem2()
{
    delete ui;
}

void SubItem2::on_lineEdit_textChanged(const QString &arg1)
{
    m_unsaved = true;
}

bool SubItem2::unsaved()
{
    return m_unsaved;
}
