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

#ifndef KIRAN_CPANEL_SIGNAL_FORWARD_H
#define KIRAN_CPANEL_SIGNAL_FORWARD_H

#include <QObject>
class SignalForward : public QObject
{
    Q_OBJECT

private:
    explicit SignalForward(QObject *parent = nullptr);

public:
    static SignalForward *instance();
    ~SignalForward();

    void initConnect();
public slots:
    void handleActiveConnectionAdded(const QString &activepath);
    void handleNotifierConnectionAdded(const QString &path);

    void handleNotifierConnectionRemoved(const QString &path);
    void handleActiveConnectionRemoved(const QString &activepath);

signals:
    void wiredConnectionAdded(const QString &path);
    void vpnConnectionAdded(const QString &path);
    void wirelessConnectionAdded(const QString &path);

    void wiredConnectionRemoved(const QString &path);
    void vpnConnectionRemoved(const QString &path);
    void wirelessConnectionRemoved(const QString &path);
    void connectionRemoved(const QString &path);

    void wiredActiveConnectionAdded(const QString &activepath);
    void vpnActiveConnectionAdded(const QString &activepath);
    void wirelessActiveConnectionAdded(const QString &activepath);

    void wiredActiveConnectionRemoved(const QString &activepath);
    void vpnActiveConnectionRemoved(const QString &activepath);
    void wirelessActiveConnectionRemoved(const QString &activepath);
    void activeConnectionRemoved(const QString &activepath);
};

#endif  // KIRAN_CPANEL_SIGNAL_FORWARD_H