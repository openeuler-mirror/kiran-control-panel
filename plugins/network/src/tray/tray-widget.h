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
    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);
    virtual int getHeight() = 0;

public slots:
    virtual void handleNotifierConnectionAdded(const QString &path) = 0;
    virtual void handleNotifierConnectionRemoved(const QString &path) = 0;
    virtual void handleActiveConnectionAdded(const QString &activepath) = 0;
    virtual void handleActiveConnectionRemoved(const QString &activepath) = 0;

    virtual void handleStateActivating(const QString &activatedPath) = 0;
    virtual void handleStateActivated(const QString &activatedPath) = 0;
    virtual void handleStateDeactivated(const QString &activatedPath) = 0;

    virtual void handleActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);

    virtual void handleDeviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

signals:
    void sizeChanged(QSize sizeHint);

protected:
    NetworkManager::Device::Ptr m_devicePtr;

private:
    QTimer m_connectionRemovedTimer;
    QTimer m_StateActivatedTimer;
    QString m_connectionRemovePath;
    QString m_activatedPath;
    QVBoxLayout *m_verticalLayout;
};

#endif  // TRAYPAGE_H
