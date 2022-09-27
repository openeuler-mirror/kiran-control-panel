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

#ifndef KIRAN_CPANEL_NETWORK_INPUT_DIALOG_H
#define KIRAN_CPANEL_NETWORK_INPUT_DIALOG_H

#include <kiran-message-box.h>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class TextInputDialog : public KiranMessageBox
{
    Q_OBJECT

public:
    explicit TextInputDialog(QWidget *parent = nullptr);
    ~TextInputDialog() override;

    void init();
    void initUI();
    void initConnection();
    void setlineEditEchoMode(QLineEdit::EchoMode);

public slots:
    void handleConfirmButtonClicked();

signals:
    void password(const QString &password);
    void ssid(const QString &ssid);

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_confirmButton;
    QPushButton *m_cancelButton;
};

#endif  // KIRAN_CPANEL_NETWORK_INPUT_DIALOG_H
