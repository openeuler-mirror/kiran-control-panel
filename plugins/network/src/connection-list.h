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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessNetwork>
#include <QList>
#include <QObject>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

struct WirelessConnectionInfo
{
    int signalStrength;
    QString accessPointPath;
    QString ssid;
    bool securitySetting;
};

struct NetworkConnectionInfo
{
    QString id;
    QString uuid;
    QString connectionPath;
    QString devicePath;
    QString activeConnectionPath;
    bool isWireless = false;
    WirelessConnectionInfo wirelessInfo;
};

Q_DECLARE_METATYPE(NetworkConnectionInfo)
Q_DECLARE_METATYPE(NetworkManager::Status)

class ConnectionList : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionList(QWidget *parent = nullptr);
    ~ConnectionList();

    void initUI();
    void initConnect();
    void setDevicePath(const QString &devicePath);
    void addWidget(QWidget *widget);
    QWidget *itemWidget(int row);
    QList<QWidget *> itemWidgetList();

    void showConnectionList(NetworkManager::ConnectionSettings::ConnectionType type);
    void showWirelessNetworkList();

    virtual void addConnection(NetworkManager::Connection::Ptr ptr, const QString &devicePath);
    virtual void addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr network,
                                    const QString &devicePath);
    virtual void addHiddenNetworkItemWidget();
    virtual void removeConnectionFromList(const QString &path);
    virtual void removeWirelessNetworkFromList(const QString &ssid);

    virtual void clearItemWidgetActiveConnectionInfo(QWidget *itemWidget);
    void updateItemWidgetActivePath(QWidget *itemWidget,
                                    QString activePath = "");
    void clearConnectionList();

    QWidget *findItemWidgetByUuid(const QString &uuid);
    QWidget *findItemWidgetBySsid(const QString &ssid);
    QWidget *findItemWidgetByActivePath(const QString &activePath);

    int count();
    void setWidgetContentsSpacing(int spacing);
    int widgetContentsSpacing();

    void setWidgetContentsMargins(int left, int top, int right, int bottom);
    QMargins contentsMargins() const;

    void sort();
    static bool lessThan(const QPair<QWidget *, int> &left,
                         const QPair<QWidget *, int> &right);
    static bool greaterThan(const QPair<QWidget *, int> &left,
                            const QPair<QWidget *, int> &right);
    static bool lessThanString(const QPair<QWidget *, QString> &left,
                               const QPair<QWidget *, QString> &right);
    static bool greaterThanString(const QPair<QWidget *, QString> &left,
                                  const QPair<QWidget *, QString> &right);

    void clearContentsWidget();

public slots:
    virtual void handleConnectionItemClicked();
    void handleConnectionUpdated();
    virtual void handleActiveStateDeactivated(const QString &activatedConnectionPath);

signals:
    void connectionUpdated(const QString &path);
    void activateSelectedConnection(const QString &connectionPath,
                                    const QString &connectionParameter = "");

    void activateSelectedWirelessNetwork(const NetworkConnectionInfo &connectionInfo);

private:
    QString m_devicePath;
    QScrollArea *m_scrollArea;
    QVBoxLayout *m_verticalLayout;
    QWidget *m_scrollAreaWidgetContents;
    QVBoxLayout *m_widgetContentsLayout;
    QList<QWidget *> m_itemWidgetList;
    // QVector<QPair<QWidget *, int> > m_sorting;
    // QVector<QPair<QWidget *, QString> > m_sortingString;
};

#endif  // KIRAN_CPANEL_NETWORK_CONNECTION_LISTS_H
