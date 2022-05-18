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
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
using namespace NetworkManager;
class ConnectionLists;
class TrayPage : public QWidget
{
    Q_OBJECT

public:
    explicit TrayPage(QWidget *parent = nullptr);
    ~TrayPage();

    void init();
    void initUI();
    void initConnection();
    void setMultiDeviceWidgetVisible(bool visible);
    void setDeviceLabel(const QString &label);
    void getDeviceList(Device::Type deviceType);

    QPointer<ConnectionLists> getConnectionListsPtr();
public slots:
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    virtual void handleActiveConnectionStateChanged(ActiveConnection::State state, const QString &path);
    virtual void handleStateActivated(const QString &activatedPath);
    virtual void handleStateDeactivated(const QString &deactivatedPath);

protected:
    QList<Device::Ptr> m_deviceList;

private:
    QVBoxLayout *m_verticalLayout;
    QWidget *m_multiDevicewidget;
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
