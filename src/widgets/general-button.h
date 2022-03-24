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

#ifndef KIRAN_CPANEL_NETWORK_GENERAL_BUTTON_H
#define KIRAN_CPANEL_NETWORK_GENERAL_BUTTON_H

#include <QWidget>
#include <NetworkManagerQt/Connection>
QT_BEGIN_NAMESPACE
namespace Ui
{
class GeneralButton;
}
QT_END_NAMESPACE
using namespace NetworkManager;

enum SettingConnectionStatus
{
    SETTING_CONNECTION_STATUS_NEW,
    SETTING_CONNECTION_STATUS_DEACTIVATED,
    SETTING_CONNECTION_STATUS_ACTIVATED
};

class GeneralButton : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralButton(QWidget *parent = nullptr);
    ~GeneralButton() override;
    void initUI();
    void initConnection();
    void setConnectionPtr(const Connection::Ptr &connection);

public slots:
    void handleDeleteConnection();
    void initButton(SettingConnectionStatus connectionStatus,const QString& activeConnectionPath="");
    void clearPtr();
signals:

private:
    Ui::GeneralButton *ui;
    Connection::Ptr m_connection;
    QString m_activeConnectionPath;
};

#endif  //KIRAN_CPANEL_NETWORK_GENERAL_BUTTON_H
