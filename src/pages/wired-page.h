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

#ifndef KIRAN_CPANEL_NETWORK_WIRED_PAGE_H
#define KIRAN_CPANEL_NETWORK_WIRED_PAGE_H

#include <NetworkManagerQt/WiredDevice>
#include <QWidget>
#include "page.h"

using namespace NetworkManager;
QT_BEGIN_NAMESPACE
namespace Ui
{
class WiredPage;
}
QT_END_NAMESPACE

class WiredPage : public Page
{
    Q_OBJECT

public:
    explicit WiredPage(QWidget *parent = nullptr);
    ~WiredPage() override;

    void initUI();
    void initConnection();

public slots:
    void showWiredConnections();
    void handleRequestActivateConnection(QString connectionPath);

    void handleStateDeactivated(const QString &deactivatedPath);
    void handleStateActivated(QString activatedPath);
    void handleActiveConnectionStateChanged(ActiveConnection::State state,const QString &path);

    void handleReturnPreviousPage();

    void getDeviceInfo();
    void refreshConnectionLists() override;
    void handleNotifierConnectionAdded(const QString &path) override;
    void handleNotifierConnectionRemoved(const QString &path) override;

    void handleActiveConnectionAdded(const QString &path) override;
    void handleActiveConnectionRemoved(const QString &path) override;
private:
    Ui::WiredPage *ui;
    WiredDevice::Ptr m_wiredDevice;
    QMap<QString, QString> m_deviceMap;
};

#endif  //KIRAN_CPANEL_NETWORK_WIRED_PAGE_H
