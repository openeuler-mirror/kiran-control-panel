/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef KIRAN_CONTROL_PANEL_CONNECTION_DETAILS_WIDGET_H
#define KIRAN_CONTROL_PANEL_CONNECTION_DETAILS_WIDGET_H

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Device>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionDetailsWidget;
}
QT_END_NAMESPACE

class ConnectionDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionDetailsWidget(NetworkManager::Device::Ptr device, QWidget *parent = nullptr);
    ~ConnectionDetailsWidget() override;
    
private:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

private:
    void init();
    void initUI();
    void setWirelessSpecificDetails();
    void setIpDetails();

private:
    Ui::ConnectionDetailsWidget *ui;
    NetworkManager::Device::Ptr m_device;
    NetworkManager::ActiveConnection::Ptr m_activeConnection;
    NetworkManager::Connection::Ptr m_connection;
};

#endif  // KIRAN_CONTROL_PANEL_CONNECTION_DETAILS_WIDGET_H
