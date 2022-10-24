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

#ifndef KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H
#define KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H

#include <NetworkManagerQt/Device>
#include <QTimer>
#include <QWidget>
#include "kiran-sidebar-widget.h"
#include "style-palette.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelNetworkWidget;
}
QT_END_NAMESPACE

class KiranSidebarItem;
class CPanelNetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CPanelNetworkWidget(QWidget *parent = nullptr);

    ~CPanelNetworkWidget() override;

    void init();
    void initSubItemsList();
    void getAvailableDeviceList();
    void initPage();
    void initConnect();
    void setSidebarItemStatus(KiranSidebarItem *sidebarItem, NetworkManager::Device::State state);

    void reload();

public slots:
    void handleDeviceAdded(const QString &devicePath);
    void handleDeviceRemoved(const QString &devicePath);
    void handleThemeChanged(Kiran::PaletteType paletteType);
    void handleSideBarItemClicked(QListWidgetItem *item);
    void handleManagedChanged();
    void handleWirelessEnabledChanged(bool enable);
    void handleNetworkingEnabledChanged(bool enable);
    void handleStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

signals:
    void subItemsChanged();

private:
    Ui::CPanelNetworkWidget *ui;
    QList<NetworkManager::Device::Ptr> m_wiredDeviceList;
    QList<NetworkManager::Device::Ptr> m_wirelessDeviceList;
    QStringList m_subItemsList;
    QMap<QString, KiranSidebarItem *> m_deviceToSidebarItem;
    QTimer m_Timer;
    QString m_addDevicePath;
    int m_waitCounts;
};

#endif  // KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H
