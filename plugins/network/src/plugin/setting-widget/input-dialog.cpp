/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#include "input-dialog.h"
#include "ui_input-dialog.h"
#include <qt5-log-i.h>

InputDialog::InputDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    init();

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    ui->confirmButton->setEnabled(false);
}

InputDialog::~InputDialog()
{
    delete ui;
}

void InputDialog::init()
{
    initConnection();
}

void InputDialog::initConnection()
{
    connect(ui->passwordEdit,&QLineEdit::textEdited,[=](){
        if(ui->passwordEdit->text().isEmpty())
            ui->confirmButton->setEnabled(false);
        else
            ui->confirmButton->setEnabled(true);
    });

    connect(ui->confirmButton,&QPushButton::clicked,[=](){
        emit password(ui->passwordEdit->text());
        this->close();
    });

    connect(ui->cancelButton,&QPushButton::clicked,[=](){
        this->close();
    });
}

void InputDialog::setTitle(const QString &title)
{
    ui->titleLabel->setText(title);
}
void InputDialog::setText(const QString &text)
{
    ui->textLabel->setText(text);
}
