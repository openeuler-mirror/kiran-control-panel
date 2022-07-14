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
#ifndef TRAYPAGE_H
#define TRAYPAGE_H

#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QStackedWidget>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>

using namespace NetworkManager;
class ConnectionLists;
class StatusNotification;
Q_DECLARE_METATYPE(ActiveConnection::State);

class TrayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayWidget(QWidget *parent = nullptr);
    ~TrayWidget();

    void init();
    void initUI();
    void initConnection();

    void getDeviceList(Device::Type deviceType);

public slots:
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    virtual void handleActiveConnectionStateChanged(ActiveConnection::State state);
    virtual void handleStateActivating(const QString &activatedPath);
    virtual void handleStateActivated(const QString &activatedPath);
    virtual void handleStateDeactivated(const QString &activatedPath);

    virtual void handleDeviceStateChanged(Device::State newstate, Device::State oldstate, Device::StateChangeReason reason);

    void distributeNotifeir();

protected:
    QList<Device::Ptr> m_deviceList;
    Device::Ptr m_devicePtr;
    StatusNotification m_statusNotification;
private:
    QTimer m_connectionRemovedTimer;
    QTimer m_StateActivatedTimer;
    QString m_connectionRemovePath;
    QString m_activatedPath;
};

#endif  // TRAYPAGE_H
