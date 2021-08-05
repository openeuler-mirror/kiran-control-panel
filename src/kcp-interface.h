/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-display is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */
#ifndef __KCP_INTERFACE_H__
#define __KCP_INTERFACE_H__

#include <kcp-plugin-interface.h>
#include "kiran-display-module-base.h"

class QTranslator;
class KcpInterface : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpInterface();
    ~KcpInterface();
    int init() override;
    void uninit() override;
    QWidget *getSubItemWidget(QString id) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QTranslator* m_translator = nullptr;
};

#endif  // __KCP_INTERFACE_H__
