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
#ifndef KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H
#define KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H

#include <style-palette.h>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include "general.h"
#include "kiran-rounded-tray-popup/kiran-rounded-tray-popup.h"

class WiredTrayWidget;
class WirelessTrayWidget;
class StatusNotifierManagerInterface;
class TrayPage;
class QTcpSocket;
class QVBoxLayout;

class NetworkTray : public KiranRoundedTrayPopup
{
    Q_OBJECT
public:
    explicit NetworkTray(QWidget *parent = nullptr);
    ~NetworkTray() override;

private:
    void init();

    void initConnect();
    void initUI();
    void initTrayIcon();
    void initMenu();
    void initTrayPage();

    void initUnavailableWidget();

    void getTrayGeometry();
    void setTrayPagePos();

    void UnavailableTrayPage();
    void reloadWiredTrayPage();
    void reloadWirelessTrayPage();

    void updateTrayIcon();
    void setTrayIcon(NetworkState state);
    void setTrayIcon(const QString &iconPath, const QString &toolTip);
    void initTcpSocket();
    void checkInternetConnectivity();

private slots:
    void handleTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void showOrHideTrayPage();
    void handleNetworkSettingClicked();

    void handleDeviceAdded(const QString &devicePath);
    void handleDeviceRemoved(const QString &devicePath);
    void handleNetworkManagerStatusChanged(NetworkManager::Status status);
    void handlePrimaryConnectionChanged(const QString &uni);

    void handleDeviceStateChanged(NetworkManager::Device::State newstate,
                                  NetworkManager::Device::State oldstate,
                                  NetworkManager::Device::StateChangeReason reason);
    void handleCarrierChanged();
    void handleDeviceManagedChanged();
    void handleWirelessEnabledChanged(bool enable);

    void handleAdjustedTraySize(QSize sizeHint);

    void handleThemeChanged(Kiran::PaletteType paletteType);
    void internetConnected();
    void internetError(QAbstractSocket::SocketError socketError);

private:
    QSystemTrayIcon *m_systemTray;
    QMenu *m_menu;
    QAction *m_networkSetting;

    TrayPage *m_wiredTrayPage;
    TrayPage *m_wirelessTrayPage;
    QWidget *m_unavailableWidget;

    StatusNotifierManagerInterface *m_statusNotifierManager;

    QList<NetworkManager::Device::Ptr> m_wiredDeviceList;
    QList<NetworkManager::Device::Ptr> m_wirelessDeviceList;

    QVBoxLayout *m_verticalLayout;
    int m_xTray, m_yTray, m_heightTray, m_widthTray;

    QTimer m_Timer;
    QTimer m_wirelessTimer;
    QString m_addDevicePath;
    int m_waitCounts;
    QSize m_wirelessTraySizeHint;
    QTcpSocket *m_tcpClient;

    struct CheckConnectivityContext {
        bool enable = false;
        QString address;
        quint64 port;
    } m_checkConnectivityContext;
    bool m_checkWiredCarrier = false;
};

#endif  // KIRAN_CPANEL_NETWORK_MANAGER_TRAY_H
