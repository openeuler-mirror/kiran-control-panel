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
#ifndef KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H
#define KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H

#include <NetworkManagerQt/Manager>
#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DetailsPage;
}
QT_END_NAMESPACE

class DetailsPage : public QWidget
{
    Q_OBJECT

public:
    explicit DetailsPage(QWidget *parent = nullptr);
    ~DetailsPage() override;

private slots:
    void handleActivatedConnectionComboBoxActivated(int index);
    void handleActiveConnectionAdded(const QString &activeConnectionPath);
    void handleActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);

    void deviceStateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);
    void changeIpV4Config();
    void changeIpV6Config();
    void changeDhcp4Config();
    void changeDhcp6Config();

    void updateDetails();

private:
    void initUI();
    void initMultiConnectionDetailsWidget();
    void initConnect();
    void clear();
    void reload();
    void initDeviceConnect();

private:
    Ui::DetailsPage *ui;
    NetworkManager::Device::List m_deviceList;
    NetworkManager::ActiveConnection::List m_activatedConnectionList;
    QTimer m_reloadTimer;
};

#endif  // KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H
