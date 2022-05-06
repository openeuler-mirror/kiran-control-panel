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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessNetwork>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionShowPage;
}
QT_END_NAMESPACE
using namespace NetworkManager;
class KiranSwitchButton;

struct WirelessConnectionInfo
{
    int signalStrength;
    QString accessPointPath;
    QString ssid;
    bool securitySetting;
};

struct ConnectionInfo
{
    QString uuid;
    QString connectionPath;
    QString devicePath;
    QString activeConnectionPath;
    bool isWireless = false;
    WirelessConnectionInfo wirelessInfo;
};

Q_DECLARE_METATYPE(ConnectionInfo);
Q_DECLARE_METATYPE(NetworkManager::Status)


class ConnectionShowPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionShowPage(QWidget *parent = nullptr);
    ~ConnectionShowPage() override;
    void initUI();
    void initConnect();

    void setTitle(QString title);
    void setSwitchButtonVisible(bool visible);
    void setCreateButtonVisible(bool visible);

    //在kf5-NetworkManager-qt中Connection是指具体的网络配置，不是指已经存在的网络
    void showConnectionLists(ConnectionSettings::ConnectionType type);
    void addConnectionToLists(Connection::Ptr ptr,const QString &devicePath);

    //在kf5-NetworkManager-qt中Network是指存在的网络
    void showWirelessNetworkLists();
    void addWirelessNetworkToLists(WirelessNetwork::Ptr network,const QString &devicePath);
    void addOtherWirelessItemToLists();

    void removeConnectionFromLists(const QString &path);
    void removeWirelessNetworkFromLists(const QString &ssid);
    void updateItemActivatedPath(QListWidgetItem *item, QString activatedPath = "");
    void findItemByUuid(const QString &uuid);
    void findItemBySsid(const QString &ssid);



public slots:
    void clearConnectionLists();
    void handleConnectionItemClicked(QListWidgetItem *item);
    void updateActivatedConnectionInfo(QString activatedPath);
    void clearDeactivatedConnectionInfo(const QString &deactivatedPath);
    void connectionStateNotify(ActiveConnection::State state);

    void connectionItemLoadingAnimation();

signals:
    void requestCreatConnection();
    void requestEditConnection(const QString &uuid, QString activeConnectionPath);
    void requestActivateCurrentItemConnection(const QString &connectionPath, const QString &connectionParameter = "");
    void requestConnectWirelessNetwork(const ConnectionInfo &connectionInfo);
    void deactivatedItemConnection(const QString &connectionPath);

private:
    Ui::ConnectionShowPage *ui;
    KiranSwitchButton *m_switchButton;
    QListWidgetItem *m_previousActivatedItem;
    QListWidgetItem *m_currentActiveItem;

};

class AnimationLoadingLabel;
class ItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemWidget(QWidget *parent = nullptr);

public:
    void setName(const QString &name);
    QString getName();
    void activatedLabel();
    void deactivateLabel();
    void setLoadingStatus(bool isLoading);
    void setLabelVisible(bool isVisible);
    void setEditButtonVisible(bool isVisible);
    void setWirelessLabel(bool security,int signal);

    QPixmap getPixmapFromSvg(const QString &svgPath);
signals:
    void editConnectionClicked();

private:
    QLabel *m_wirelessLabel;
    QLabel *m_connectionName;
    QHBoxLayout *m_horizonLayout;
    QPushButton *m_editConnection;
    AnimationLoadingLabel *m_activatedLabel;
};


class CustomSortListItem : public QListWidgetItem
{
public:
    explicit CustomSortListItem(QWidget *parent = nullptr);
    bool operator<(const QListWidgetItem &other) const;
};

#endif  //KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
