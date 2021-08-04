/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

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
