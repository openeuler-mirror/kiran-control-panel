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

#ifndef KIRAN_CPANEL_NETWORK_DISCONNECT_AND_DELETE_BUTTON_H
#define KIRAN_CPANEL_NETWORK_DISCONNECT_AND_DELETE_BUTTON_H

#include <QWidget>
#include <NetworkManagerQt/Connection>
QT_BEGIN_NAMESPACE
namespace Ui
{
class DisconnectAndDeleteButton;
}
QT_END_NAMESPACE


enum SettingConnectionStatus
{
    SETTING_CONNECTION_STATUS_NEW,
    SETTING_CONNECTION_STATUS_DEACTIVATED,
    SETTING_CONNECTION_STATUS_ACTIVATED
};

class DisconnectAndDeleteButton : public QWidget
{
    Q_OBJECT

public:
    explicit DisconnectAndDeleteButton(QWidget *parent = nullptr);
    ~DisconnectAndDeleteButton() override;
    void initUI();
    void initConnection();
    void setConnectionPtr(const NetworkManager::Connection::Ptr &connection);

public slots:
    void handleDeleteConnection();
    void handleIgnoreWireless();

    void initButton(SettingConnectionStatus connectionStatus,const QString& activeConnectionPath="");
    void clearPtr();
signals:
    void disconnectButtonClicked();
    void deleteButtonClicked();

private:
    Ui::DisconnectAndDeleteButton *ui;
    NetworkManager::Connection::Ptr m_connection;
    QString m_activeConnectionPath;
};

#endif  // KIRAN_CPANEL_NETWORK_DISCONNECT_AND_DELETE_BUTTON_H
