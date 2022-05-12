/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-cpanel-power is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_
#define KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_

#include <kcp-plugin-interface.h>
#include <QTranslator>

class KcpPowerInterface : public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    ~KcpPowerInterface();

    int init() override;
    void uninit() override;
    QWidget* getSubItemWidget(QString id) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QWidget* m_currentWidget;
    QTranslator* m_translator = nullptr;
};

#endif  //KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_
