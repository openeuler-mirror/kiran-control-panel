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

#include <QTimer>
#include "plugin-interface-v2.h"
#include "subitem.h"

class TestPlugin : public QObject, public KiranControlPanel::PluginInterfaceV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterfaceV2_iid)
    Q_INTERFACES(KiranControlPanel::PluginInterfaceV2)

public:
    TestPlugin() = default;
    ~TestPlugin() = default;

    int init(KcpInterface* interface) override;
    void uninit() override;
    QVector<KiranControlPanel::SubItemPtr> getSubItems() override;

private slots:
    void handleTimerTimeout();

private:
    QTimer m_timer;
    KcpInterface* m_controlPanelInterface = nullptr;
    QVector<KiranControlPanel::SubItemPtr> m_subitems;
};