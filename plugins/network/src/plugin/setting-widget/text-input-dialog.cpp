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

#include "text-input-dialog.h"
#include <qt5-log-i.h>

TextInputDialog::TextInputDialog(QWidget *parent) : KiranMessageBox(parent)
{
    init();
    m_confirmButton->setEnabled(false);
}

TextInputDialog::~TextInputDialog()
{
}

void TextInputDialog::init()
{
    initUI();
    initConnection();
}

void TextInputDialog::initUI()
{
    m_lineEdit = new QLineEdit(this);
    addWidgetToDialog(m_lineEdit, Qt::AlignVCenter);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setTitle(tr("Tips"));
    //    setText(tr("Please input password"));

    m_confirmButton = new QPushButton(tr("Yes"));
    m_cancelButton = new QPushButton(tr("Cancel"));
    m_confirmButton->setAccessibleName(QString("ButtonConfirm"));
    m_cancelButton->setAccessibleName(QString("ButtonCancel"));
    addButton(m_confirmButton, QDialogButtonBox::AcceptRole);
    addButton(m_cancelButton, QDialogButtonBox::RejectRole);
}

void TextInputDialog::initConnection()
{
    connect(m_lineEdit, &QLineEdit::textEdited, this, [this]()
            {
                if(m_lineEdit->text().isEmpty())
                    m_confirmButton->setEnabled(false);
                else
                    m_confirmButton->setEnabled(true); });

    connect(m_lineEdit, &QLineEdit::returnPressed, this, &TextInputDialog::handleConfirmButtonClicked);
    connect(m_confirmButton, &QPushButton::clicked, this, &TextInputDialog::handleConfirmButtonClicked);

    connect(m_cancelButton, &QPushButton::clicked, this, [this]()
            { this->close(); });
}

void TextInputDialog::setlineEditEchoMode(QLineEdit::EchoMode echoMode)
{
    m_lineEdit->setEchoMode(echoMode);
}

void TextInputDialog::handleConfirmButtonClicked()
{
    if (m_lineEdit->echoMode() == QLineEdit::Password)
        emit password(m_lineEdit->text());
    else
        emit ssid(m_lineEdit->text());
    this->close();
}
