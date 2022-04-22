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

#ifndef KIRAN_CPANEL_NETWORK_PAGE_H
#define KIRAN_CPANEL_NETWORK_PAGE_H
#include <QWidget>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
enum EditPages
{
    PAGE_SHOW,
    PAGE_SETTING
};
using namespace NetworkManager;

class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent= nullptr);
    ~Page();

    void initNotifierConnection();

public slots:
    virtual void refreshConnectionLists();
    virtual void handleNotifierConnectionAdded(const QString &path);
    virtual void handleNotifierConnectionRemoved(const QString &path);
    virtual void handleActiveConnectionAdded(const QString &activepath);
    virtual void handleActiveConnectionRemoved(const QString &activepath);

    void getDeviceInfo(Device::Type deviceType);
protected:
    QMap<QString, QString> m_deviceMap;
};

#endif  //KIRAN_CPANEL_NETWORK_PAGE_H
