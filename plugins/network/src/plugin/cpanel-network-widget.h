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

#ifndef KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H
#define KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H

#include <QWidget>
#include <NetworkManagerQt/Device>
#include "kiran-sidebar-widget.h"
#include "style-palette.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelNetworkWidget;
}
QT_END_NAMESPACE

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

    void reload();
    QPixmap trayIconColorSwitch(const QString &iconPath);

public slots:
    void handleDeviceAdded(const QString &devicePath);
    void handleDeviceRemoved(const QString &devicePath);
    void handleThemeChanged(Kiran::PaletteType paletteType);
    void handleSideBarItemClicked(QListWidgetItem *item);

signals:
    void subItemsChanged();

private:
    Ui::CPanelNetworkWidget *ui;
    QList<NetworkManager::Device::Ptr> m_wiredDeviceList;
    QList<NetworkManager::Device::Ptr> m_wirelessDeviceList;
    QStringList m_subItemsList;

    QTimer m_Timer;
    QString m_addDevicePath;
    int waitCounts;
};

#endif  //KIRAN_CPANEL_NETWORK_CPANEL_NETWORK_WIDGET_H
