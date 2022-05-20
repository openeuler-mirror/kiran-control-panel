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

#ifndef KIRAN_CPANEL_NETWORK_TRAY_PAGE_H
#define KIRAN_CPANEL_NETWORK_TRAY_PAGE_H

#include <NetworkManagerQt/Device>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class TrayPage;
}
QT_END_NAMESPACE
using namespace NetworkManager;
enum TrayConnectionType
{
    TRAY_CONNECTION_TYPE_WIRED,
    TRAT_CONNECTION_TYPE_WIRELESS
};

class TrayPage : public QWidget
{
    Q_OBJECT

public:
    explicit TrayPage(TrayConnectionType trayConnectionType, QWidget *parent = nullptr);
    ~TrayPage() override;

    void init();
    void initUI();
    void initConnection();
    void initWiredPage();
    void initWirelessPage();
    void getDeviceList(Device::Type deviceType);
    void setMultiWiredDeviceWidget();
    QStringList devicePathList();
    int pageHeight();

public slots:
    void handleDeviceComboBoxChanged(int index);
//    void handleDeviceAdded(const QString &devicePath);
//    void handleDeviceRemoved(const QString &devicePath);

private:
    Ui::TrayPage *ui;
    TrayConnectionType m_trayConnectionType;
    QList<Device::Ptr> m_deviceList;
};

#endif  // KIRAN_CPANEL_NETWORK_TRAY_PAGE_H
