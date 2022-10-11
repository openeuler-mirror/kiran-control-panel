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
#ifndef TRAYPAGE_H
#define TRAYPAGE_H

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <QPointer>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>

Q_DECLARE_METATYPE(NetworkManager::ActiveConnection::State);

class TrayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayWidget(QWidget *parent = nullptr);
    ~TrayWidget();

    void init();
    void initUI();
    void initConnection();

    void getDeviceList(NetworkManager::Device::Type deviceType);

public slots:
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    virtual void handleActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    virtual void handleStateActivating(const QString &activatedPath);
    virtual void handleStateActivated(const QString &activatedPath);
    virtual void handleStateDeactivated(const QString &activatedPath);

    virtual void handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

    void distributeNotifeir();

protected:
    QList<NetworkManager::Device::Ptr> m_deviceList;
    NetworkManager::Device::Ptr m_devicePtr;

private:
    QTimer m_connectionRemovedTimer;
    QTimer m_StateActivatedTimer;
    QString m_connectionRemovePath;
    QString m_activatedPath;
};

#endif  // TRAYPAGE_H
