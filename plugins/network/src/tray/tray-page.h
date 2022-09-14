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

class TrayPage : public QWidget
{
    Q_OBJECT

public:
    explicit TrayPage(NetworkManager::Device::List deviceList, QWidget *parent = nullptr);
    ~TrayPage() override;

    void init();
    void initUI();
    void initWiredTrayWidget(const QString &devicePath);
    void initWirelessTrayWidget(const QString &devicePath);
    void initConnection();

    void setMultiDeviceWidget();
    void setSingleDeviceWidget();
    QStringList devicePathList();

public slots:
    void handleDeviceComboBoxChanged(int index);
    void handleAdjustedTraySize(QSize sizeHint);

signals:
    void sizeChanged(QSize sizeHint);

private:
    Ui::TrayPage *ui;
    QList<NetworkManager::Device::Ptr> m_deviceList;
};

#endif  // KIRAN_CPANEL_NETWORK_TRAY_PAGE_H
