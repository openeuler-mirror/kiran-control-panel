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

#include <QList>
#include <QObject>
#include <QSharedPointer>

#include <panel-interface.h>
#include <plugin-subitem-interface.h>
#include <QVector>

// 控制中心插件接口第二版
// 插件通过实现该接口，编译成共享库，放置在指定位置，主面板进行加载显示
namespace KiranControlPanel
{
class PluginInterfaceV2
{
public:
    virtual ~PluginInterfaceV2(){};

public:
    //主面板调用该接口初始化该插件，插件可在其中进行部分初始化操作，例如安装翻译等操作
    //成功返回0
    virtual int init(PanelInterface* interface) = 0;

    //主面板调用该接口取消掉该插件初始化做的操作并卸载该插件
    virtual void uninit() = 0;

    // 功能项数组
    // 功能项发生变更时，应调用init时传入KcpInterface接口，通知主面板功能项发生变更,及时同步功能项
    virtual QVector<KiranControlPanel::SubItemPtr> getSubItems() = 0;
};
}  // namespace KiranControlPanel

#define KiranControlPanel_PluginInterfaceV2_iid "com.kylinsec.Kiran.ControlPanel.PluginInterface/2.4"
Q_DECLARE_INTERFACE(KiranControlPanel::PluginInterfaceV2, KiranControlPanel_PluginInterfaceV2_iid)
