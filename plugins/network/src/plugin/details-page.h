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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */
#ifndef KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H
#define KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H

#include <NetworkManagerQt/Manager>
#include <QWidget>
using namespace NetworkManager;

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

    void initUI();
    void initMultiConnectionDetailsWidget();
    void initConnect();
    void clear();
    void reload();

public slots:
    void handleActivatedConnectionComboBoxChanged(int index);
    void handleDeviceAdded(const QString &devicePath);
    void handleDeviceRemoved(const QString &devicePath);
    void handleActiveConnectionAdded(const QString &activeConnectionPath);
    void handleActiveConnectionRemoved(const QString &activeConnectionPath);
    void handleActiveConnectionChanged();
    void handleActiveConnectionStateChanged(ActiveConnection::State state);

private:
    Ui::DetailsPage *ui;
    Device::List m_deviceList;
    ActiveConnection::List m_activatedConnectionList;
};

#endif  // KIRAN_CPANEL_NETWORK_DETAILS_PAGE_H
