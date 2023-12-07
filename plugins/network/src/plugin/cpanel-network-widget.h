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

    QStringList subItemNameList();
    void setCurrentSubItem(int index);

private slots:
    void removeDevice(const QString &devicePath);
    void changeTheme(Kiran::PaletteType paletteType);

    void changeSideBarItem(QListWidgetItem *item);
    void addWirelessDevice(const QString &devicePath);
    void addWiredDevice(const QString &devicePath);

    void handleWirelessEnabledChanged(bool enable);
    void handleNetworkingEnabledChanged(bool enable);
    void changeDeviceState(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);

private:
    void init();
    void initPage();
    void initConnect();
    void initWiredManager();
    void initWirelessManager();

    void updateSidebarItemStatus(NetworkManager::Device::Type deviceType);
    void setSidebarItemStatus(KiranSidebarItem *sidebarItem, NetworkManager::Device::State state);
    void removeSidebarItem(const QString &text,int index);

    bool isExistWiredItem();
    bool isExistWirelessItem();

signals:
    void subItemsChanged();

private:
    Ui::CPanelNetworkWidget *ui;
    QStringList m_subItemsList;
    QList<KiranSidebarItem *> m_kiranSidebarItems;
    QString m_addDevicePath;
};

#endif  // KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H
