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

#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QStackedWidget>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
using namespace NetworkManager;
class ConnectionLists;
class ConnectionTray : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionTray(QWidget *parent = nullptr);
    ~ConnectionTray();

    void init();
    void initUI();
    void initConnection();

    void getDeviceList(Device::Type deviceType);

public slots:
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    virtual void handleDeviceAdded(const QString &devicePath);
    virtual void handleDeviceRemoved(const QString &devicePath);

    virtual void handleActiveConnectionStateChanged(ActiveConnection::State state);
    virtual void handleStateActivated(const QString &activatedPath);
    virtual void handleStateDeactivated();

protected:
    QList<Device::Ptr> m_deviceList;

private:
    QVBoxLayout *m_verticalLayout;
    QWidget *m_multiDevicewidget;
    QStackedWidget* m_stackedWidget;
    QVBoxLayout *m_verticalDeviceWidgetLayout;
    QLabel *m_deviceLabel;
    QComboBox *m_deviceComboBox;
    QScrollArea *m_scrollArea;
    QWidget *m_scrollAreaWidgetContents;
    QVBoxLayout *m_verticalScrollAreaWidgetContentsLayout;
    ConnectionLists *m_connectionLists;

    QTimer m_connectionTimer;
    QString m_connectionRemovePath;

};

#endif  // TRAYPAGE_H
