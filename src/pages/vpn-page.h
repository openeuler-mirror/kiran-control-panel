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

#ifndef KIRAN_CPANEL_NETWORK_VPN_PAGE_H
#define KIRAN_CPANEL_NETWORK_VPN_PAGE_H

#include <QWidget>
#include "page.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class VpnPage;
}
QT_END_NAMESPACE

class VpnPage : public Page
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = nullptr);
    ~VpnPage() override;

    void initUI();
    void initConnection();

public slots:
    void showVpnConnections();
    void clearVpnSetting();
    void refreshConnectionLists() override;
    void handleRequestEditConnection(const QString &uuid, QString activeConnectionPath);
    void handleActivateConnection(QString connectionPath);
    void handleNotifierConnectionChanged() override;
private:
    Ui::VpnPage *ui;
};

#endif  //KIRAN_CPANEL_NETWORK_VPN_PAGE_H
