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
* Author:     luoqing <luoqing@kylinsec.com.cn>
 */
#ifndef KIRAN_CPANEL_NETWORK_NETWORK_DETAILS_H
#define KIRAN_CPANEL_NETWORK_NETWORK_DETAILS_H

#include <QWidget>
#include <NetworkManagerQt/Manager>
using namespace NetworkManager;

QT_BEGIN_NAMESPACE
namespace Ui
{
class NetworkDetails;
}
QT_END_NAMESPACE

class NetworkDetails : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkDetails(QWidget *parent = nullptr);
    ~NetworkDetails() override;

    void initUI();
    void getAvailableDeviceList();

private:
    Ui::NetworkDetails *ui;
    QList<Device::Ptr> m_wiredDeviceList;
    QList<Device::Ptr> m_wirelessDeviceList;
};

#endif  // KIRAN_CPANEL_NETWORK_NETWORK_DETAILS_H
