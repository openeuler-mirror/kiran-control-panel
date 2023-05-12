/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include "ksd_power_proxy.h"

class GnomeSMProxy;
class PowerInterface : public KSDPowerProxy
{
    Q_OBJECT
private:
    PowerInterface(const QString &service, const QString &path,
                   const QDBusConnection &connection, QObject *parent = nullptr);

public:
    static PowerInterface *getInstance() { return m_instance; };
    static void globalInit();
    static void globalDeinit() { delete m_instance; };

    // 将kiran-session-manager提供的接口
    bool screenLockedWhenSuspend();
    bool LockScreenWhenSuspend(bool checked);

private:
    static PowerInterface *m_instance;
    GnomeSMProxy *m_gnomeSmProxy;
};